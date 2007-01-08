# Ogre Includes
#import Ogre

# Project Includes
from .. vehicle import VehicleFactory
from _ogresys import OgreSys

class Vehicle(object):          
    def __init__(self, config):
        print "Created Simulated vehicle:", config
        self.ogre = OgreSys()
        self.ogre._setUp(config)
        
    def start_update(self):
        self.ogre.go()


VehicleFactory.createFunc['Sim'] = Vehicle