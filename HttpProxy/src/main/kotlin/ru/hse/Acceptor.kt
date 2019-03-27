package ru.hse

import java.net.ServerSocket
import java.util.logging.Logger
import kotlin.concurrent.thread

class Acceptor(val port: Int, clientHandler: ClientHandler) {

    private val listener: Thread
    private val workers: MutableList<Thread> = ArrayList()

    init {
        listener = thread(start = false, name = "Acceptor") {
            try {
                val serverSocket = ServerSocket(port)
                serverSocket.use {
                while (!Thread.currentThread().isInterrupted) {
                        val socket = it.accept()
                        workers.add(clientHandler.run(socket))
                    }
                }
            } catch (e: Exception) {
                logger.severe("Closing client acceptor: ${e.message}")
                e.printStackTrace()
            }
        }
    }

    fun start() {
        listener.start()
        logger.info("Proxy started on port $port")
    }

    fun stop() {
        listener.interrupt()
        workers.forEach(Thread::interrupt)
    }

    companion object {
        val logger: Logger = Logger.getLogger(Acceptor::class.java.name)
    }
}
