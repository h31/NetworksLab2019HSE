package ru.hse.spb.dns.client

import ru.hse.spb.dns.protocol.*
import ru.hse.spb.dns.protocol.Protocol.Request.RequestType.*
import java.io.IOException

private const val GREETING = ":> "
private const val EXIT = "exit"
private const val HELP = "help"

private val HELP_MESSAGE = """Usage:
    <domain> [requestType]
    exit
    help
    -----------------
    Possible request types: WEB (default), EMAIL
"""

private fun greeting() = print(GREETING)

private fun Exception.printCause() {
    if (message != null) {
        println("Cause: $message")
    }
}

private fun String.asRequestType(): Protocol.Request.RequestType? = when (this) {
    "WEB" -> WEB
    "EMAIL" -> EMAIL
    else -> null
}

fun main(args: Array<String>) {
    if (args.size != 1) {
        println("Incorrect arguments. Usage: dnsClient dnsAddress:dnsPort")
        return
    }
    val address = try {
        args[0].validateIsServerAddress()
    } catch (e: ValidationException) {
        println("Incorrect address: ${e.message}")
        return
    }

    println(HELP_MESSAGE)
    loop@ while (true) {
        greeting()
        val command = readLine() ?: continue
        when (command) {
            EXIT -> break@loop
            HELP -> {
                println(HELP_MESSAGE)
                continue@loop
            }
        }
        val tokens = command.split(Regex("\\s+"))

        val domain = tokens[0].asDomain()
        if (domain == null) {
            println("Incorrect domain")
            continue
        }
        val requestType = if (tokens.size < 2) WEB else tokens[1].asRequestType()
        if (requestType == null) {
            println("Unknown request type")
            continue
        }

        try {
            val ip = getAddressOfDomain(domain, requestType, address)
            println("Address of $domain is $ip")
        } catch (e: AddressNotFoundException) {
            println("Address of $domain not found.")
            e.printCause()
        } catch (e: IOException) {
            println("Network error")
            e.printCause()
        }
    }
}