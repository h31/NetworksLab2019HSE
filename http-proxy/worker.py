from threading import Thread
from connection import Connection
import logging


class Worker(Thread):

    def __init__(self, sock, cache):
        super().__init__()
        self.__socket = sock
        self.__cache = cache
        self.__client_connection = Connection(self.__socket)
        logging.info("Start new worker")

    def interrupt(self):
        logging.info("Interrupt worker")
        self.__client_connection.close()

    def run(self):
        request = self.__client_connection.receive_message()
        logging.info("Get request: %s" % (str(request)))
        response = self.__get_from_cache_with_check(self.__cache.get(request), request)
        if response is None:
            response = Worker.__connect_and_send(request)
        self.__client_connection.send_message(response)
        logging.info("Send response: %s" % (str(request)))
        self.__cache.put(request, response)
        self.__client_connection.close()

    @staticmethod
    def __connect_and_send(request):
        server_connection = Connection()
        server_connection.establish(request.get_host())
        server_connection.send_message(request)
        response = server_connection.receive_message()
        server_connection.close()
        return response

    @staticmethod
    def __get_from_cache_with_check(cache_answer, request):
        timestamp, response = cache_answer
        if timestamp is not None:
            request.add_modify_request(timestamp)
            tmp_response = Worker.__connect_and_send(request)
            if tmp_response.is_modify():
                response = None
        return response
