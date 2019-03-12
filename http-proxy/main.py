from sys import argv
from acceptor import Acceptor


if __name__ == '__main__':
    port = int(argv[1])
    my_server = Acceptor('', port)
    my_server.start()
    x = input("input anything to exit")
    my_server.interrupt()
    my_server.join()
