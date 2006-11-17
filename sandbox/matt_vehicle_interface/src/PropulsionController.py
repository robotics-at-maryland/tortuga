import threading
from VehicleInterface import PropulsionMessage

class Controller ( threading.thread ):
    
    def run ( self ):
        print 'You called my start method, yeah.'
        print 'Were you expecting something amazing?'
    def set_message(self,message):
        self.instructions = message
    def print_message(self):
        print self.instructions
