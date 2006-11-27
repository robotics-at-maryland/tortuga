# Ogre Includes
import ogre

# Project Includes
from .. vehicle import VehicleFactory

class Vehicle(object):          
    def __init__(self, val):
        print "Created Simulated vehicle: %s" % val

        # Ogre stuff

    def _setUp(self):


VehicleFactory.createFunc['Sim'] = Vehicle