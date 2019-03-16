from threading import Thread
from connection import Connection
import logging
import time


class Worker(Thread):

    def __init__(self, sock, cache):
        super().__init__()
        self.__socket = sock
        self.__cache = cache
        self.__client_connection = Connection(self.__socket)

    def interrupt(self):
        self.__client_connection.close()

    def run(self):
        request = self.__client_connection.receive_message()
        logging.info("Get request: %s" % (str(request)))
        timestamp, response = self.__cache.get(request)
        if timestamp is not None:
            request.add_header('If-Modified-Since', time.ctime(timestamp))
            server_connection = Connection()
            server_connection.establish(request.get_host())
            server_connection.send_message(request)
            tmp_response = server_connection.receive_message()
            if tmp_response.get_status() != 304:
                response = None
        if response is None:
            server_connection = Connection()
            server_connection.establish(request.get_host())
            server_connection.send_message(request)
            response = server_connection.receive_message()
            server_connection.close()
        self.__client_connection.send_message(response)
        logging.info("Send response: %s" % (str(request)))
        self.__cache.put(request, response)
        self.__client_connection.close()
