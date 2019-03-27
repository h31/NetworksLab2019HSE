import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.net.Socket
import java.net.URL

fun getRequestFromSocket(socket: Socket) : Request {
    val requestLines = getDataFromSocket(socket)
    return formRequest(requestLines)
}

private fun formRequest(requestLines: List<String>): Request {
    val header = requestLines[0].split(' ')
    val url = URL(header[1])
    return Request(url.host, url.port, header[0], requestLines.joinToString("\n"))
}

fun getResponseFromSocket(socket: Socket) : Response {
    val requestLines = getDataFromSocket(socket)
    return formResponse(requestLines)
}

fun formResponse(requestLines: List<String>): Response {
    return Response(requestLines.joinToString("\n"))
}

fun sendRequest(request: Request) : Response {
    Socket(request.host, request.port).use {
        sendDataToSocket(it, request.data)
        return getResponseFromSocket(it)
    }
}

fun sendResponse(socket: Socket, response: Response) {
    sendDataToSocket(socket, response.data)
}

private fun getDataFromSocket(socket: Socket): List<String> {
    val lines = mutableListOf<String>()
    BufferedReader(InputStreamReader(socket.inputStream)).use {
        var line = it.readLine()
        while (!line.isEmpty()) {
            lines.add(line)
            line = it.readLine()
        }
    }
    return lines
}

private fun sendDataToSocket(socket: Socket, data: String) {
    val writer = BufferedWriter(OutputStreamWriter(socket.outputStream))
    writer.append(data)
    writer.flush()
}
