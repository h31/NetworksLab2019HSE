from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR, SHUT_WR
from threading import Thread, RLock

from cache import Cache
from worker import Worker

BACKLOG_SIZE = 10


class Acceptor(Thread):
    def __init__(self, address, port):
        super().__init__()
        self.__address = address
        self.__port = port
        self.__server_socket = socket(AF_INET, SOCK_STREAM)
        self.__server_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        self.__server_socket.bind((address, port))
        self.__server_socket.listen(BACKLOG_SIZE)
        self.__interrupted_mutex = RLock()
        self.__interrupted = False

    def interrupt(self):
        with self.__interrupted_mutex:
            socket(AF_INET, SOCK_STREAM).connect((self.__address, self.__port))
            self.__interrupted = True

    def run(self):
        workers = []
        cache = Cache()
        while True:
            (client_socket, _) = self.__server_socket.accept()
            with self.__interrupted_mutex:
                if self.__interrupted:
                    break
            worker = Worker(client_socket, cache)
            workers.append(worker)
            worker.start()
        self.__server_socket.shutdown(SHUT_WR)
        for worker in workers:
            worker.join()
