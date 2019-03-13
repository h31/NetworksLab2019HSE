import re

class Message:

    def __init__(self):
        self.__body = ""
        self.__start_line = ""
        self.__headers = {}

    def set_start_line(self, start_line):
        self.__start_line = start_line

    def add_header(self, header_title, header_value):
        self.__headers[header_title] = header_value

    def append_to_body(self, body):
        self.__body += body
        if len(self.__body) == self.__get_body_len():
            return True
        else:
            return False

    def get_body(self):
        return self.__body

    def get_status(self):
        return self.__headers.get('Status:')

    def to_str(self):
        message = self.__start_line
        for key, value in self.__headers.items():
            message += key + ' ' + value
        message += '\r\n'
        message += self.__body
        return message

    def get_host(self):
        if self.__headers.get('Host:') is not None:
            return re.split(r":", self.__headers.get('Host:'))[0]
        else:
            return None

    def can_cache(self):
        return re.search(r"no-cache|no-store", self.__headers.get('Host:')) is None

    def __get_body_len(self):
        return int(self.__headers.get('Content-Length:'))
