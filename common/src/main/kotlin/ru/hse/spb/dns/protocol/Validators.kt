package ru.hse.spb.dns.protocol

import org.apache.commons.validator.routines.EmailValidator
import org.apache.commons.validator.routines.DomainValidator
import org.apache.commons.validator.routines.InetAddressValidator

class ValidationException(override val message: String) : Exception(message)

fun String.asIp(): Ip? = if (this == "localhost" || InetAddressValidator.getInstance().isValid(this)) this else null

fun String.validateIsIp(): Ip = asIp() ?: throw ValidationException("$this is not valid ip address")

fun String.asDomain(): Domain? = run {
    val domain = removeSuffix(".")
    if (domain.isEmpty() || domain.matches(Regex("([a-zA-Z0-9-_]+\\.)*[a-zA-Z0-9][a-zA-Z0-9-_]+(\\.[a-zA-Z]{2,11})?"))) {
        "$domain."
    } else {
        null
    }
}

fun String.validateIsDomain(): Domain = asDomain() ?: throw ValidationException("$this is not valid domain")

fun String.asPort(): Int? = run {
    val port = this.toIntOrNull()
    if (port in 0..65535) port
    else null
}

fun String.validateIsPort(): Int = asPort() ?: throw ValidationException("$this is not valid port")

fun String.asBoolean(): Boolean? = when (this) {
    "T" -> true
    "F" -> false
    else -> null
}

fun String.validateIsServerAddress(): NameServerAddress = run {
    val errorMessage by lazy { "$this is not valid DNS server address"}
    val tokens = split(":")
    if (tokens.size != 2) {
        throw ValidationException(errorMessage)
    }
    val ip = tokens[0].validateIsIp()
    val port = tokens[1].validateIsPort()
    NameServerAddress(ip, port)
}

fun String.validateIsNumber(): Int = this.toIntOrNull() ?: throw ValidationException("$this is not a number")

fun String.validateNotNegative(fieldName: String): Int = run {
    val field = validateIsNumber()
    if (field < 0) {
        throw ValidationException("$fieldName must be >= 0")
    }
    field
}

fun String.validateIsEmail(): Email = also {
    if (!EmailValidator.getInstance().isValid(this)) {
        throw ValidationException("$this is not an email")
    }
}