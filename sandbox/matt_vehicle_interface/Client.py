from socket import *
from threading import *

class SimpleClient(Thread):
    def init(self,host,port):
        self.socket = socket(AF_INET,SOCK_STREAM)
        self.kill_message = "close communication"
        self.port = port
        self.host = host
    def start(self):
        self.socket.connect((self.host,self.port))
        self.input_loop()
    def close(self):
        self.socket.close()
    def write(self,message):
        self.socket.send(message)
    def input_loop(self):
        input = ""
        while input != self.kill_message:
            input = raw_input()
            self.write(input)
            