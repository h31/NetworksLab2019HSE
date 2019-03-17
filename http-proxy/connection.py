from socket import socket, AF_INET, SOCK_STREAM, SHUT_WR
import logging

from httparser import HTTParser

MAX_CHUNK_LEN = 1024


class Connection:

    def __init__(self, sock=None):
        self.__socket = sock
        self.__host = "client"

    def establish(self, host):
        self.__host = host
        self.__socket = socket(AF_INET, SOCK_STREAM)
        self.__socket.connect(self.__host)

    def close(self):
        self.__socket.shutdown(SHUT_WR)

    def receive_message(self):
        parser = HTTParser()
        while True:
            chunk = self.__socket.recv(MAX_CHUNK_LEN)
            if not chunk:
                logging.error("Connect aborted %s" % self.__host)
                raise ConnectionAbortedError(self.__host)
            message = parser.append(chunk)
            if message:
                return message

    def send_message(self, message):
        msg = message.to_bytes()
        total_sent = 0
        while total_sent < len(msg):
            sent = self.__socket.send(msg[total_sent:])
            if sent == 0:
                break
            total_sent += sent
