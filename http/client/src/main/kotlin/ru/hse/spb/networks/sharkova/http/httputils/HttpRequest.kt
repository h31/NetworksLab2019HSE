package ru.hse.spb.networks.sharkova.http.httputils

/**
 * This class represents an HTTP request.
 */
data class HttpRequest(private val method: String,
                       private val urlStart: String,
                       private val queryParameters: Map<String, String> = emptyMap(),
                       private val fields: Map<String, String> = emptyMap(),
                       private val body: String = "") {
    private val completeUrlString: String
    init {
        val urlBuilder = StringBuilder("/$urlStart")
        if (queryParameters.isNotEmpty()) {
            urlBuilder.append("?")
            for (parameter in queryParameters) {
                urlBuilder.append(parameter.key)
                urlBuilder.append("=")
                urlBuilder.append(parameter.value.replace(Regex("[\\s]"), "%20"))
                urlBuilder.append("&")
            }
            urlBuilder.setLength(urlBuilder.length - 1)
        }
        completeUrlString = urlBuilder.toString()
    }

    override fun toString(): String {
        val requestBuilder = StringBuilder("$method $completeUrlString HTTP/1.1\r\n")
        for (field in fields) {
            requestBuilder.append("${field.key}: ${field.value}\r\n")
        }
        requestBuilder.append("\r\n")
        requestBuilder.append(body)
        return requestBuilder.toString()
    }


}