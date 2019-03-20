from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR, SHUT_WR
from threading import Thread, RLock
import logging

from cache import Cache
from worker import Worker

BACKLOG_SIZE = 10


class Acceptor(Thread):
    def __init__(self, address, port, cache_expire, cache_max_size):
        super().__init__()
        logging.info("Accept connection address: %s, port %s" % (address, port))
        self.__address = address
        self.__port = port
        self.__cache_expire = cache_expire
        self.__cache_max_size = cache_max_size
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
        cache = Cache(self.__cache_expire, self.__cache_max_size)
        while True:
            (client_socket, _) = self.__server_socket.accept()
            with self.__interrupted_mutex:
                if self.__interrupted:
                    break
            logging.info("Accept new connection from: %s" % str(client_socket.getpeername()))
            worker = Worker(client_socket, cache)
            workers.append(worker)
            worker.start()
        self.__server_socket.shutdown(SHUT_WR)
        for worker in workers:
            worker.interrupt()
            worker.join()
