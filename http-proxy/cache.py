import sys
import logging
from time import time
from operator import itemgetter


class Cache:
    def __init__(self, expire, max_size):
        self.__cache = {}
        self.__expire = expire
        self.__max_size = max_size

    def get(self, request):
        if request.can_cache() and not self.__check_expire(request):
            logging.info("Get from cache: %s" % (str(request)))
            return self.__cache.get(str(request), (None, None))
        else:
            return None, None

    def put(self, request, response):
        self.__clear_old_cache()
        if request.can_cache() and response.can_cache():
            self.__cache[str(request)] = (time(), response)

    def __check_expire(self, request):
        set_time, value = self.__cache.get(str(request), (None, None))
        if set_time is not None:
            if set_time + self.__expire < time():
                self.__cache.pop(request)
                return True
            return False
        return True

    def __check_cache_size(self):
        return sys.getsizeof(self.__cache) < self.__max_size

    def __clear_old_cache(self):
        logging.info("Clear old cache values")
        key_for_deleted = []
        if not self.__check_cache_size():
            for key, value in self.__cache.items():
                if value[0] + self.__expire < time():
                    key_for_deleted.append(key)
        for key in key_for_deleted:
            self.__cache.pop(key)
        if not self.__check_cache_size():
            sorted_by_time_cache = sorted(self.__cache.items(), key=itemgetter(1))
            for key, value in sorted_by_time_cache:
                self.__cache.pop(key)
                if self.__check_cache_size():
                    return
