from threading import Thread
from connection import Connection


class Worker(Thread):

    def __init__(self, sock, cache):
        super().__init__()
        self.__socket = sock
        self.__cache = cache

    def run(self):
        client_connection = Connection(self.__socket)
        request = client_connection.receive_message()
        response = self.__cache.get(request)
        if response is None:
            server_connection = Connection()
            server_connection.establish(request.get_host())
            server_connection.send_message(request)
            response = server_connection.receive_message()
            server_connection.close()
        client_connection.send_message(response)
        client_connection.close()
