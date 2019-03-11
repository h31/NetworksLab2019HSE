package ru.hse.spb.kazakov.http

import java.net.ServerSocket
import java.net.Socket

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

    private class RequestResponseCycle(private val clientSocket: Socket) : Runnable {
        override fun run() {
            while (true) {

            }
        }
    }
}