import threading
import time
from VehicleInterface import PropulsionMessage

class Controller ( threading.Thread ): 
    prop_time = 0.5
    def __init__(self,vehicle):
        threading.Thread.__init__(self)
        self.vehicle = vehicle
    def set_vehicle(self,vehicle):
        self.vehicle = vehicle   
    def run ( self ):
        self.instructions = PropulsionMessage(0,0,0,0)
        self.message_changed = 1
        while True:
            self.set_message()
            if self.message_changed:
                self.process_message()
                self.message_changed = 0
            time.sleep(self.prop_time)
    def set_message(self):
        new_message = self.vehicle.propulsion_instruction
        if self.instructions != new_message:
            self.message_changed = 1
            self.instructions = new_message
    def print_message(self):
        print self.instructions
        
    """
    Function that the Controller group will be implementing
    Can call any C code you would like
    """
    def process_message(self):
        self.print_message()
