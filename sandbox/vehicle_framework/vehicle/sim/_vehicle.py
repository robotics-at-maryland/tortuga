# Ogre Includes
#import Ogre

# Project Includes
from .. import VehicleFactory
from . import GraphicsSystem, PhysicsSystem, InputSystem, GUISystem

class Vehicle(object):          
    def __init__(self, config):
        print "Created Simulated vehicle:", config
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
        
        
    def update(self, time_since_last_update):
        # Update all components, drop out if one returns false
        for component in self.components:
            if not component.update(time_since_last_update):
                return False
        return True
        

# Register Simuldated Vehicle with Factory
VehicleFactory.createFunc['Sim'] = Vehicle