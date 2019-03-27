package ru.hse

fun main() {
    val acceptor = Acceptor(3030, ClientHandler(LruCache(), listOf()))
    acceptor.start()
}