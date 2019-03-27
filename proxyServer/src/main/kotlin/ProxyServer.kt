import java.net.ServerSocket
import kotlin.concurrent.thread

class ProxyServer private constructor(port: Int, val options: ProxyOptions) {

    private val serverSocket : ServerSocket = ServerSocket(port)
    private lateinit var acceptThread : Thread
    private val clients = ArrayList<Client>()

    fun run() {
        acceptThread = initMainThread()
    }

    fun stop() {
        acceptThread.interrupt()
        clients.forEach(Client::stop)
    }

    private fun initMainThread(): Thread {
        return thread {
            while (!Thread.currentThread().isInterrupted) {
                val clientSocket = serverSocket.accept()
                val client = Client.initClient(this, clientSocket)
                clients.add(client)
                client.run()
            }
        }
    }

    companion object {

        fun initServer(port : Int, options: ProxyOptions) : ProxyServer {
            return ProxyServer(port, options)
        }

    }

}