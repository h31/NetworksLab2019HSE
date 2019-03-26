package ru.hse.spb.networks.sharkova.http

import ru.hse.spb.networks.sharkova.http.httputils.MalformedHttpException
import java.io.IOException
import java.lang.IllegalArgumentException

fun main(args: Array<String>) {
    if (args.size != 2 || args[1].toIntOrNull() == null) {
        println("Incorrect arguments. Required arguments: [host name] [port number]")
        return
    }

    try {
        val client = Client(args[0], args[1].toInt())
        client.run()
    } catch (e: IOException) {
        outputErrorMessage()
        println("Server might currently be unavailable.")
    } catch (e: IllegalArgumentException) {
        outputErrorMessage()
    } catch (e: MalformedHttpException) {
        println("Server was unable to response. ")
    }
}

private fun outputErrorMessage() = println("Unable to connect to the server. Make sure the arguments are correct.")
