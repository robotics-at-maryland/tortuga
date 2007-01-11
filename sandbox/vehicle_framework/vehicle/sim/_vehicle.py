import logging

# Project Includes
from .. import VehicleFactory
from . import GraphicsSystem, PhysicsSystem, InputSystem, GUISystem

class Vehicle(object):          
    def __init__(self, config):
        self._setup_logging(config['Logging']) 
        
        self.graphics_sys = GraphicsSystem(config['Graphics'])
        self.physics_sys = PhysicsSystem(config['Physics'], self.graphics_sys)
        self.input_sys = InputSystem(self.graphics_sys)
        self.gui_sys = GUISystem(config['GUI'], self.graphics_sys, 
                                 self.input_sys)
        
        self.components = [self.input_sys, self.physics_sys, 
                           self.graphics_sys]
        
    def __del__(self):
        del self.gui_sys
        del self.input_sys
        del self.physics_sys
        del self.graphics_sys
        
    def _setup_logging(self, config):
        # Setup the config so only critical messages get sent to console
        root = logging.getLogger('')
        
        file_format = logging.Formatter("%(asctime)s %(name)-12s %(levelname)"
                                        "-8s %(message)s")
        console_format = logging.Formatter('%(name)-12s %(message)s')
        
        # Send only critical message to the console, and everything to the 
        # main log file
        console = logging.StreamHandler()
        console.setLevel(logging.CRITICAL)
        file_handler = logging.FileHandler(config['file'], 'w')
        file_handler.setLevel(logging.INFO)
        
        root.addHandler(console)
        root.addHandler(file_handler)
        
        
        
        
    def update(self, time_since_last_update):
        # Update all components, drop out if one returns false
        for component in self.components:
            if not component.update(time_since_last_update):
                return False
        return True
        

# Register Simuldated Vehicle with Factory
VehicleFactory.createFunc['Sim'] = Vehicle