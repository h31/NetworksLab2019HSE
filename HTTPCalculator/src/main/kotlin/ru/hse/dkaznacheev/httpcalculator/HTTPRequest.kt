package ru.hse.dkaznacheev.httpcalculator

import java.net.URLDecoder
import java.util.LinkedHashMap

data class HTTPRequest(val requestLine: String, val headers: LinkedHashMap<String, String>, val body: String) {
    val requestType: String
    val url: String

    init {
        val tokens = requestLine.split(" ")
        requestType = tokens[0]
        url = URLDecoder.decode(tokens[1], getCharset() ?: "utf-8")
    }

    override fun toString(): String {
        val headersString = headers.toList().joinToString("\n") { "${it.first}: ${it.second}" }
        return requestLine + "\n" + headersString + "\n" + body
    }

    private fun getCharset(): String? {
        val contentType = headers["Content-type"] ?: return null
        val tokens = contentType.split("; ")

        return tokens
            .find { it.startsWith("charset=") }
            ?.substring("charset=".length)
    }
}