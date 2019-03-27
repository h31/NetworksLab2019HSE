fun main() {
    val proxy = ProxyServer.initServer(9880, DefaultOptions())
    proxy.run()
}
