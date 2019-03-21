package ru.hse.dkaznacheev.httpcalculator

import ru.hse.dkaznacheev.httpcalculator.Calculation.Companion.parseCalculationfromURL
import ru.hse.dkaznacheev.httpcalculator.HTTPService.receiveMessage
import ru.hse.dkaznacheev.httpcalculator.HTTPService.respondBadCalculation
import ru.hse.dkaznacheev.httpcalculator.HTTPService.respondBadRequest
import ru.hse.dkaznacheev.httpcalculator.HTTPService.respondSuccessfulCalculation
import java.io.BufferedWriter
import java.net.ServerSocket
import java.net.Socket
import java.net.SocketException

class Server(port: Int) {
    private val server = ServerSocket(port)

    private fun processCalculation(calculation: Calculation, writer: BufferedWriter) {
        val result = calculation.evaluate()
        if (result == null) {
            respondBadCalculation(writer)
        } else {
            respondSuccessfulCalculation(result, writer)
        }
    }

    private fun clientHandler(socket: Socket) {
        while (true) {
            try {
                val writer = socket.getOutputStream().bufferedWriter()
                val request = receiveMessage(socket.getInputStream().bufferedReader())

                if (request == null) {
                    respondBadRequest(writer)
                    return
                }

                val calculation = parseCalculationfromURL(request.url)

                if (calculation == null) {
                    respondBadRequest(writer)
                    return
                }

                if (!calculation.isLong()) {
                    processCalculation(calculation, writer)
                } else {
                    Thread { processCalculation(calculation, writer) }.start()
                }
            } catch (e: SocketException) {
                socket.close()
                println("Client disconnected")
                return
            }
        }
    }

    fun start() {
        while (true) {
            val socket = server.accept()
            Thread {clientHandler(socket)}.start()
        }
    }
}

fun main() {
    Server(8000).start()
}