from message import Message
from enum import Enum
from collections import deque


class HTTParser:

    def __init__(self):
        self.__message = Message()
        self.__state = self.State.READING_START_LINE
        self.__prefix = b""

    def append(self, chunk):
        tokens = self.__tokenize(chunk)
        if self.__parse(tokens):
            return self.__message
        return None

    def __tokenize(self, chunk):
        tokens = deque()
        tokens.extend((self.__prefix + chunk).splitlines())
        if not chunk.endswith(b"\r\n"):
            self.__prefix = tokens[-1]
            tokens.pop()
        else:
            self.__prefix = b""
        return tokens



    class State(Enum):
        READING_START_LINE = 1
        READING_HEADERS = 2
        READING_BODY = 3
