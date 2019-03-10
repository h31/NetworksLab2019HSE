package ru.hse.spb.dns.server

import ru.hse.spb.dns.protocol.asBoolean
import ru.hse.spb.dns.protocol.asPort
import java.io.File
import java.io.IOException
import java.util.concurrent.Executors

private val USAGE = """Usage:
    server <port> <isRecursive> <isRootServer> <configFile>
      Run single server.
      Possible values of <isRecursive> and <isRootServer>: T (true) and F (false)
      example: server 1234 T F configs/config.dns

    -------------------------------------------
    server run-preset <presetFile>
      Run preset with five servers (see configs in resources folder)

""".trimIndent()

fun main(args: Array<String>) {
    if (args.isEmpty()) {
        println(USAGE)
        return
    }

    if (args[0] == "run-preset") {
        if (args.size < 2) {
            println("No path to preset config")
            println("You can yse \"../../server/src/main/resources/dns.preset\" config for example")
            return
        }
        try {
            runServers(File(args[1]))
        } catch (e: IOException) {
            println("Error: ${e.message}")
        }
        return
    }

    val server = createServer(args.joinToString(" "), null) ?: return
    server.serve()
}

private fun runServers(presetFile: File) {
    val exec = Executors.newCachedThreadPool()
    val directory = presetFile.parent
    presetFile.useLines {
        it.forEach { line ->
            if (line.isBlank() || line.startsWith("#")) {
                return@forEach
            }
            val server = createServer(line, directory) ?: return@forEach

            exec.execute {
                server.serve()
            }
        }
    }
}

private fun createServer(line: String, configDirectory: String?): Server? {
    val tokens = line.split(Regex("\\s+"))
    if (tokens.size != 4) {
        println("Incorrect format: $line")
        return null
    }

    val port = tokens[0].asPort()
    val isRecursive = tokens[1].asBoolean()
    val isRootServer = tokens[2].asBoolean()
    val configFile = if (configDirectory != null)
        "$configDirectory/${tokens[3]}"
    else
        tokens[3]
    if (port == null || isRecursive == null || isRootServer == null) {
        println("Incorrect format: $line")
        return null
    }
    return Server(port, isRecursive, isRootServer, configFile)
}