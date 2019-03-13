from socket import socket, gethostbyname, AF_INET, SOCK_STREAM, SHUT_WR

from httparser import HTTParser

MAX_CHUNK_LEN = 1024
HTTP_PORT = 80


class Connection:

    def __init__(self, sock=None):
        self.__socket = sock

    def establish(self, url):
        self.__socket = socket(AF_INET, SOCK_STREAM)
        host = gethostbyname(url)
        self.__socket.connect((host, HTTP_PORT))

    def close(self):
        self.__socket.shutdown(SHUT_WR)

    def receive_message(self):
        parser = HTTParser()
        while True:
            chunk = self.__socket.recv(MAX_CHUNK_LEN)
            message = parser.append(chunk.decode())
            if message is not None:
                return message

    def send_message(self, message):
        msg = message.to_str().encode()
        total_sent = 0
        while total_sent < len(msg):
            sent = self.__socket.send(msg[total_sent:])
            if sent == 0:
                break
            total_sent += sent
