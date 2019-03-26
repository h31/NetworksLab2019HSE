package ru.hse

import java.io.BufferedReader
import java.io.InputStreamReader
import java.lang.Exception
import java.net.Socket
import java.util.logging.Logger
import kotlin.concurrent.thread

class ClientHandler(cache: Cache, blackList: List<String>) {

    fun run(clientSocket: Socket): Thread {
        return thread {
            val clientAddress = clientSocket.inetAddress
            logger.info("Handling client $clientAddress")
            try {
                clientSocket.use {
                    val reader = BufferedReader(InputStreamReader(it.inputStream))
                    var header = reader.readLine()
                    logger.info("Client data: $header")
                    while (true) {
                        header = reader.readLine() ?: break
                        logger.info(header)
                    }
                    reader.close()
                }
            } catch (e: Exception) {
                logger.severe("Connection with $clientAddress interrupted: ${e.message}")
            }
        }
    }

    companion object {
        val logger = Logger.getLogger(ClientHandler::class.java.name)
    }
}
