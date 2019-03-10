package ru.hse.spb.dns.server

import ru.hse.spb.dns.protocol.*
import java.io.File

/*
1. domain IN SOA ns e-mail Serial Refresh Retry Expire DefaultTTL -- главная запись
2. domain IN NS address -- запись о DNS сервере
3. name IN A address -- аддресная запись
4. сname IN CNAME name -- запись о псевдониме
5. Mail-domain IN MX address -- запись о email сервере

1. ftk.spbstu.ru. IN SOA ns.ftk.spbstu.ru. admin.ftk.spbstu.ru 2008121801 3600 900 2592000 900
2. ftk.spbstu.ru. IN NS 195.19.212.13
3. aivt IN A 195.19.212.16
4. www IN CNAME aivt.ftk.spbstu.ru
5. aivt IN MX aivtmail.ftk.spbstu.ru.
 */

private const val IN = "IN"

internal class ConfigurationParsingException(private val messages: List<String>) : Exception() {
    constructor(message: String?) : this(if (message != null) listOf(message) else emptyList())

    override val message: String
        get() = "Errors:\n${messages.joinToString("\n")}"
}

internal enum class DnsRecordType {
    SOA, NS, A, CNAME, MX
}


private fun String.toDnsRecordType(): DnsRecordType =
    try {
        DnsRecordType.valueOf(this)
    } catch (_: IllegalStateException) {
        throw ConfigurationParsingException("Unknown DNS record: $this")
    }

data class Configuration(
    val domain: Domain,
    val rootNameServer: NameServerAddress,
    val adminEmail: Email,
    val serial: Int,
    val refresh: Int,
    val retry: Int,
    val expire: Int,
    val defaultTTL: Int
)

data class ServerConfiguration(
    val configuration: Configuration,
    val webAddresses: Map<Domain, Ip>,
    val emailAddresses: Map<Domain, Ip>,
    val nameServers: Map<Domain, NameServerAddress>
)

fun File.readConfiguration(): ServerConfiguration {
    var configuration: Configuration? = null
    val webAddresses = mutableMapOf<Domain, Ip>()
    val emailAddresses = mutableMapOf<Domain, Ip>()
    val nameServers = mutableMapOf<Domain, NameServerAddress>()
    val errorMessages = mutableListOf<String>()
    val aliases = mutableListOf<Pair<Domain, Domain>>()
    forEachLine { line ->
        if (line.startsWith("#") || line.isBlank()) {
            return@forEachLine
        }
        try {
            try {
                val tokens = line.split(Regex("\\s+"))
                if (tokens[1] != IN) throw ConfigurationParsingException("Illegal record format: $line")
                when (tokens[2].toDnsRecordType()) {
                    DnsRecordType.SOA -> {
                        // domain IN SOA ns e-mail Serial Refresh Retry Expire DefaultTTL
                        if (configuration != null) {
                            throw ConfigurationParsingException("Multiple SOA records")
                        }
                        configuration = Configuration(
                            domain = tokens[0].validateIsDomain(),
                            rootNameServer = tokens[3].validateIsServerAddress(),
                            adminEmail = tokens[4].validateIsEmail(),
                            serial = tokens[5].validateIsNumber(),
                            refresh = tokens[6].validateNotNegative("refresh"),
                            retry = tokens[7].validateNotNegative("retry"),
                            expire = tokens[8].validateNotNegative("expire"),
                            defaultTTL = tokens[9].validateNotNegative("defaultTTL")
                        )

                    }

                    DnsRecordType.NS -> {
                        // domain IN NS address -- запись о DNS сервере
                        val domain = tokens[0].validateIsDomain()
                        val address = tokens[3].validateIsServerAddress()
                        nameServers[domain] = address
                    }

                    DnsRecordType.A -> {
                        // name IN A address -- аддресная запись
                        val domain = tokens[0].validateIsDomain()
                        val address = tokens[3].validateIsIp()
                        webAddresses[domain] = address
                    }

                    DnsRecordType.CNAME -> {
                        // сname IN CNAME name -- запись о псевдониме
                        val domain = tokens[0].validateIsDomain()
                        val targetDomain = tokens[3].validateIsDomain()
                        aliases += domain to targetDomain
                    }

                    DnsRecordType.MX -> {
                        // Mail-domain IN MX address -- запись о email сервере
                        val domain = tokens[0].validateIsDomain()
                        val address = tokens[3].validateIsIp()
                        emailAddresses[domain] = address
                    }
                }
            } catch (e: IndexOutOfBoundsException) {
                throw ConfigurationParsingException("Illegal record: $line")
            } catch (e: ValidationException) {
                throw ConfigurationParsingException(e.message)
            }
        } catch (e: ConfigurationParsingException) {
            errorMessages.add(e.message)
        }
    }
    for ((domain, targetDomain) in aliases) {
        if (targetDomain !in webAddresses) {
            errorMessages += "There is no record for $targetDomain"
            continue
        }
        webAddresses[domain] = webAddresses[targetDomain]!!
    }

    if (configuration == null) {
        errorMessages += "There is no SOA record"
    }

    val cleanedNameServers = configuration?.domain?.let { domain ->
        mutableMapOf<Domain, NameServerAddress>().apply {
            for ((name, address) in nameServers) {
                if (name.endsWith(domain) && name.length != domain.length) {
                    put(name.removeSuffix(domain), address)
                } else {
                    errorMessages += "$name is not a subdomain of $domain"
                }
            }
        }
    }

    if (errorMessages.isNotEmpty()) {
        throw ConfigurationParsingException(errorMessages)
    }

    return ServerConfiguration(
        configuration!!,
        webAddresses,
        emailAddresses,
        cleanedNameServers!!
    )
}