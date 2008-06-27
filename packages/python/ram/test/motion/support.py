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

import ram.timer as timer
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
    def __init__(self, eventHub = core.EventHub()):
        vehicle.IVehicle.__init__(self, "Vehicle", eventHub)
        self.depth = 0
        self.orientation = ext.math.Quaternion.IDENTITY
        
    def getDepth(self):
        return self.depth
    
    def getOrientation(self):
        return self.orientation
    
    def publishOrientationUpdate(self, vehicleOrientation):
        event = ext.math.OrientationEvent()
        event.orientation = vehicleOrientation
        self.publish(vehicle.IVehicle.ORIENTATION_UPDATE, event)
    
# For testing purposes
class MockTimer(timer.Timer):
    ORIG = timer.Timer
    LOG = {}
    
    def __init__(self, eventPublisher, eventType, sleepTime, repeat = False):
        MockTimer.ORIG.__init__(self, eventPublisher, eventType, sleepTime)
        
        self.sleepTime = sleepTime
        self.started = False
        self.repeat = repeat
        
        # Log the timer so we can reference it in our tests
        MockTimer.LOG[eventType] = self
        
    def run(self):
        pass
        
    def start(self):
        self.started = True
        
    def finish(self):
        """
        Fires off the finish event
        """
        self._complete()

# Mock Motion
class MockMotion(object):
    def __init__(self, type = motion.basic.Motion.NORMAL):
        self.controller = None
        self.vehicle = None
        self.stoped = False
        self.type = type
        self.started = False
    
    def start(self, controller, vehicle, eventHub, eventPublisher):
        assert not self.started

        self.controller = controller
        self.vehicle = vehicle
        self.started = True
        
    def stop(self):
        self.stoped = True
        
# Provides basic test support
class MotionTest(unittest.TestCase):
    def mockSleep(self, seconds):
        self.seconds = seconds
    
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle(self.eventHub)
        self.controller = MockController(self.eventHub)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.qeventHub, self.eventHub]
        self.motionManager = motion.basic.MotionManager({}, deps)

        # Replace Timer with out Mock Timer Class
        MockTimer.LOG = {}
        timer._origTimer = timer.Timer
        timer.Timer = MockTimer

    def tearDown(self):
        # Put the original timer class back
        timer.Timer = timer._origTimer
        del timer._origTimer
