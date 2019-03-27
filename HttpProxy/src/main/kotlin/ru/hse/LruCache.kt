package ru.hse

import java.util.*


class LruCache<K, V>(val expirationTime: Int = 1000, val cacheSize: Int = 1000) : Cache<K, V> {
    private val cache = Collections.synchronizedMap(LinkedHashMap<K, PageData<V>>())
    override fun lookUp(request: K): V? {
        val data = cache[request] ?: return null
        if (getTime() - data.addedTime > expirationTime) {
            cache.remove(request)
            return null
        }

        data.lookedUpTime = getTime()
        return data.page
    }

    override fun addPage(request: K, response: V) {
        if (cache.size == cacheSize) {
            cache.remove(cache.iterator().next().key)
        }
        val time = getTime()
        cache[request] = PageData(response, time, time)
    }

    // Suppose don't need it
    override fun updateTime(request: K, response: V) {
        if (!cache.containsKey(request)) {
            val time = getTime()
            cache[request] = PageData(response, time, time)
        }

        val data = cache[request]
        data?.lookedUpTime = getTime()
    }

    private fun getTime(): Long = System.currentTimeMillis() / 1000

    data class PageData<V>(val page: V, val addedTime: Long, var lookedUpTime: Long)
}
