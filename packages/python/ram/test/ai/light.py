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
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

class TestStart(support.AITestCase):
    DEPTH = 5
    SPEED = 1
    
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.light.Start' : {
                        'depth' : TestStart.DEPTH,
                        'speed'  : TestStart.SPEED
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(light.Start)
    
    def testStart(self):
        """Make sure we are diving with no detector on"""
        self.assertFalse(self.visionSystem.redLightDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
    def testConfig(self):
        self.assertEqual(TestStart.DEPTH, 
                         self.motionManager.currentMotion.desiredDepth)
        self.assertEqual(TestStart.SPEED, 
                         self.motionManager.currentMotion.speed)
        
    def testFinish(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(light.Searching)
        
class TestSearching(support.AITestCase):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.light.Searching' : {
                        'legTime' : 10,
                        'sweepAngle'  : 9,
                        'speed' : 8,
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(light.Searching)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.redLightDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        
    def testConfig(self):
        self.assertEqual(10, self.machine.currentState()._legTime)
        self.assertEqual(9, self.machine.currentState()._sweepAngle)
        self.assertEqual(8, self.machine.currentState()._speed)
            
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 5, 
                         6)
        self.assertCurrentState(light.Align)
        self.assertEqual(5, self.ai.data['lastLightEvent'].x)
        self.assertEqual(6, self.ai.data['lastLightEvent'].y)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.redLightDetector)

class TestFindAttempt(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(light.FindAttempt)

    def testLightFound(self):
        self.assertCurrentState(light.FindAttempt)
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 5, 
                         6)
        self.assertCurrentState(light.Align)
        self.assertEqual(5, self.ai.data['lastLightEvent'].x)
        self.assertEqual(6, self.ai.data['lastLightEvent'].y)

    def testTimeout(self):
        self.assertCurrentState(light.FindAttempt)
        self.injectEvent(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(light.Searching)

class TestAlign(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(light.Align)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testLightFound(self):
        """Make sure new found events move the vehicle"""
        # Light  dead ahead and below us
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0,
                         0, y = -0.5, azimuth = math.Degree(15))
        
        # Bigger numbers = deeper, and we want to go deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['lastLightEvent'].x)
        self.assertEqual(-0.5, self.ai.data['lastLightEvent'].y)
        
        # Smaller numbers = shallow, and we want to go shallower
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0,
                         0, y = 0.5, azimuth = math.Degree(15))
        self.assertLessThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['lastLightEvent'].x)
        self.assertEqual(0.5, self.ai.data['lastLightEvent'].y)
    
    def testLightLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.LIGHT_LOST)
        self.assertCurrentState(light.FindAttempt)
        
    def testSeek(self):
        """Make sure we try to hit the light when close"""
        self.injectEvent(ram.motion.seek.SeekPoint.POINT_ALIGNED)
        self.assertEqual(1, self.controller.depthHolds)
        self.assertCurrentState(light.Seek)
        
class TestSeek(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(light.Seek)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPoint)
        
    def testLightFound(self):
        """Make sure new found events move the vehicle"""
        # Light  dead ahead and below us
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0,
                         0, y = -0.5, azimuth = math.Degree(15))
        
        # Bigger numbers = deeper, and the vehicle should not change depth
        self.assertEqual(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['lastLightEvent'].x)
        self.assertEqual(-0.5, self.ai.data['lastLightEvent'].y)
        
        # Smaller numbers = shallow, and the vehicle should not change depth
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 0,
                         0, y = 0.5, azimuth = math.Degree(15))
        self.assertEqual(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['lastLightEvent'].x)
        self.assertEqual(0.5, self.ai.data['lastLightEvent'].y)
    
    def testLightLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.LIGHT_LOST)
        self.assertCurrentState(light.FindAttempt)
        
    def testHit(self):
        """Make sure finding the light changes to hit"""
        self.injectEvent(vision.EventType.LIGHT_ALMOST_HIT)
        self.assertCurrentState(light.Hit)
        
class TestHit(support.AITestCase):       
    def testStart(self):
        # Turn this on, so we make sure it goes off
        self.visionSystem.redLightDetector = True
        
        # Make sure we start driving forward
        self.machine.start(light.Hit)
        self.assert_(self.controller.speed > 0)
        self.assertEqual(False, self.visionSystem.redLightDetector)

        # Subscribe to end event
        self._lightHit = False
        def lightHit(event):
            self._lightHit = True
        self.qeventHub.subscribeToType(light.LIGHT_HIT, lightHit)
        
        # Now make sure we stop
        self.releaseTimer(light.Hit.FORWARD_DONE)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._lightHit)
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)
