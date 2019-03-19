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
        try:
            self.__socket.shutdown(SHUT_WR)
            self.__socket.close()
        except OSError:
            pass
        finally:
            logging.info("Socket closed")

    def receive_message(self):
        parser = HTTParser()
        while True:
            try:
                chunk = self.__socket.recv(MAX_CHUNK_LEN)
            except BrokenPipeError:
                chunk = 0
            if not chunk:
                logging.error("Connection aborted by %s" % self.__host)
                return None
            message = parser.append(chunk)
            if message:
                return message

    def send_message(self, message):
        msg = message.to_bytes()
        total_sent = 0
        while total_sent < len(msg):
            try:
                sent = self.__socket.send(msg[total_sent:])
            except BrokenPipeError:
                sent = 0
            if not sent:
                logging.error("Connection aborted by %s" % self.__host)
                break
            total_sent += sent
