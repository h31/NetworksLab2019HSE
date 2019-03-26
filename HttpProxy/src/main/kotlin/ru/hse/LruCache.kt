package ru.hse

import java.util.*


class LruCache(private val expirationTime: Int = 1000, val cacheSize: Int = 1000) : Cache {
    private val cache = Collections.synchronizedMap(LinkedHashMap<String, PageData>());
    override fun lookUp(request: String): String? {
        val data = cache[request] ?: return null
        if (getTime() - data.addedTime > expirationTime) {
            cache.remove(request)
            return null
        }

        data.lookedUpTime = getTime()
        return data.page
    }

    override fun addPage(request: String, page: String) {
        if (cache.size == cacheSize) {
            cache.remove(cache.iterator().next().key);
        }
        val time = getTime()
        cache[request] = PageData(page, time, time)
    }

    // Suppose don't need it
    override fun updateTime(request: String, page: String) {
        if (!cache.containsKey(request)) {
            val time = getTime()
            cache[request] = PageData(page, time, time)
        }

        val data = cache[request]
        data?.lookedUpTime = getTime()
    }

    private fun getTime(): Long = System.currentTimeMillis() / 1000

    data class PageData(val page: String, val addedTime: Long, var lookedUpTime: Long)
}
