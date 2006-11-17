import threading
from VehicleInterface import PropulsionMessage

class Controller ( threading.Thread ):    
    def run ( self ):
        self.instructions = PropulsionMessage(0,0,0,0,0)
        self.message_changed = 1
        while True:
            if self.message_changed:
                self.process_message()
                self.message_changed = 0
                
    def set_message(self,message):
        if self.instructions != message:
            self.message_changed = 1
        self.instructions = message
    def process_message(self):
        self.print_message()
    def print_message(self):
        print self.instructions
