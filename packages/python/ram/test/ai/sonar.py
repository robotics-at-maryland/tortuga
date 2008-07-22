# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/pipe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.sonar as sonar
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device

import ram.motion as motion
import ram.motion.pipe

import ram.test.ai.support as aisupport

class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.Searching)
    
    def testUpdateLeft(self):
        # To the left of the vehicle
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.5, 0.8, -0.1),
                         pingTimeUSec = 1)
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
        self.assertGreaterThan(self.controller.yawChange, 0)
        
        self.releaseTimer(sonar.Searching.CHANGE)
        self.assertCurrentState(sonar.FarSeeking)
    
    def testUpdateRight(self):
        # To the right
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.8, -0.5, -0.1),
                         pingTimeUSec = 2)
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
        self.assertLessThan(self.controller.yawChange, 0)
        
        self.releaseTimer(sonar.Searching.CHANGE)
        self.assertCurrentState(sonar.FarSeeking)
        
class TransSeekingTestCase(object):
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)     
        
    def testUpdateLeft(self):
        # To the left of the vehicle
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.5, 0.8, -0.1),
                         pingTimeUSec = 9)
        
        # TODO: Figure out why I have to comment this out
        self.assertGreaterThan(self.controller.speed, 0)
        #self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
                
        self.assertCurrentState(self._myClass)
        
    def testUpdateRight(self):
        # To the right
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.8, -0.5, -0.1),
                         pingTimeUSec = 10)
        
        # TODO: Figure out why I have to comment this out
        self.assertLessThan(self.controller.speed, 0)
        #self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        self.assertCurrentState(self._myClass)
        
class TestCloseSeeking(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonar.FarSeeking
        self.machine.start(sonar.FarSeeking)
        
    def testClose(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.8),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        self.assertCurrentState(sonar.CloseSeeking)
        
class TestCloseSeeking(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonar.CloseSeeking
        self.machine.start(sonar.CloseSeeking)
        
    def testClose(self):
        # Subscribe to end event
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(sonar.COMPLETE, complete)
        
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.9),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        
        # Make sure we hit the end state and give off the event
        self.assert_(self.machine.complete)
        self.assert_(self._complete)

class TestHovering(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonar.Hovering
        self.machine.start(sonar.Hovering)
        
    def testClose(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.9),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        self.assertCurrentState(sonar.Hovering)

