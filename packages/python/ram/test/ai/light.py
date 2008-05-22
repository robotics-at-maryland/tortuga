# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.light as light
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support
        
class TestSearching(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(light.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.redLightDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0, 
                         0)
        self.assertCurrentState(light.Seek)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.redLightDetector)
        
class TestSeek(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(light.Seek)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPoint)
        
    def testLightFound(self):
        """Make sure new found events move the vehicle"""
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0,
                         0, y = -0.5, azimuth = math.Degree(15))
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
    
    def testLightLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.LIGHT_LOST)
        self.assertCurrentState(light.Searching)
        
    def testHit(self):
        """Make sure we try to hit the light when close"""
        self.injectEvent(vision.EventType.LIGHT_ALMOST_HIT)
        self.assertCurrentState(light.Hit)
        
class TestHit(support.AITestCase):       
    def testStart(self):
        # Make our timer blocks in the background
        self.timerBlock = True
        # Turn this on, so we make sure it goes off
        self.visionSystem.redLightDetector = True
        
        # Make sure we start driving forward
        self.machine.start(light.Hit)
        self.assert_(self.controller.speed > 0)
        self.assertEqual(False, self.visionSystem.redLightDetector)
        
        # Now make sure we stop
        self.releaseTimer(self.machine.currentState().timer)
        self.assertEqual(0, self.controller.speed)
        
        self.assertCurrentState(light.End)
