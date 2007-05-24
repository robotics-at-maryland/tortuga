from socket import *
from threading import *

class SimpleClient(Thread):
    def init(self,host,port):
        self.socket = socket(AF_INET,SOCK_STREAM)
        self.port = port
        self.host = host
    def start(self):
        self.socket.connect((self.host,self.port))
    def close(self):
        self.socket.close()