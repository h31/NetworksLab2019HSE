package ru.hse.spb.networks.sharkova.http

import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import com.google.gson.stream.JsonReader
import ru.hse.spb.networks.sharkova.http.httputils.HttpResponseParser
import ru.hse.spb.networks.sharkova.http.httputils.HttpRequest
import ru.hse.spb.networks.sharkova.http.httputils.HttpResponse
import java.io.*
import java.net.Socket
import java.nio.charset.Charset
import java.util.ArrayList


/**
 * Basic HTTP client implementation.
 * Supported commands: add product, list products, buy product, exit, output help.
 */
class Client(private val host: String, port: Int) {
    private val serverSocket = Socket(host, port)
    private val serverOutput = DataInputStream(serverSocket.getInputStream())
    private val outputStreamWriter = OutputStreamWriter(serverSocket.getOutputStream(),
        Charset.forName("UTF-8"))

    companion object {
        private const val ADD_PRODUCT = 1
        private const val LIST_PRODUCTS = 2
        private const val BUY_PRODUCT = 3
        private const val EXIT = 4
        private const val HELP = 5
    }

    /**
     * Runs the client request-response cycle, reading the user input and handling it accordingly
     * (sending HTTP request, receiving HTTP response).
     */
    fun run() {
        outputUsage()
        while (true) {
            val input = readLine()
            when (input?.toIntOrNull()) {
                ADD_PRODUCT -> {
                    println("Input product name, price, and amount, each on a new line")
                    val name = readLine()
                    val price = readLine()?.toIntOrNull()
                    val amount = readLine()?.toIntOrNull()
                    if (name == null) {
                        outputIncorrectArgumentMessage("name")
                    } else if (price == null || price < 0) {
                        outputIncorrectArgumentMessage("price")
                    } else if (amount == null || amount < 0) {
                        outputIncorrectArgumentMessage("amount")
                    } else {
                        addProduct(name, price.toString(), amount.toString())
                    }
                }
                LIST_PRODUCTS -> {
                    listProducts()
                }
                BUY_PRODUCT -> {
                    println("Input product id. " +
                            "If you don't know the id, input -1 to return to the main menu " +
                            "and input 2 to list all products")
                    val id = readLine()?.toIntOrNull() ?: outputIncorrectArgumentMessage("id")
                    if (id != -1) {
                        buyProduct(id.toString())
                    }
                }
                EXIT -> {
                    stop()
                    return
                }
                HELP -> {
                    outputUsage()
                }
                else -> {
                    println("Incorrect command number.")
                    outputUsage()
                }
            }
            println()
        }
    }

    private fun addProduct(name: String, price: String, amount: String) {
        val request = HttpRequest("POST", "add",
            mapOf("name" to name, "price" to price, "amount" to amount), mapOf("Host" to host))
        outputStreamWriter.write(request.toString())
        outputStreamWriter.flush()
        val response = HttpResponseParser.parseInput(serverOutput)
        if (response.getStatusCode() != HttpResponse.StatusCode.OK) {
            println("Could not add product.")
        }

    }

    private fun listProducts() {
        val request = HttpRequest("GET", "list", fields = mapOf("Host" to host))
        outputStreamWriter.write(request.toString())
        outputStreamWriter.flush()
        val response = HttpResponseParser.parseInput(serverOutput)
        if (response.getStatusCode() == HttpResponse.StatusCode.OK) {
            val products = jsonToProducts(response.body)
            for (product in products) {
                println("Id: ${product.id}; name: ${product.name.replace("%20", " ")}; " +
                        "price: ${product.price}; amount: ${product.amount}")
            }
        } else {
            println("Could not retrieve product list. ${response.statusCode} ${response.reasonPhrase}")
        }
    }

    private fun buyProduct(id: String) {
        val request = HttpRequest("POST", "buy", mapOf("id" to id), mapOf("Host" to host))
        outputStreamWriter.write(request.toString())
        outputStreamWriter.flush()
        val response = HttpResponseParser.parseInput(serverOutput)
        if (response.getStatusCode() == HttpResponse.StatusCode.OK) {
            println(String(response.body))
        } else {
            println("Could not buy product. ${response.statusCode} ${response.reasonPhrase}")
        }
    }

    private fun stop() {
        serverSocket.close()
    }

    private fun outputUsage() {
        println("Input a single number corresponding to the following commands:")
        println("1: Add product")
        println("2: List products")
        println("3: Buy product")
        println("4: Exit program")
        println("5: Output this help message")
    }

    private fun outputIncorrectArgumentMessage(argumentName: String) {
        println("Incorrect argument: $argumentName")
    }

    private fun jsonToProducts(byteArray: ByteArray): List<Product> {
        val reader = JsonReader(StringReader(String(byteArray)))
        val listType = object : TypeToken<ArrayList<Product>>(){}.type
        return Gson().fromJson(reader, listType)
    }


    private data class Product(val id: Int, val name: String, val price: Int, var amount: Int)
}