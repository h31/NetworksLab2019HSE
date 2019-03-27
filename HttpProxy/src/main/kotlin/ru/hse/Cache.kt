package ru.hse

interface Cache<K, V> {
    fun lookUp(request: K): V?
    fun addPage(request: K, response: V)
    fun updateTime(request: K, response: V)
}
