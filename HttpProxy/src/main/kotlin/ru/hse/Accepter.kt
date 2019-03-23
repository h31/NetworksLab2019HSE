package ru.hse

import java.net.ServerSocket
import kotlin.concurrent.thread

class Acceptor(cache: Cache, port: Int) {

    private val listener: Thread
    private val workers: MutableList<Thread> = ArrayList()

    init {
        val serverSocket = ServerSocket(port)
        listener = thread(start = false, name = "Acceptor") {
            while (!Thread.currentThread().isInterrupted) {
                serverSocket.use {
                    val socket = serverSocket.accept()
                    workers.add(startClientHandler(socket, cache))
                }
            }
        }
    }

    fun start() {
        listener.start()
    }

    fun stop() {
        listener.interrupt()
        workers.forEach(Thread::interrupt)
    }
}
