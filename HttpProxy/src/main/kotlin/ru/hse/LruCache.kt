package ru.hse
import java.util.*

/**
 * An implementation of last recently used cache.
 * After each access to the element the time of last access is updated.
 * So if we want to remove an element because cache is full we remove one that was accessed last.
 */
class LruCache<K, V>(val expirationTime: Long = 1000, val cacheSize: Int = 1000) : Cache<K, V> {
    private val cache = Collections.synchronizedMap(HashMap<K, PageData<V>>())
    private val order = PriorityQueue<Expiration<K>>()
    override fun lookUp(request: K): V? {
        val data = cache[request] ?: return null
        if (getTime() > data.validTill) {
            cache.remove(request)
            return null
        }

        val newTime = getTime()
        synchronized(order) {
            order.remove(Expiration(data.lookedUpTime, request))
            order.add(Expiration(newTime, request))
        }
        data.lookedUpTime = newTime
        return data.page
    }

    override fun addPage(request: K, response: V, expiration: Long) {
        if (cache.size == cacheSize) {
            synchronized(order) {
                val first = order.poll()
                cache.remove(first.key)
            }
        }
        val time = getTime()

        val tillTime = time + (if (expiration != 0L) expiration else this.expirationTime)
        synchronized(order) {
            order.add(Expiration(time, request))
        }
        cache[request] = PageData(response, time, tillTime)
    }

    private fun getTime(): Long = System.currentTimeMillis() / 1000

    data class PageData<V>(val page: V, var lookedUpTime: Long, val validTill: Long)

    data class Expiration<K>(val time: Long, val key: K)
}
