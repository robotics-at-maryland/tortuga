# Ogre Includes
#import Ogre

# Project Includes
from .. vehicle import VehicleFactory
from . import GraphicsSystem
from . import PhysicsSystem

class Vehicle(object):          
    def __init__(self, config):
        print "Created Simulated vehicle:", config
        self.gfx_sys = GraphicsSystem(config)
        self.phy_sys = PhysicsSystem(config, self.gfx_sys)
        
    def __del__(self):
        del self.phy_sys
        del self.gfx_sys
        
    def start_update(self):
        self.gfx_sys.start_update()


VehicleFactory.createFunc['Sim'] = Vehicle