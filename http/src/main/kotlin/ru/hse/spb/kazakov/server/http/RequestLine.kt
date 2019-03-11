package ru.hse.spb.kazakov.server.http

data class RequestLine(
    val method: String,
    val httpVersion: HttpVersion,
    val url: URL
) {
    data class HttpVersion(val majorNumber: Int, val minorNumber: Int)
}

