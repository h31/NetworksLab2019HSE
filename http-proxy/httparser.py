from message import Message, NEW_LINE_B


class HTTParser:

    def __init__(self):
        self.__message = Message()
        self.__head = ""

    def append(self, chunk):
        if self.__head is None:
            return self.__parse_body_part(chunk)
        if NEW_LINE_B * 2 not in chunk:
            self.__head += chunk.decode()
            return None
        parts = chunk.split(NEW_LINE_B * 2, 1)
        self.__head += parts[0].decode()
        self.__parse_head()
        return self.__parse_body_part(parts[1] if len(parts) > 1 else b"")

    def __parse_head(self):
        tokens = self.__head.splitlines()
        self.__message.set_start_line(tokens[0])
        for token in tokens[1:]:
            header = token.split(": ", maxsplit=1)
            self.__message.add_header(header[0], header[1])
        self.__head = None

    def __parse_body_part(self, chunk):
        return self.__message if self.__message.append_to_body(chunk) else None
