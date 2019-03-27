package ru.hse.spb.kazakov.server.http

import java.nio.charset.Charset

class HttpResponse(
    private val responseType: HttpResponseType,
    private val contentType: ContentType = ContentType.TEXT,
    private val body: String = ""
) {
    override fun toString(): String =
            responseType.toString() + contentType + getContentLengthField() + "\r\n" + body

    private fun getContentLengthField() = "Content-Length: ${body.toByteArray(Charset.forName("UTF-8")).size}\r\n"
}

enum class HttpResponseType {
    OK {
        override fun toString(): String = "HTTP/1.1 200 OK\r\n"
    },

    BAD_REQUEST {
        override fun toString(): String = "HTTP/1.1 400 Bad Request\r\n"
    },

    NOT_FOUND {
        override fun toString(): String = "HTTP/1.1 404 Not Found\r\n"
    },

    METHOD_NOT_ALLOWED {
        override fun toString(): String = "HTTP/1.1 405 Method Not Allowed\r\n"
    },

    UNPROCESSABLE_ENTITY {
        override fun toString(): String = "HTTP/1.1 422 Unprocessable Entity\r\n"
    },

    SERVER_ERROR {
        override fun toString(): String = "HTTP/1.1 500 Internal Server Error\r\n"
    },

    NOT_IMPLEMENTED {
        override fun toString(): String = "HTTP/1.1 501 Not Implemented\r\n"
    }
}

enum class ContentType {
    TEXT {
        override fun toString(): String = "Content-Type: text/plain\r\n"
    },

    JSON {
        override fun toString(): String = "Content-Type: application/json\r\n"
    }
}