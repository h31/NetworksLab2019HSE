package ru.hse.dkaznacheev.httpcalculator

import java.net.ServerSocket

class Server(port: Int) {
    private val server = ServerSocket(port)

    fun start() {
        val socket = server.accept()

        println(HTTPService.receiveRequest(socket.getInputStream().bufferedReader()))

        val writer = socket.getOutputStream().bufferedWriter()
        writer.write("200 OK\nHI THERE\n")
        writer.flush()
        socket.close()
    }
}

fun main() {
    Server(8000).start()
}