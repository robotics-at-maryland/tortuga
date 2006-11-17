import threading
import time
import PropulsionController

class PropulsionMessager(threading.Thread):
    #    interval in seconds that we send propulsion a message
    prop_time = 0.5 #will be set by the propulsion controller
    
    def __init__(self,v):
        self.vehicle = v;
        self.propulsion_controller = PropulsionController.Controller
    def start(self):
        while True:
            self.fire_propulsion_message()
            time.sleep(self.prop_time)
    def fire_propulsion_message(self):
        def send_message():
            self.propulsion_controller.set_message(self.vehicle.propulsion_instruction)  
    """
    Function stops the sending of messages to the propulsion system every
    prop_time seconds
    """
    def stop_propulsion_messaging(self):
        self.cancel()
    """
    Helper function that allows the propulsion messager to repeat
    """
    
        
    def operate(self):
        self.propulsion_controller.start()
        self.fire_propulsion_messaging()