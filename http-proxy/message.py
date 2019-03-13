class Message:

    def __init__(self):
        self.__body = ""
        self.__start_line = ""
        self.__headers = []

    def set_start_line(self, start_line):
        self.__start_line = start_line

    def add_header(self, header):
        pass

    def set_body(self, body):
        self.__body = body

    def get_body(self):
        return self.__body

    def get_status(self):
        pass

    def to_str(self):
        pass

    def get_host(self):
        pass
