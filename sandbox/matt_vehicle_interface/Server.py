from socket import *
from threading import *

class SimpleServer(Thread):
    
    def init(self,port):
        self.kill_message = "close communication"
        self.PORT = port
        
    def run(self):
        self.socket = socket(AF_INET,SOCK_STREAM)
        print gethostname()
        self.socket.bind(('',self.PORT))
        print gethostname()
        print "listening..."
        self.socket.listen(1)
        (self.clientsocket, self.address) = self.socket.accept()
        print "client is at: ", self.address
        
        message = ""
        
        while(message != self.kill_message):
            message = self.clientsocket.recv(50)
            if message != "":
                print message
            
        self.socket.close()
        self.clientsocket.close()
    
    
