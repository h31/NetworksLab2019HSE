package ru.hse.dkaznacheev.httpcalculator

import java.util.LinkedHashMap

data class HTTPRequest(val requestLine: String, val headers: LinkedHashMap<String, String>, val body: String) {
    override fun toString(): String {
        val headersString = headers.toList().joinToString("\n") { "${it.first}:{${it.second}" }
        return requestLine + "\n" + headersString + "\n" + body
    }
}