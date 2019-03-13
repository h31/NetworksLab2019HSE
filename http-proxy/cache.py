import time
import sys


class Cache:
    def __init__(self, expire, max_size):
        self.__cache = {}
        self.__expire = expire
        self.__max_size = max_size

    def get(self, request):
        if request.can_cache():
            return self.__check_expire(request)
        else:
            return None

    def put(self, request, response):
        if request.can_cache() and response.can_cache() and self.__clear_old_cache:
            self.__cache[request] = (time.time(), response)

    def __check_expire(self, request):
        set_time, value = self.__cache.get(request)
        if set_time + self.__expire > time.time():
            self.__cache.pop(request)
            return None
        else:
            return value

    def __check_cache_size(self):
        return sys.getsizeof(self.__cache) < self.__max_size

    def __clear_old_cache(self):
        if not self.__check_cache_size():
            for key, set_time, value in dict.items():
                if set_time + self.__expire > time.time():
                    self.__cache.pop(key)
        return self.__check_cache_size()
