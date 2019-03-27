package ru.hse.spb.kazakov.server.http

import com.sun.xml.internal.messaging.saaj.util.TeeInputStream
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
    val input = TeeInputStream(inputStream, output)
    val slidingWindow = ArrayDeque<Int>()

    var ch = input.read()
    while (ch != -1) {
        if (slidingWindow.size == "\r\n\r\n".length) {
            slidingWindow.removeFirst()
        }
        slidingWindow.addLast(ch)

        val stringVal = slidingWindow.toIntArray().joinToString("") { it.toChar().toString() }
        if (stringVal == "\r\n\r\n") {
            break
        }
        ch = input.read()
    }

    if (ch == -1) {
        throw MalformedURLException()
    }

    return ByteArrayInputStream(output.toByteArray())
}

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

    return RequestLine(matchResult.groupValues[1], httpVersion, URL("http://" + groups[2]))
}

private val fieldRegexp = Regex("""\s*([^\s]+)\s*:\s*(.*)\s*""")
private fun parseFields(bufferedReader: BufferedReader): Map<String, String> {
    val fields = HashMap<String, String>()

    var line = bufferedReader.readLine()
    while (line != null && !line.isEmpty()) {
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

