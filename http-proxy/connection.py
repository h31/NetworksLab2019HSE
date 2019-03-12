from socket import socket, gethostbyname, AF_INET, SOCK_STREAM, SHUT_WR

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

    def __receive(self):
        chunks = []
        total_received = 0
        while True:
            chunk = self.__socket.recv(MAX_CHUNK_LEN)
            pass
            chunks.append(chunk)
            print(chunk)
            total_received += len(chunk)
        return b''.join(chunks)

    def __send(self, msg):
        total_sent = 0
        while total_sent < msg.len:
            sent = self.__socket.send(msg[total_sent:])
            if sent == 0:
                break
            total_sent += sent

    def receive_message(self):
        pass

    def send_message(self, message):
        pass
