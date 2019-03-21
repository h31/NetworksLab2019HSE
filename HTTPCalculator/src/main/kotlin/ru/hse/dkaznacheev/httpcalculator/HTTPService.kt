package ru.hse.dkaznacheev.httpcalculator

import java.io.BufferedReader
import java.io.BufferedWriter
import kotlin.collections.LinkedHashMap

object HTTPService {
    fun receiveRequest(reader: BufferedReader): HTTPRequest? {
        var line: String
        val headers = LinkedHashMap<String, String>()

        val requestLine = reader.readLine() ?: return null
        while (true) {
            line = reader.readLine()
            println(line)
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

    private fun respond(statusCode: String, body: String, writer: BufferedWriter) {
        val outWriter = System.out.bufferedWriter()
        outWriter.write("HTTP/1.1 $statusCode\n")
        outWriter.write(
            "Content-Type: text/plain; charset=utf-8\n" +
                    "Server: calculator/1.0\n" +
                    "Content-Length: ${body.length}\n")
        outWriter.write("\n")
        outWriter.write(body)
        outWriter.flush()

        writer.write("HTTP/1.1 $statusCode\n")
        writer.write(
            "Content-Type: text/plain; charset=utf-8\n" +
            "Server: calculator/1.0\n" +
            "Content-Length: ${body.length}\n")
        writer.write("\n")
        writer.write(body)
        writer.flush()
    }

    fun respondBadRequest(writer: BufferedWriter) {
        respond("400 Bad Request Error", "", writer)
    }

    fun respondBadCalculation(writer: BufferedWriter) {
        respond("409 Conflict", "Division by zero\n", writer)
    }

    fun respondSuccessfulCalculation(result: Long, writer: BufferedWriter) {
        respond("200 OK", "$result\n", writer)
    }
}