# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion/support.py

"""
Provides support classes for the testing of the Motion subsystem
"""

# STD Imports
import unittest

# Project Imports
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle
import ext.math

import ram.motion as motion
import ram.motion.basic

# Mock subsystems
class MockController(control.IController):
    def __init__(self, eventHub):
        control.IController.__init__(self, "Controller", eventHub)
        self.depth = 0
        self.yawChange = 0
        self.speed = 0
        self.sidewaysSpeed = 0
        self.desiredOrientation = ext.math.Quaternion.IDENTITY
        
    def setDepth(self, depth):
        self.depth = depth
        
    def getDepth(self):
        return self.depth
        
    def setSpeed(self, speed):
        self.speed = speed
        
    def setSidewaysSpeed(self, speed):
        self.sidewaysSpeed = speed
        
    def getSpeed(self):
        return self.speed
    
    def getSidewaysSpeed(self):
        return self.sidewaysSpeed
        
    def yawVehicle(self, yawChange):
        self.yawChange = yawChange
        
    def getDesiredOrientation(self):
        return self.desiredOrientation
        
    def publishAtDepth(self, vehicleDepth):
        event = ext.math.NumericEvent()
        event.number = vehicleDepth
        self.publish(control.IController.AT_DEPTH, event)
        
    def publishAtOrientation(self, vehicleOrientation):
        event = ext.math.OrientationEvent()
        event.orientation = vehicleOrientation
        self.publish(control.IController.AT_ORIENTATION, event)
        
        
class MockVehicle(vehicle.IVehicle):
    def __init__(self):
        vehicle.IVehicle.__init__(self, "B")
        self.depth = 0
        self.orientation = ext.math.Quaternion.IDENTITY
        
    def getDepth(self):
        return self.depth
    
    def getOrientation(self):
        return self.orientation
    

# Mock Motion
class MockMotion(object):
    def __init__(self):
        self.controller = None
        self.vehicle = None
        self.stoped = False
    
    def start(self, controller, vehicle, eventHub, eventPublisher):
        self.controller = controller
        self.vehicle = vehicle
        
    def stop(self):
        self.stoped = True
        
# Provides basic test support
class MotionTest(unittest.TestCase):
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle()
        self.controller = MockController(self.eventHub)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.qeventHub, self.eventHub]
        self.motionManager = motion.basic.MotionManager({}, deps)
