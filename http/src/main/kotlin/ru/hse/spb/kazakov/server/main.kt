package ru.hse.spb.kazakov.server

@ExperimentalUnsignedTypes
fun main(args : Array<String>) {
    if (args.size != 1) {
        printUsage()
        return
    }
    val port = try {
        args[0].toInt()
    } catch (exception: NumberFormatException) {
        printUsage()
        return
    }

    val server = Server(port)
    server.run()

    println("Print \"stop\" to stop the server")
    var userInput = readLine()
    while (userInput != "stop") {
        userInput = readLine()
    }
    server.stop()
}

fun printUsage() {
    println("Usage: ./server-server-1.0 <port>")
}
