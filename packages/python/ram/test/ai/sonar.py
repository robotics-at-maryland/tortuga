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

class TestSeeking(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.FarSeeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        
        
    def testUpdate(self):
        # To the left of the vehicle
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.5, 0.8, -0.1))
        
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 0)
        self.assertGreaterThan(self.controller.yawChange, 0)
        
        self.assertCurrentState(sonar.FarSeeking)
        
        # To the right
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.8, -0.5, -0.1))
        
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 0)
        self.assertLessThan(self.controller.yawChange, 0)
        
        self.assertCurrentState(sonar.FarSeeking)
        
    def testClose(self):
        self.assertCurrentState(sonar.FarSeeking)
        
        # Be so close as to trigger the transition
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.29, 0.3, -0.85))
        self.qeventHub.publishEvents()
        # Make sure we hit the end state
        # TODO: Figure out why I crash
        #self.assert_(self.machine.complete)
