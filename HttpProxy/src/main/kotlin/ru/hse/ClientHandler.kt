package ru.hse

import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.net.Socket
import java.net.URL
import java.util.logging.Logger
import kotlin.concurrent.thread

class ClientHandler(private val cache: Cache<String, String>, private val blackList: List<String>) {

    fun run(clientSocket: Socket): Thread {
        return thread {
            val clientAddress = clientSocket.inetAddress
            logger.info("Handling client $clientAddress")
            try {
                clientSocket.use {
                    val request = extractRequestDetails(it)
                    if (blackList.contains(request.host)) {
                        logger.severe("Host ${request.host} is blacklisted")
                        responseWithError(it)
                        return@use
                    }
                    if (request.method == "GET") {
                        val cached = cache.lookUp(request.host)
                        val canCache = Cache.canBeCached(request.data.split("\n"))
                        if (canCache && cached != null) {
                            logger.info("Cached copy of response from ${request.host} was sent")
                            responseWith(cached, it)
                        } else {
                            logger.info("Fresh HTTP response from ${request.host}")
                            val response = retrieveResponse(request)
                            logger.info(response)
                            if (canCache) {
                                cache.addPage(request.host, response)
                            }
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

                val response = extractResponse(it)
                logger.info("Received $response")
                return response
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

    private fun extractRequestDetails(socket: Socket): Request {
        val lines = readLines(socket)

        val headerLine = lines[0].split(' ')
        val url = URL(headerLine[1])
        val protocol = url.protocol
        val port = if (url.port == -1) 80 else url.port
        val authB64 = lines.find { it.contains("Authorization") }?.split(' ')?.get(2)
        return Request(
            method = headerLine[0],
            host = url.host,
            port = port,
            protocol = protocol,
            url = url.toString(),
            data = joinLines(lines),
            auth = authB64
        )
    }

    private fun extractResponse(socket: Socket): String {
        val lines = readLines(socket, 2)
        return joinLines(lines)
    }

    private fun readLines(socket: Socket, times: Int = 1): List<String> {
        val lines = mutableListOf<String>()
        val reader = BufferedReader(InputStreamReader(socket.inputStream))
        repeat(times) {
            do {
                val line: String = reader.readLine() ?: return@repeat
                lines.add(line)
                logger.info(line)
            } while (!line.isEmpty())
        }
        return lines
    }

    private fun joinLines(lines: List<String>): String {
        return lines.joinToString(separator = SEPARATOR, postfix = SEPARATOR + SEPARATOR)
    }

    companion object {
        const val ERROR_RESPONSE = "HTTP/1.0 200 OK\r\n" +
                "Content-Length: 11\r\n" +
                "\r\n" +
                "Error\r\n" +
                "\r\n"
        const val SEPARATOR = "\r\n"

        val logger = Logger.getLogger(ClientHandler::class.java.name)!!
    }
}
