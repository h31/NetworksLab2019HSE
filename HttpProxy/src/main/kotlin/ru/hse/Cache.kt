package ru.hse

interface Cache<K, V> {
    fun lookUp(request: K): V?
    fun addPage(request: K, response: V)
    fun updateTime(request: K, response: V)

    companion object {
        fun canBeCached(headers: List<String>): Boolean {
            val cachePolicy = headers.find { it.contains("Cache-Control") }?.split(' ')?.get(2) ?: return true
            return !cachePolicy.contains("no-cache") && !cachePolicy.contains("no-store")
        }
    }

}
