package ru.hse.spb.dns

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Test
import ru.hse.spb.dns.server.readConfiguration
import java.io.File

class ConfigurationTest {
    private fun testConfiguration(file: String, expected: String) {
        val configuration = File(ClassLoader.getSystemResource(file).toURI()).readConfiguration()
        assertEquals(expected, configuration.toString())
    }

    @Test
    fun testRootConfiguration() {
        testConfiguration("root.dns", "ServerConfiguration(configuration=Configuration(domain=., rootNameServer=localhost/127.0.0.1:0, adminEmail=admin@admin.com, serial=1, refresh=1, retry=10, expire=10, defaultTTL=10), webAddresses={}, emailAddresses={}, nameServers={ru=localhost/127.0.0.1:1112, com=localhost/127.0.0.1:1114})")
    }

    @Test
    fun testRuConfiguration() {
        testConfiguration("ru.dns", "ServerConfiguration(configuration=Configuration(domain=ru., rootNameServer=localhost/127.0.0.1:1111, adminEmail=admin@admin.com, serial=1, refresh=1, retry=10, expire=10, defaultTTL=10), webAddresses={info.=192.168.0.1}, emailAddresses={}, nameServers={hse.=localhost/127.0.0.1:1113})")
    }

    @Test
    fun testComConfiguration() {
        testConfiguration("com.dns", "ServerConfiguration(configuration=Configuration(domain=com., rootNameServer=localhost/127.0.0.1:1111, adminEmail=admin@admin.com, serial=1, refresh=1, retry=10, expire=10, defaultTTL=10), webAddresses={}, emailAddresses={}, nameServers={hse.=localhost/127.0.0.1:1115})")
    }

    @Test
    fun testHseRuConfiguration() {
        testConfiguration("hse.ru.dns", "ServerConfiguration(configuration=Configuration(domain=hse.ru., rootNameServer=localhost/127.0.0.1:1112, adminEmail=admin@admin.com, serial=1, refresh=1, retry=10, expire=10, defaultTTL=10), webAddresses={spb.=192.168.0.2, www.spb.=192.168.0.2}, emailAddresses={}, nameServers={})")
    }

    @Test
    fun testHseComConfiguration() {
        testConfiguration("hse.com.dns", "ServerConfiguration(configuration=Configuration(domain=hse.com., rootNameServer=localhost/127.0.0.1:1114, adminEmail=admin@admin.com, serial=1, refresh=1, retry=10, expire=10, defaultTTL=10), webAddresses={spb.=192.168.0.3}, emailAddresses={}, nameServers={})")
    }
}