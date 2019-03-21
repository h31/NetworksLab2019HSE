package ru.hse

import java.net.ServerSocket
import kotlin.concurrent.thread

class Acceptor(cache: Cache, port: Int) {

    lateinit var thread: Thread

    init {
        val serverSocket = ServerSocket(port)
        serverSocket.use {
            thread = thread(start = false, name = "Acceptor") {
                while (!Thread.currentThread().isInterrupted) {
                    val socket = serverSocket.accept()
                    socket.use {
                        socket.getOutputStream()
                    }
                }
            }
        }
    }

    fun start() {
        thread.start()
    }

    fun stop() {
        thread.interrupt()
    }
}
