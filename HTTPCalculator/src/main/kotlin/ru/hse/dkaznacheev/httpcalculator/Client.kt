package ru.hse.dkaznacheev.httpcalculator

import ru.hse.dkaznacheev.httpcalculator.Calculation.Companion.parseCalculationFromLine
import ru.hse.dkaznacheev.httpcalculator.HTTPService.receiveMessage
import ru.hse.dkaznacheev.httpcalculator.HTTPService.resultCode
import ru.hse.dkaznacheev.httpcalculator.HTTPService.sendGetRequest
import java.net.ConnectException
import java.net.Socket
import java.net.SocketException

class Client(address: String, port: Int) {
    private val socket = Socket(address, port)
    private val reader = socket.getInputStream().bufferedReader()
    private val writer = socket.getOutputStream().bufferedWriter()

    private fun receiveResults() {
        while (!socket.isClosed) {
            try {
                val response = receiveMessage(reader)
                if (response == null) {
                    println("Error receiving response")
                    continue
                }
                when (resultCode(response.requestLine)) {
                    "200" -> println(response.body)
                    "400" -> println("Incorrect request")
                    "409" -> println("Error: ${response.body}")
                }
            } catch (e: SocketException) {
                println("Connection closed")
                return
            }
        }
    }

    fun start() {
        println("Connected to server!")
        Thread {receiveResults()}.start()
        while (true) {
            val line = readLine()
            if (line == null || line == "exit") {
                socket.close()
                return
            }
            val calculation = parseCalculationFromLine(line)
            if (calculation == null) {
                println("Incorrect input")
                continue
            }
            val url = calculation.toURL()
            sendGetRequest(url, writer)
        }
    }
}

fun main(args: Array<String>) {
    if (args.size < 2) {
        println("Incorrect argument size")
        return
    }

    val address = args[0]
    val port = args[1].toInt()

    try {
        Client(address, port).start()
    } catch (e: ConnectException) {
        println("Error connecting $address/$port")
    }
}