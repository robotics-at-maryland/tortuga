# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/simvehicle.py

"""
    Contains the vehicle class, main object for the vehicle simulation, it
encapsulates all aspects of the GUI, and simulation elements away from the 
control code below.
"""

import logging

from vehicle import VehicleFactory, IVehicle
from vehicle.sim import core
from vehicle.sim.gui import GUISystem
from vehicle.sim.input import InputSystem
from vehicle.sim.physics import PhysicsSystem
from vehicle.sim.graphics import GraphicsSystem

class Vehicle(IVehicle):          
    def __init__(self, config):
        self.scene = None
        self.gui_sys = None
        self.input_sys = None
        self.physics_sys = None
        self.graphics_sys = None
        
        # Create all our components
        self.graphics_sys = GraphicsSystem(config['Graphics'])
        self.physics_sys = PhysicsSystem(config['Physics'], self.graphics_sys)
        self.input_sys = InputSystem(self.graphics_sys)
        self.gui_sys = GUISystem(config['GUI'], self.graphics_sys, 
                                 self.input_sys)
        
        self.components = [self.input_sys, self.physics_sys, 
                           self.graphics_sys]
        
        # load the scene
        self.scene = core.load_scene(config['Scenes'], self.graphics_sys, 
                                     self.physics_sys)
        
    def __del__(self):
        del self.scene
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
VehicleFactory.createFunc['Simulated'] = Vehicle