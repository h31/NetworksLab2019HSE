package ru.hse

fun main() {
    val cache = LruCache()
    val acceptor = Acceptor(cache, 111)
    acceptor.start()
}