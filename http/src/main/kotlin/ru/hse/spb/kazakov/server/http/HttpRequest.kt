package ru.hse.spb.kazakov.server.http

data class HttpRequest(
    val requestLine: RequestLine,
    val messageBody: ByteArray,
    val fields: Map<String, String>
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as HttpRequest

        if (requestLine != other.requestLine) return false
        if (!messageBody.contentEquals(other.messageBody)) return false
        if (fields != other.fields) return false

        return true
    }

    override fun hashCode(): Int {
        var result = requestLine.hashCode()
        result = 31 * result + messageBody.contentHashCode()
        result = 31 * result + fields.hashCode()
        return result
    }
}