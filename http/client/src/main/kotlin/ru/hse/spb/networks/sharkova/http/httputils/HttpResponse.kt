package ru.hse.spb.networks.sharkova.http.httputils

/**
 * This class represents an HTTP response.
 */
data class HttpResponse(val httpVersion: HttpVersion,
                        val statusCode: Int,
                        val reasonPhrase: String,
                        val fields: Map<String, String>,
                        val body: ByteArray) {

    /**
     * This method allows us to check the status code and process it accordingly.
     * It is more readable than simply using constants in client code.
     */
    fun getStatusCode(): StatusCode = when (statusCode) {
        200 -> StatusCode.OK
        400 -> StatusCode.BAD_REQUEST
        404 -> StatusCode.NOT_FOUND
        405 -> StatusCode.METHOD_NOT_ALLOWED
        422 -> StatusCode.UNPROCESSABLE_ENTITY
        500 -> StatusCode.SERVER_ERROR
        501 -> StatusCode.NOT_IMPLEMENTED
        else -> StatusCode.NOT_IMPLEMENTED
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as HttpResponse

        if (httpVersion != other.httpVersion) return false
        if (statusCode != other.statusCode) return false
        if (reasonPhrase != other.reasonPhrase) return false
        if (fields != other.fields) return false
        if (!body.contentEquals(other.body)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = httpVersion.hashCode()
        result = 31 * result + statusCode
        result = 31 * result + reasonPhrase.hashCode()
        result = 31 * result + fields.hashCode()
        result = 31 * result + body.contentHashCode()
        return result
    }

    data class HttpVersion(val majorNumber: Int, val minorNumber: Int)

    enum class StatusCode {
        OK,
        BAD_REQUEST,
        NOT_FOUND,
        METHOD_NOT_ALLOWED,
        UNPROCESSABLE_ENTITY,
        SERVER_ERROR,
        NOT_IMPLEMENTED
    }
}