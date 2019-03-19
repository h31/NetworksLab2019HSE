import sys
from re import split, search
from time import ctime

NEW_LINE = "\r\n"
NEW_LINE_B = b"\r\n"
HTTP_PORT = 80
SUPPORTED_METHODS = ["GET", "POST", "HEAD"]


def not_implemented_response():
    response = Message()
    response.set_start_line("HTTP/1.1 501 Not Implemented")
    return response


class Message:

    def __init__(self):
        self.__body = b""
        self.__start_line = ""
        self.__headers = {}

    def set_start_line(self, start_line):
        self.__start_line = start_line

    def add_header(self, header_title, header_value):
        self.__headers[header_title] = header_value

    def append_to_body(self, chunk):
        self.__body += chunk
        if self.__is_chunked():
            complete = self.__body.endswith(b"0" + NEW_LINE_B * 2)
            if complete:
                self.__body = self.__body[:-2]
            return complete
        return len(self.__body) >= self.__get_body_len()

    def get_body(self):
        return self.__body

    def get_status(self):
        return self.__headers.get("Status")

    def to_bytes(self):
        message = self.__start_line + NEW_LINE
        message += NEW_LINE.join(["%s: %s" % header for header in self.__headers.items()])
        message += NEW_LINE * 2
        message = message.encode()
        if self.__get_body_len() > 0 or self.__is_chunked():
            message += self.__body + NEW_LINE_B
        return message

    def get_host(self):
        host = self.__headers.get("Host")
        if host:
            host = split(r":", host)
            return host[0], int(host[1]) if len(host) > 1 else HTTP_PORT
        return None, None

    def can_cache(self):
        if self.__headers.get("Cache-Control") is None:
            return True
        return search(r"no-cache|no-store", self.__headers.get("Cache-Control")) is None

    def can_cache_with_size(self, cache_size):
        return self.can_cache() and sys.getsizeof(self) <= cache_size

    def is_modify(self):
        return self.get_status() != 304

    def add_modify_request(self, timestamp):
        self.add_header('If-Modified-Since', ctime(timestamp))

    def is_method_supported(self):
        method = self.__start_line.split(" ")[0]
        return method in SUPPORTED_METHODS

    def __get_body_len(self):
        return int(self.__headers.get("Content-Length", "0"))

    def __is_chunked(self):
        return self.__headers.get("Transfer-Encoding", "") == "chunked"

    def __hash__(self):
        return hash(self.__body) + hash(self.__start_line) + hash(self.get_host())

    def __eq__(self, other):
        if not isinstance(other, Message):
            return False
        return (self.__body == other.__body
                and self.__start_line == other.__start_line and self.__headers == other.__headers)

    def __str__(self):
        host, port = self.get_host()
        body = self.__body if self.__body is not None else ""
        if host is not None:
            return "%s %s:%s %s" % (self.__start_line, host, port, body)
        return "%s %s" % (self.__start_line, body)
