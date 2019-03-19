import sys
import logging
from threading import RLock
from time import time
from operator import itemgetter


class Cache:

    def __init__(self, expire, max_size):
        self.__cache = {}
        self.__expire = expire
        self.__max_size = max_size
        self.__dict_mutex = RLock()
        self.__cache_size = 0

    def get(self, request):
        with self.__dict_mutex:
            if request.can_cache() and not self.__check_expire(request):
                logging.info("Get from cache: %s" % (str(request)))
                return self.__cache.get(str(request), (None, None))
            else:
                return None, None

    def put(self, request, response):
        if not request.can_cache() or not response.can_cache_with_size(self.__max_size):
            return
        with self.__dict_mutex:
            self.__clear_old_cache(response)
            if self.__check_cache_size(response):
                self.__cache[str(request)] = (time(), response)
                self.__cache_size += sys.getsizeof(response)

    def __check_expire(self, request):
        set_time, value = self.__cache.get(str(request), (None, None))
        if set_time is not None:
            if set_time + self.__expire < time():
                self.__pop_key_and_log(request)
                return True
            return False
        return True

    def __check_cache_size(self, response):
        return self.__cache_size + sys.getsizeof(response) < self.__max_size

    def __clear_old_cache(self, response):
        if self.__check_cache_size(response):
            return
        key_for_deleted = []
        for key, value in self.__cache.items():
            if value[0] + self.__expire < time():
                key_for_deleted.append(key)
        for key in key_for_deleted:
            self.__pop_key_and_log(key)
        sorted_by_time_cache = sorted(self.__cache.items(), key=itemgetter(1))
        for key, value in sorted_by_time_cache:
            if self.__check_cache_size(response):
                return
            self.__pop_key_and_log(key)

    def __pop_key_and_log(self, key):
        key = str(key)
        _, value = self.__cache[key]
        logging.info("Remove from cache: %s" % str(value))
        self.__cache_size -= sys.getsizeof(value)
        self.__cache.pop(str(key))
