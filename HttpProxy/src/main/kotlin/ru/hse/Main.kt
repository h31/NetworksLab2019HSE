package ru.hse

fun main() {
    val cache = Cache()
    val acceptor = Acceptor(cache, 111)
    acceptor.start()
}