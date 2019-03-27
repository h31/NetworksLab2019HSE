import java.util.*

class Cache(private val validTime: Long) {

    private val cache = Collections.synchronizedMap(LinkedHashMap<Request, CachedResponse>())

    fun getPage(request: Request) : Response? {
        val responseCache = cache[request] ?: return null
        return if (isValid(responseCache)) responseCache.response else null
    }

    fun cachePage(request: Request, response: Response) {
        cache[request] = CachedResponse(response, System.currentTimeMillis())
    }

    private fun isValid(responseCache: CachedResponse) : Boolean {
        return System.currentTimeMillis() - responseCache.time < validTime
    }

    data class CachedResponse(
        val response: Response,
        val time: Long
    )
}
