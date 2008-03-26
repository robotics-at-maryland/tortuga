# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  control/direct.py

"""
Provides a very simple controller for the vehicle that provides direct control
of the vehicle with the keyboard.
"""

# Project Imports
import ram.event as event

# Register Are Event types
event.add_event_types(['PORT_THRUST_UP', 'PORT_THRUST_DOWN',
                       'STARBOARD_THRUST_UP', 'STARBOARD_THRUST_DOWN',
                       'FORE_THRUST_UP', 'FORE_THRUST_DOWN',
                       'AFT_THRUST_UP', 'AFT_THRUST_DOWN'])

class DirectVehicleController(object):
    """
    This listens to key events and will move the vehicle accordingly
    """
    def __init__(self, vehicle):
        self.vehicle = vehicle
        
        self.handler_map = {
             'PORT_THRUST_UP' : self.port_up,
             'PORT_THRUST_DOWN' : self.port_down,
             'STARBOARD_THRUST_UP' : self.starboard_up,
             'STARBOARD_THRUST_DOWN' : self.starboard_down,
             'FORE_THRUST_UP' : self.fore_up,
             'FORE_THRUST_DOWN' : self.fore_down,
             'AFT_THRUST_UP' : self.aft_up,
             'AFT_THRUST_DOWN' : self.aft_down }
            
        event.register_handlers(self.handler_map)
    
    def __del__(self):
        # Make sure to remove event handlers so they are called after the 
        # object is gone
        event.remove_handlers(self.handler_map)
    
    def update(self, time):
        return True
    
    def port_up(self):
        self.vehicle.PortThruster.power += 0.005
    def port_down(self):
        self.vehicle.PortThruster.power -= 0.005
    def starboard_up(self):
        self.vehicle.StarboardThruster.power += 0.005
    def starboard_down(self):
        self.vehicle.StarboardThruster.power -= 0.005
    def fore_up(self):
        self.vehicle.ForeThruster.power += 0.005
    def fore_down(self):
        self.vehicle.ForeThruster.power -= 0.005
    def aft_up(self):
        self.vehicle.AftThruster.power += 0.005
    def aft_down(self):
        self.vehicle.AftThruster.power -= 0.005