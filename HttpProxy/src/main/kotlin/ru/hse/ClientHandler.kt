package ru.hse

import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.lang.Exception
import java.net.Socket
import java.net.URL
import java.util.logging.Logger
import kotlin.concurrent.thread
import kotlin.math.log

class ClientHandler(private val cache: Cache<Request, String>, private val blackList: List<String>) {

    fun run(clientSocket: Socket): Thread {
        return thread {
            val clientAddress = clientSocket.inetAddress
            logger.info("Handling client $clientAddress")
            try {
                clientSocket.use {
                    val request = extractDetails(it)
                    if (blackList.contains(request.host)) {
                        logger.severe("Host ${request.host} is blacklisted")
                        responseWithError(it)
                        return@use
                    }
                    if (request.method == "GET") {
                        val cached = cache.lookUp(request)
                        if (cached != null) {
                            logger.info("Cached copy of response from ${request.host} was sent")
                            responseWith(cached, it)
                        } else {
                            logger.info("Fresh HTTP response from ${request.host}")
                            val response = retrieveResponse(request)
                            logger.info(response)
                            cache.addPage(request, response)
                            responseWith(response, it)
                        }

                    } else {
                        responseWith(retrieveResponse(request), it)
                    }
                }
            } catch (e: Exception) {
                logger.severe("Connection with $clientAddress interrupted: ${e.message}")
            }
        }
    }

    private fun retrieveResponse(request: Request): String {
        try {
            Socket(request.host, request.port).use {
                val writer = BufferedWriter(OutputStreamWriter(it.outputStream))
                writer.append(request.data)
                writer.flush()
                logger.info("Sent ${request.data}")

                return extractDetails(it).data
            }
        } catch (e: Exception) {
            logger.severe("Request to ${request.host} failed: ${e.message}")
            return ERROR_RESPONSE
        }
    }

    private fun responseWith(response: String, socket: Socket) {
        val writer = BufferedWriter(OutputStreamWriter(socket.outputStream))
        writer.append(response)
        writer.flush()
    }

    private fun responseWithError(socket: Socket) = responseWith(ERROR_RESPONSE, socket)

    private fun extractDetails(it: Socket): Request {
        val lines = mutableListOf<String>()
        val reader = BufferedReader(InputStreamReader(it.inputStream))
        reader.use {
            do {
                val line = it.readLine()
                lines.add(line)
            } while (lines.size < 2 || !lines.last().trim().isEmpty() || !line.trim().isEmpty())
        }

        val headerLine = lines[0].split(' ')
        val url = URL(headerLine[1])
        val protocol = url.protocol
        val port = if (url.port == -1) 80 else url.port
        val authB64 = lines.find { it.contains("Authorization") }?.split(' ')?.get(2)
        val details = Request(
            method = headerLine[0],
            host = url.host,
            port = port,
            protocol = protocol,
            url = url.toString(),
            data = lines.joinToString(separator = "\r\n"),
            auth = authB64
        )
        return details
    }

    companion object {
        const val ERROR_RESPONSE = "HTTP/1.0 200 OK\r\n" +
                "Content-Length: 11\r\n" +
                "\r\n" +
                "Error\r\n" +
                "\r\n" +
                "\r\n"

        val logger = Logger.getLogger(ClientHandler::class.java.name)
    }
}
