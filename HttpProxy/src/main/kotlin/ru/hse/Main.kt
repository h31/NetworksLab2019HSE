package ru.hse

fun main() {
    val acceptor = Acceptor(3030, ClientHandler(LruCache(), listOf("google.com", "ya.ru")))
    acceptor.start()
}