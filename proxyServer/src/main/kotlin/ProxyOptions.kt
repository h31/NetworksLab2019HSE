interface ProxyOptions {

    fun filtered(request: Request) : Boolean

    fun getCache() : Cache

}
