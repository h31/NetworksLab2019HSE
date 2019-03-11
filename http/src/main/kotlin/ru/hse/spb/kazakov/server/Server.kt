package ru.hse.spb.kazakov.server

import ru.hse.spb.kazakov.server.http.*
import java.io.DataInputStream
import java.io.OutputStreamWriter
import java.lang.Exception
import java.net.ServerSocket
import java.net.Socket
import java.nio.charset.Charset

class Server(port: Int) {
    private val serverSocket = ServerSocket(port)
    private val clientsSockets = mutableListOf<Socket>()
    private val serverThreads = mutableListOf<Thread>()

    fun run() {
        val acceptClientCycle = {
            while (true) {
                val clientSocket = serverSocket.accept()
                clientsSockets.add(clientSocket)
                val clientThread = Thread(RequestResponseCycle(clientSocket))
                serverThreads.add(clientThread)
                clientThread.run()
            }
        }

        val acceptClientsThread = Thread(acceptClientCycle)
        serverThreads.add(acceptClientsThread)
        acceptClientsThread.run()
    }

    fun stop() {
        serverSocket.close()
        clientsSockets.forEach { it.close() }
        serverThreads.forEach { it.join() }
    }

    private class RequestResponseCycle(clientSocket: Socket) : Runnable {
        private val dataInputStream = DataInputStream(clientSocket.getInputStream())
        private val outputStream = OutputStreamWriter(clientSocket.getOutputStream(), Charset.forName("UTF-8"))

        override fun run() {
            while (true) {
                val request = try {
                    parseHttpRequest(dataInputStream)
                } catch (exception: MalformedHttpException) {
                    outputStream.write(HttpResponse(HttpResponseType.BAD_REQUEST).toString())
                    continue
                } catch (exception: Exception) {
                    outputStream.write(HttpResponse(HttpResponseType.SERVER_ERROR).toString())
                    continue
                }

                val response = processRequest(request)
                outputStream.write(response.toString())
            }
        }

        private fun processRequest(request: HttpRequest): HttpResponse {

        }
    }
}