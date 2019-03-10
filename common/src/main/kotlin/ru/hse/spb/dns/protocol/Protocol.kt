package ru.hse.spb.dns.protocol

import com.google.protobuf.InvalidProtocolBufferException
import java.net.InetSocketAddress

typealias Email = String
typealias Ip = String
typealias NameServerAddress = InetSocketAddress
typealias Domain = String

enum class ResponseType {
    Found, Redirect, NotFound
}

val Protocol.Response.responseType: ResponseType
    get() = when {
        hasFound() -> ResponseType.Found
        hasRedirect() -> ResponseType.Redirect
        hasNotFound() -> ResponseType.NotFound
        else -> throw InvalidProtocolBufferException("Bad message")
    }

private fun buildResponse(init: Protocol.Response.Builder.() -> Unit): Protocol.Response = Protocol.Response.newBuilder().apply(init).build()

@Suppress("FunctionName")
fun Request(domain: Domain, requestType: Protocol.Request.RequestType): Protocol.Request = Protocol.Request.newBuilder().apply {
    this.address = domain.validateIsDomain()
    this.requestType = requestType
}.build()

@Suppress("FunctionName")
fun FoundResponse(ip: Ip) = buildResponse {
    found = Protocol.Response.FoundResponse.newBuilder().apply {
        this.ip = ip
    }.build()
}

@Suppress("FunctionName")
fun RedirectResponse(address: InetSocketAddress) = buildResponse {
    redirect = Protocol.Response.RedirectResponse.newBuilder().apply {
        ip = address.hostName
        port = address.port
    }.build()
}

@Suppress("FunctionName")
fun NotFoundResponse(author: Domain, message: String? = null) = buildResponse {
    notFound = Protocol.Response.NotFoundResponse.newBuilder().apply {
        this.author = author
        if (message != null) this.message = message
    }.build()
}