package ru.hse

fun main() {
    val acceptor = Acceptor(3030, ClientHandler(Cache(), listOf()))
    acceptor.start()
}