import unittest
from cache import Cache
from message import Message

class Test(unittest.TestCase):

    def test_create_message(self):
        message = Message()
        message.set_start_line("GET /background.png HTTP/1.0")
        message.add_header('Host', 'example.org')
        message.add_header('Content-Length', '110')
        message.append_to_body('<html><body><a href="http://example.com/about.html#contacts">Click here</a></body></html>')
        self.assertEqual('GET /background.png HTTP/1.0\r\nHost: example.org\r\nContent-Length: 110\r\n\r\n<html><body><a href="http://example.com/about.html#contacts">Click here</a></body></html>\r\n', message.to_str())

    def test_get_host_message(self):
        message = Message()
        message.set_start_line("GET /background.png HTTP/1.0")
        message.add_header('Host', 'example.org')
        message.add_header('Content-Length', '110')
        message.append_to_body('<html><body><a href="http://example.com/about.html#contacts">Click here</a></body></html>')
        host = message.get_host()
        self.assertEqual('example.org', host)

    def test_need_cash_message(self):
        message = Message()
        message.set_start_line("GET /background.png HTTP/1.0")
        message.add_header('Host', 'example.org')
        message.add_header('Content-Length', '110')
        message.append_to_body('<html><body><a href="http://example.com/about.html#contacts">Click here</a></body></html>')
        can = message.can_cache()
        message.add_header('Cache-Control', 'no-store')
        cant = message.can_cache()
        self.assertEqual(True, can)
        self.assertEqual(False, cant)