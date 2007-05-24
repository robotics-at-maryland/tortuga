from socket import *
from threading import *

PORT = 8085

class SimpleServer(Thread):
        
    def run(self):
        self.socket = socket(AF_INET,SOCK_STREAM)
        print gethostname()
        self.socket.bind(('',PORT))
        print gethostname()
        print "listening..."
        self.socket.listen(1)
        (self.clientsocket, self.address) = self.socket.accept()
        print "client is at: ", self.address
        self.socket.close()
        self.clientsocket.close()
    
    
