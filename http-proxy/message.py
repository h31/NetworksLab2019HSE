from re import split, search

NEW_LINE = b"\r\n"


class Message:

    def __init__(self):
        self.__body = b""
        self.__start_line = b""
        self.__headers = {}

    def set_start_line(self, start_line):
        self.__start_line = start_line

    def add_header(self, header_title, header_value):
        self.__headers[header_title] = header_value

    def append_to_body(self, chunk):
        self.__body += chunk
        return len(self.__body) == self.__get_body_len()

    def get_body(self):
        return self.__body

    def get_status(self):
        return self.__headers.get(b"Status:")

    def to_str(self):
        message = self.__start_line + NEW_LINE
        message += NEW_LINE.join([b"%s %s" % header for header in self.__headers.items()])
        message += NEW_LINE * 2
        if self.__get_body_len() > 0:
            message += self.__body + NEW_LINE
        return message

    def get_host(self):
        host = self.__headers.get(b"Host:")
        if host:
            return split(r":", host)[0]
        else:
            return None

    def can_cache(self):
        return search(r"no-cache|no-store", self.__headers.get(b"Cache-Control:")) is None

    def __get_body_len(self):
        return int(self.__headers.get(b"Content-Length:", b"0"))
