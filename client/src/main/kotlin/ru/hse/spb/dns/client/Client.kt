package ru.hse.spb.dns.client

import ru.hse.spb.dns.protocol.*
import java.net.InetSocketAddress
import java.net.Socket

class AddressNotFoundException(override val message: String? = null) : Exception(message)

fun getAddressOfDomain(domain: Domain, requestType: Protocol.Request.RequestType, address: InetSocketAddress): Ip {
    val response = getAnswerFromServer(domain, requestType, address)
    return when (response.responseType) {
        ResponseType.Found -> response.found.ip
        ResponseType.Redirect -> throw IllegalStateException()
        ResponseType.NotFound -> {
            val notFound = response.notFound
            val message = if (notFound.hasMessage()) notFound.message else null
            throw AddressNotFoundException(message)
        }
    }
}

fun getAnswerFromServer(
    domain: Domain,
    requestType: Protocol.Request.RequestType,
    address: InetSocketAddress
): Protocol.Response {
    var response: Protocol.Response
    var currentAddress = address
    do {
        response = communicateWithNameServer(domain, requestType, currentAddress)
        if (response.responseType == ResponseType.Redirect) {
            currentAddress = with(response.redirect) { InetSocketAddress(ip, port) }
        }
    } while (response.responseType == ResponseType.Redirect)
    return response
}

fun communicateWithNameServer(
    domain: Domain,
    requestType: Protocol.Request.RequestType,
    address: InetSocketAddress
): Protocol.Response =
    Socket().use { socket ->
        socket.connect(address)
        val request = Request(domain, requestType)
        request.writeDelimitedTo(socket.getOutputStream())
        Protocol.Response.parseDelimitedFrom(socket.getInputStream())
    }