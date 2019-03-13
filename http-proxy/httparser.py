from message import Message
from enum import Enum


class HTTParser:

    def __init__(self):
        self.__message = Message()
        self.__state = self.State.START_LINE
        self.__prefix = ""

    def append(self, chunk):
        tokens = self.__tokenize(chunk)
        if self.__parse(tokens):
            return self.__message
        return None

    def __tokenize(self, chunk):
        tokens = (self.__prefix + chunk).splitlines()
        if not chunk.endswith("\r\n"):
            self.__prefix = tokens[-1]
            tokens.pop()
        else:
            self.__prefix = ""
        return tokens

    def __parse(self, tokens):
        for token in tokens:
            if self.__state == self.State.START_LINE:
                self.__message.set_start_line(token)
                self.__state = self.State.HEADERS
            elif self.__state == self.State.HEADERS:
                if not token:
                    self.__state = self.State.BODY
                    if self.__message.append_to_body(""):
                        return True
                    continue
                header = token.split(": ", maxsplit=1)
                self.__message.add_header(header[0], header[1])
            elif self.__message.append_to_body(token):
                return True
        return False

    class State(Enum):
        START_LINE = 1
        HEADERS = 2
        BODY = 3
