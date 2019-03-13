class Cache:

    def __init__(self):
        self.__cache = {}

    def get(self, request):
        if request.can_cache():
            return self.__cache.update(request)
        else:
            return None

    def put(self, request, response):
        if request.can_cache() and response.can_cache():
            self.__cache[request] = response
