package ru.hse.spb.kazakov.server.http

import java.io.*
import java.net.MalformedURLException
import java.util.*


fun parseHttpRequest(inputStream: DataInputStream): HttpRequest {
    val bufferedReader = BufferedReader(InputStreamReader(getHeaderStream(inputStream)))

    val requestLine = try {
        parseRequestLine(bufferedReader)
    } catch (exception: MalformedURLException) {
        throw MalformedHttpException()
    }
    val fields = parseFields(bufferedReader)
    val body = parseBody(inputStream, fields["Content-Length"])

    return HttpRequest(requestLine, body, fields)
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
        throw MalformedURLException()
    }

    return ByteArrayInputStream(output.toByteArray())
}

private fun isContainsEmptyLine(array: IntArray): Boolean =
    array.size == 4 && array[0] == '\r'.toInt() && array[1] == '\n'.toInt() && array[2] == '\r'.toInt() && array[3] == '\n'.toInt()

private val requestLineRegexp = Regex("""([^\s]+)\s+([^\s]+)\s+HTTP/(\d+)\.(\d+)""")
private fun parseRequestLine(inputStream: BufferedReader): RequestLine {
    var line = inputStream.readLine()
    while (line != null && line.isEmpty()) {
        line = inputStream.readLine()
    }
    if (line == null) {
        throw MalformedHttpException()
    }

    val matchResult = requestLineRegexp.matchEntire(line) ?: throw MalformedHttpException()
    val groups = matchResult.groupValues
    val httpVersion = RequestLine.HttpVersion(groups[3].toInt(), groups[4].toInt())

    return RequestLine(matchResult.groupValues[1], httpVersion, URL(groups[2]))
}

private val fieldRegexp = Regex("""\s+([^\s]+)\s+:\s+(.*)\s+""")
private fun parseFields(bufferedReader: BufferedReader): Map<String, String> {
    val fields = HashMap<String, String>()

    var line = bufferedReader.readLine()
    while (line != null && line.isEmpty()) {
        val matchResult = fieldRegexp.matchEntire(line) ?: throw MalformedHttpException()
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
        val size = try {
            bodySize.toInt()
        } catch (exception: NumberFormatException) {
            throw MalformedHttpException()
        }
        val result = ByteArray(size)
        input.readFully(result)
        result
    }

