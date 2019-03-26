package ru.hse.spb.kazakov.server

import com.google.gson.Gson
import ru.hse.spb.kazakov.server.http.*
import java.io.DataInputStream
import java.io.OutputStreamWriter
import java.lang.Exception
import java.net.ServerSocket
import java.net.Socket
import java.net.SocketException
import java.nio.charset.Charset
import java.util.concurrent.locks.ReentrantReadWriteLock

@ExperimentalUnsignedTypes
class Server(port: Int) {
    private val serverSocket = ServerSocket(port)
    private val clientsSockets = mutableListOf<Socket>()
    private val serverThreads = mutableListOf<Thread>()
    private val products = mutableListOf<Product>()
    private val productsAccess = ReentrantReadWriteLock()

    fun run() {
        val acceptClientCycle = {
            try {
                while (true) {
                    val clientSocket = serverSocket.accept()
                    clientsSockets.add(clientSocket)
                    val clientThread = Thread(RequestResponseCycle(clientSocket))
                    serverThreads.add(clientThread)
                    clientThread.start()
                }
            } catch (exception: SocketException) {}
        }

        val acceptClientsThread = Thread(acceptClientCycle)
        serverThreads.add(acceptClientsThread)
        acceptClientsThread.start()
    }

    fun stop() {
        serverSocket.close()
        clientsSockets.forEach { it.close() }
        serverThreads.forEach { it.join() }
    }

    private fun productsToJson(): String {
        productsAccess.readLock().lock()
        val productsJson = Gson().toJson(products)
        productsAccess.readLock().unlock()
        return productsJson
    }

    private fun addProduct(name: String, price: UInt, amount: UInt) {
        productsAccess.writeLock().lock()
        val product = Product(products.size, name, price, amount)
        products.add(product)
        productsAccess.writeLock().unlock()
    }

    private data class Product(val id: Int, val name: String, val price: UInt, var amount: UInt)

    private inner class RequestResponseCycle(clientSocket: Socket) : Runnable {
        private val dataInputStream = DataInputStream(clientSocket.getInputStream())
        private val outputStream = OutputStreamWriter(clientSocket.getOutputStream(), Charset.forName("UTF-8"))

        override fun run() {
            while (true) {
                val request = try {
                    parseHttpRequest(dataInputStream)
                } catch (exception: MalformedHttpException) {
                    outputStream.write(HttpResponse(HttpResponseType.BAD_REQUEST).toString())
                    continue
                } catch (exception: Exception) {
                    outputStream.write(HttpResponse(HttpResponseType.SERVER_ERROR).toString())
                    break
                }

                val response = processRequest(request)
                outputStream.write(response.toString())
                outputStream.flush()
            }
        }

        private fun processRequest(request: HttpRequest): HttpResponse =
            when (request.requestLine.method) {
                "GET" -> {
                    if (request.requestLine.url.path != "/list") {
                        HttpResponse(HttpResponseType.NOT_FOUND)
                    } else {
                        HttpResponse(HttpResponseType.OK, ContentType.JSON, productsToJson())
                    }
                }

                "POST" -> {
                    when (request.requestLine.url.path) {
                        "/add" -> {
                            val queryParameters = request.requestLine.url.queryParameters
                            val name = queryParameters["name"]
                            val price = queryParameters["price"]?.toUInt()
                            val amount = queryParameters["amount"]?.toUInt()
                            if (name == null || price == null || amount == null || price == 0U || amount == 0U) {
                                HttpResponse(HttpResponseType.UNPROCESSABLE_ENTITY)
                            } else {
                                addProduct(name, price, amount)
                                HttpResponse(HttpResponseType.OK)
                            }
                        }

                        "/buy" -> {
                            val id = request.requestLine.url.queryParameters["id"]?.toInt()
                            productsAccess.writeLock().lock()
                            if (id == null) {
                                productsAccess.writeLock().unlock()
                                HttpResponse(HttpResponseType.UNPROCESSABLE_ENTITY)
                            } else if (id < 0 || id >= products.size) {
                                productsAccess.writeLock().unlock()
                                HttpResponse(HttpResponseType.OK, ContentType.TEXT, "No product found")
                            } else {
                                val product = products[id]
                                val resultMessage =
                                    if (product.amount > 0U) "Confirmed".also { product.amount-- } else "Not enough product in stock"
                                productsAccess.writeLock().unlock()
                                HttpResponse(HttpResponseType.OK, ContentType.TEXT, resultMessage)
                            }
                        }

                        else -> HttpResponse(HttpResponseType.NOT_FOUND)
                    }
                }

                else -> HttpResponse(HttpResponseType.NOT_IMPLEMENTED)
            }
    }

}
