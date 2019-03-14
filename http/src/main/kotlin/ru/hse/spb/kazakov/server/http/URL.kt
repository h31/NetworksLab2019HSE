package ru.hse.spb.kazakov.server.http

import java.net.URL

class URL(urlString: String) {
    private val url = URL(urlString)
    val path: String
        get() = url.path
    val queryParameters: Map<String, String>

    init {
        queryParameters = if (!url.query.isNullOrEmpty()) {
            url.query.split("&")
                .map(this::parseQueryParameter)
                .toMap()
        } else {
            emptyMap()
        }
    }

    private fun parseQueryParameter(parameter: String): Pair<String, String> {
        val splitIndex = parameter.indexOf("=")
        val key = if (splitIndex > 0) parameter.substring(0, splitIndex) else parameter
        val value = if (splitIndex > 0 && parameter.length > splitIndex + 1) parameter.substring(splitIndex + 1) else ""
        return Pair(key, value)
    }
}