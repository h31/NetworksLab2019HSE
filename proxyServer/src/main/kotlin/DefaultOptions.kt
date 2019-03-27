class DefaultOptions : ProxyOptions {

    private val cache = Cache(1000000)

    override fun filtered(request: Request): Boolean {
        return false
    }

    override fun getCache(): Cache {
        return cache
    }

}