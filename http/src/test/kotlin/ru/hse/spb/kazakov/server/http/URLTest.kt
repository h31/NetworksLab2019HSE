package ru.hse.spb.kazakov.server.http

import org.junit.Assert.*
import org.junit.Test

class URLTest {
    @Test
    fun testPath() {
        val url = URL("http://example.com/ex")
        assertEquals("/ex", url.path)
    }

    @Test
    fun testParametersWithValue() {
        val url = URL("http://example.com/doc?param=12&q=fd")
        assertEquals(2, url.queryParameters.size)
        assertEquals("12", url.queryParameters["param"])
        assertEquals("fd", url.queryParameters["q"])
    }

    @Test
    fun testParametersWithNoValue() {
        val url = URL("http://example.com/doc?param=&q")
        assertEquals(2, url.queryParameters.size)
        assertEquals("", url.queryParameters["param"])
        assertEquals("", url.queryParameters["q"])
    }
}