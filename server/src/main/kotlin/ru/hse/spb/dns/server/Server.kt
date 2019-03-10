package ru.hse.spb.dns.server

import ru.hse.spb.dns.client.communicateWithNameServer
import ru.hse.spb.dns.protocol.*
import ru.hse.spb.dns.protocol.Protocol.Request.RequestType.EMAIL
import ru.hse.spb.dns.protocol.Protocol.Request.RequestType.WEB
import java.io.File
import java.io.IOException
import java.net.ServerSocket
import java.net.Socket
import java.net.SocketTimeoutException
import java.util.concurrent.Executors

class Server(
    private val port: Int,
    private val isRecursive: Boolean,
    private val isRootServer: Boolean,
    dnsTableFile: String
) {
    private val serverConfiguration: ServerConfiguration
    private val myDomain: Domain
        get() = serverConfiguration.configuration.domain
    init {
        serverConfiguration = try {
            File(dnsTableFile).readConfiguration()
        } catch (e: IOException) {
            throw ConfigurationParsingException(e.message)
        }
    }

    fun serve() {
        val exec = Executors.newCachedThreadPool()
        ServerSocket(port).use { serverSocket ->
            while (true) {
                try {
                    val socket = serverSocket.accept()
                    exec.submit(Worker(socket))
                } catch (e: IOException) {
                    // TODO
                    e.printStackTrace()
                } catch (e: SocketTimeoutException) {
                    break
                }
            }
        }
    }

    private inner class Worker(private val socket: Socket) : Runnable {
        override fun run() {
            try {
                val request = Protocol.Request.parseDelimitedFrom(socket.getInputStream())
                val response = processRequest(request)
                response.writeDelimitedTo(socket.getOutputStream())
            } catch (e: Exception) {
                // TODO
                e.printStackTrace()
            } finally {
                socket.close()
            }
        }

        private val Domain.inOurDomain: Boolean
            get() = endsWith(myDomain)

        private val Domain.simpleName: String
            get() = removeSuffix(myDomain)

        private fun processRequest(request: Protocol.Request): Protocol.Response {
            val domain = request.address.asDomain()
                ?: return NotFoundResponse(myDomain, "${request.address} is not a valid domain")

            return tryFoundAddress(domain, request.requestType)
                ?: run {
                    val nameServerAddress = tryFoundNameServer(domain) ?: return NotFoundResponse(myDomain)
                    if (isRecursive) {
                        communicateWithNameServer(domain, request.requestType, nameServerAddress)
                    } else {
                        RedirectResponse(nameServerAddress)
                    }
                }
        }

        private fun tryFoundAddress(domain: Domain, requestType: Protocol.Request.RequestType): Protocol.Response? {
            if (!domain.inOurDomain) return null
            val addresses = when (requestType) {
                WEB -> serverConfiguration.webAddresses
                EMAIL -> serverConfiguration.emailAddresses
            }
            val address = addresses[domain.simpleName] ?: return null
            return FoundResponse(address)
        }

        private fun tryFoundNameServer(domain: Domain): NameServerAddress? {
            if (!domain.inOurDomain) return if (isRootServer) null else serverConfiguration.configuration.rootNameServer

            for ((nameServerDomain, nameServerAddress) in serverConfiguration.nameServers) {
                if (domain.simpleName.endsWith(nameServerDomain)) return nameServerAddress
            }
            return null
        }
    }

}