from sys import argv
from acceptor import Acceptor


if __name__ == "__main__":
    port = int(argv[1])
    cache_expire = int(argv[2])
    cache_max_size = int(argv[3])
    my_server = Acceptor("", port, cache_expire, cache_max_size)
    my_server.start()
    x = input("input anything to exit\n")
    my_server.interrupt()
    my_server.join()
