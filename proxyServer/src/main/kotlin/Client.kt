import java.net.Socket
import kotlin.concurrent.thread

class Client private constructor(val server: ProxyServer, val socket: Socket) {

    private lateinit var mainThread : Thread

    fun run() {
        mainThread = initMainThread()
    }

    fun stop() {
        mainThread.interrupt()
    }

    private fun initMainThread() : Thread {
        return thread {
            while (!Thread.currentThread().isInterrupted) {
                socket.use {
                    val request : Request = getRequestFromSocket(it)
                    processRequest(request)
                }
            }
        }
    }

    private fun processRequest(request: Request) {
        if (server.options.filtered(request)) {
            sendResponse(socket, Response(
                """
                    HTTP/1.0 200 OK
                    Content-Length: 11

                    Error
                """
            ))
            return
        }
        if (request.type == "GET") {
            val cached = server.options.getCache().getPage(request)
            if (cached != null) {
                sendResponse(socket, cached)
                return
            } else {
                val response = sendRequest(request)
                server.options.getCache().cachePage(request, response)
                sendResponse(socket, response)
            }
        } else {
            val response = sendRequest(request)
            sendResponse(socket, response)
        }
    }

    companion object {

        fun initClient(server: ProxyServer, socket: Socket) : Client {
            return Client(server, socket)
        }

    }
}
