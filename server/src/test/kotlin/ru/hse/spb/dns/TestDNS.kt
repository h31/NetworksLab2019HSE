package ru.hse.spb.dns

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Test
import ru.hse.spb.dns.client.getAnswerFromServer
import ru.hse.spb.dns.protocol.Protocol.Request.RequestType.EMAIL
import ru.hse.spb.dns.protocol.Protocol.Request.RequestType.WEB
import ru.hse.spb.dns.protocol.ResponseType.Found
import ru.hse.spb.dns.protocol.ResponseType.NotFound
import ru.hse.spb.dns.protocol.responseType
import ru.hse.spb.dns.protocol.validateIsServerAddress

class TestDNS {
    companion object {
        private val RU_SERVER = "localhost:1113".validateIsServerAddress()
        private val COM_SERVER = "localhost:1115".validateIsServerAddress()
    }

    @Test
    fun testCorrectAddressInOuterNetwork() {
        val response = getAnswerFromServer("spb.hse.com", WEB, RU_SERVER)
        assertEquals(Found, response.responseType)
        assertEquals("192.168.0.3", response.found.ip)
    }

    @Test
    fun testCorrectAddressInLocalNetwork() {
        val response = getAnswerFromServer("spb.hse.ru", WEB, RU_SERVER)
        assertEquals(Found, response.responseType)
        assertEquals("192.168.0.2", response.found.ip)
    }

    @Test
    fun testCorrectAddressInCloseNetwork() {
        val response = getAnswerFromServer("info.ru", WEB, RU_SERVER)
        assertEquals(Found, response.responseType)
        assertEquals("192.168.0.1", response.found.ip)
    }

    @Test
    fun testAddressAlias() {
        val response = getAnswerFromServer("www.spb.hse.ru", WEB, COM_SERVER)
        assertEquals(Found, response.responseType)
        assertEquals("192.168.0.2", response.found.ip)
    }

    @Test
    fun testNotFoundLocal() {
        val response = getAnswerFromServer("aaa.spb.hse.ru", EMAIL, RU_SERVER)
        assertEquals(NotFound, response.responseType)
        assertEquals("hse.ru.", response.notFound.author)
        assertFalse(response.notFound.hasMessage())
    }

    @Test
    fun testNotFoundGlobal() {
        val response = getAnswerFromServer("aaa.spb.hse.ru", EMAIL, COM_SERVER)
        assertEquals(NotFound, response.responseType)
        assertEquals("hse.ru.", response.notFound.author)
        assertFalse(response.notFound.hasMessage())
    }

    @Test
    fun testEmail() {
        val response = getAnswerFromServer("mail.hse.com", EMAIL, RU_SERVER)
        assertEquals(Found, response.responseType)
        assertEquals("192.168.1.1", response.found.ip)
    }

    @Test
    fun testNotEmail() {
        val response = getAnswerFromServer("mail.hse.com", WEB, RU_SERVER)
        assertEquals(NotFound, response.responseType)
        assertFalse(response.notFound.hasMessage())
    }
}