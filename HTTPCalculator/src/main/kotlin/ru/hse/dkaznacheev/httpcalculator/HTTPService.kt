package ru.hse.dkaznacheev.httpcalculator

import java.io.BufferedReader
import kotlin.collections.LinkedHashMap

object HTTPService {
    fun receiveRequest(reader: BufferedReader): HTTPRequest? {
        var line: String
        val headers = LinkedHashMap<String, String>()

        val requestLine = reader.readLine() ?: return null
        while (true) {
            line = reader.readLine()
            if (line == "")
                break
            val parts = line.split(": ")
            if (parts.size < 2)
                return null
            headers[parts[0]] = parts.drop(1).joinToString()
        }
        if ("Content-Length" in headers) {
            val length = headers["Content-Length"]!!.toInt()
            val buffer = CharArray(length)
            reader.read(buffer, 0, length)
            return HTTPRequest(requestLine, headers, buffer.contentToString())
        }
        return HTTPRequest(requestLine, headers, "")
    }
}