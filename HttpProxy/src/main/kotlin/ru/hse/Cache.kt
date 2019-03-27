package ru.hse

interface Cache<K, V> {
    fun lookUp(request: K): V?
    fun addPage(request: K, response: V, expiration: Long = 0)

    companion object {
        fun canBeCached(headers: List<String>): Boolean {
            val cachePolicy = headers.find { it.contains("Cache-Control") }?.split(' ')?.get(2) ?: return true
            return !cachePolicy.contains("no-cache") && !cachePolicy.contains("no-store")
        }

        fun getExpirationTime(headers: List<String>): Long {
            val expiration = headers.find { it.contains("Cache-Control") }?.split(' ')?.get(2) ?: return 0
            if (expiration.contains("max-age="))
                return expiration.split('=')[2].toLong()
            return 0
        }
    }

}
