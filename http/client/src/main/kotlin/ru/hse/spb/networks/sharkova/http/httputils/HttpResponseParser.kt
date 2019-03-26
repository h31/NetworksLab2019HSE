package ru.hse.spb.networks.sharkova.http.httputils

import java.io.*
import java.util.*

/**
 * This class parses an HTTP response from an input stream.
 */
class HttpResponseParser {
    companion object {
        private val statusLineRegex = Regex("""HTTP/(\d+)\.(\d+)\s+(\d+)\s+([^\s]+)""")
        private val fieldRegex = Regex("""\s*([^\s]+)\s*:\s*(.*)\s*""")

        /**
         * Reads an HTTP response from input stream.
         * @param inputStream stream from which HTTP response is read
         * @return parsed HTTP response
         */
        fun parseInput(inputStream: DataInputStream): HttpResponse {
            val bufferedReader = BufferedReader(InputStreamReader(getHeaderStream(inputStream)))
            var line = bufferedReader.readLine()

            while (line != null && line.isEmpty()) {
                line = readLine()
            }
            if (line == null) {
                throw MalformedHttpException()
            }

            val matchResult = statusLineRegex.matchEntire(line) ?: throw MalformedHttpException()
            val groups = matchResult.groupValues
            val httpVersion = HttpResponse.HttpVersion(groups[1].toInt(), groups[2].toInt())
            val statusCode = groups[3].toIntOrNull() ?: throw MalformedHttpException()
            val reasonPhrase = groups[4]

            val fields = parseFields(bufferedReader)
            val body = parseBody(inputStream, fields["Content-Length"])

            return HttpResponse(httpVersion, statusCode, reasonPhrase, fields, body)
        }

        private fun parseFields(bufferedReader: BufferedReader): Map<String, String> {
            val fields = HashMap<String, String>()

            var line = bufferedReader.readLine()
            while (line != null && !line.isEmpty()) {
                val matchResult = fieldRegex.matchEntire(line) ?: throw MalformedHttpException()
                val groups = matchResult.groupValues
                fields[groups[1]] = groups[2]
                line = bufferedReader.readLine()
            }

            return fields
        }

        private fun parseBody(input: DataInputStream, bodySize: String?): ByteArray =
            if (bodySize == null) {
                byteArrayOf()
            } else {
                val size = bodySize.toIntOrNull() ?: throw MalformedHttpException()
                val result = ByteArray(size)
                input.readFully(result)
                result
            }

        private fun getHeaderStream(inputStream: DataInputStream): InputStream {
            val output = ByteArrayOutputStream()
            val slidingWindow = ArrayDeque<Int>()

            var ch = inputStream.read()
            while (ch != -1) {
                if (slidingWindow.size == 4) {
                    slidingWindow.removeFirst()
                }
                slidingWindow.addLast(ch)
                output.write(ch)

                val buffer = slidingWindow.toIntArray()
                if (isContainsEmptyLine(buffer)) {
                    break
                }
                ch = inputStream.read()
            }

            if (ch == -1) {
                throw MalformedHttpException()
            }

            return ByteArrayInputStream(output.toByteArray())
        }

        private fun isContainsEmptyLine(array: IntArray): Boolean =
            array.size == 4 && array[0] == '\r'.toInt() && array[1] == '\n'.toInt() && array[2] == '\r'.toInt() && array[3] == '\n'.toInt()
    }

}