package ru.hse.spb.kazakov.server.http

import org.junit.Test

import org.junit.Assert.*
import java.io.ByteArrayInputStream
import java.io.DataInputStream
import java.nio.charset.Charset

class HttpRequestParserKtTest {
    @Test
    fun testNoBodyHttpRequest() {
        val queryString = "GET /hello.txt HTTP/1.1\r\n" +
                "Host: www.example.com\r\n" +
                "User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n" +
                "\r\n"
        val query = parseHttpRequest(queryString.toDataInputStream())
        val requestLine = query.requestLine

        assertEquals("GET", requestLine.method)
        assertEquals("/hello.txt", requestLine.url.path)
        assertEquals(0, requestLine.url.queryParameters.size)
        assertEquals(1, requestLine.httpVersion.majorNumber)
        assertEquals(1, requestLine.httpVersion.minorNumber)
        assertEquals("www.example.com", query.fields["Host"])
        assertEquals("curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3", query.fields["User-Agent"])
    }

    @Test
    fun testHttpRequestWithBody() {
        val body = "Message body"
        val queryString = "POST www.example.com HTTP/1.0\r\n" +
                "Content-Length: ${body.toByteArray(Charset.forName("UTF-8")).size}\r\n" +
                "\r\n" +
                body
        val query = parseHttpRequest(queryString.toDataInputStream())
        val requestLine = query.requestLine

        assertEquals("POST", requestLine.method)
        assertEquals("", requestLine.url.path)
        assertEquals(0, requestLine.url.queryParameters.size)
        assertEquals(1, requestLine.httpVersion.majorNumber)
        assertEquals(0, requestLine.httpVersion.minorNumber)
        assertEquals("${body.toByteArray(Charset.forName("UTF-8")).size}", query.fields["Content-Length"])
        assertArrayEquals(body.toByteArray(Charset.forName("UTF-8")), query.messageBody)
    }

    private fun String.toDataInputStream(): DataInputStream =
        DataInputStream(ByteArrayInputStream(this.toByteArray(Charset.forName("UTF-8"))))
}