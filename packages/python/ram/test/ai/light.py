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
        self.ai.data['lastLightEvent'] = vision.RedLightEvent()
        self.machine.start(light.FindAttempt)

    def testLightFound(self):
        self.assertCurrentState(light.FindAttempt)
        self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent, 5, 
                         6)
        self.assertCurrentState(light.Align)
        self.assertEqual(5, self.ai.data['lastLightEvent'].x)
        self.assertEqual(6, self.ai.data['lastLightEvent'].y)
        
    def testBackwardsMovement(self):
        # Stop the machine
        self.machine.stop()
        
        # Set the range to a close enough value
        self.ai.data['lastLightEvent'].range = 4
        
        # Restart the machine
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), -4.0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)
        
    def testOutsideRadius(self):
        # Stop the machine
        self.machine.stop()
        
        # Set the light event to outside the radius but not in the inner range
        self.ai.data['lastLightEvent'] = vision.RedLightEvent(0.8, 0.8)
        self.ai.data['lastLightEvent'].range = 7
        
        # Restart the machine
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(type(None))
        self.assertLessThan(self.controller.yawChange, 0)
        
        # Stop the machine and set the last light event to the opposite side
        self.machine.stop()
        self.ai.data['lastLightEvent'].x = -0.8
        
        # Restart the machine
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(type(None))
        self.assertGreaterThan(self.controller.yawChange, 0)
        
    def testForwardsMovement(self):
        # Stop the machine
        self.machine.stop()
        
        # Set the range to far away but outside of the radius
        self.ai.data['lastLightEvent'] = vision.RedLightEvent(0.8, 0.8)
        self.ai.data['lastLightEvent'].range = 10
        
        # Restart the machine
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(type(None))
        self.assertAlmostEqual(self.controller.getSpeed(), 0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)
        self.assertLessThan(self.controller.yawChange, 0)
        
        # Now stop the machine and give it a valid value
        self.machine.stop()
        self.ai.data['lastLightEvent'] = vision.RedLightEvent()
        self.ai.data['lastLightEvent'].range = 10
        
        # Star the machine again
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), 1, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)
        
    def testOtherCases(self):
        # Stop the machine
        self.machine.stop()
        
        # Set the range to in between the two ranges
        self.ai.data['lastLightEvent'].range = 6
        
        # Restart the machine
        self.machine.start(light.FindAttempt)
        self.assertCurrentMotion(type(None))
        self.assertAlmostEqual(self.controller.getSpeed(), 0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

    def testTimeout(self):
        self.assertCurrentState(light.FindAttempt)
        self.releaseTimer(state.FindAttempt.TIMEOUT)
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
        # Add a lastLightEvent for FindAttempt
        self.ai.data['lastLightEvent'] = vision.RedLightEvent()
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
        # Add a lastLightEvent for FindAttempt
        self.ai.data['lastLightEvent'] = vision.RedLightEvent()
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
        
        # Now make sure we change direction
        self.releaseTimer(light.Hit.FORWARD_DONE)
        self.assertLessThan(self.controller.speed, 0)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._lightHit)
        
        # Make sure it goes to Continue
        self.assertCurrentState(light.Continue)

class TestContinue(support.AITestCase):
    def testStart(self):
        # Start the state machine
        self.machine.start(light.Continue)
        
        # Check to make sure it is in the first motion
        cstate = self.machine.currentState()
        
        self.assertCurrentState(light.Continue)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertLessThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        
        # Upward motion
        cstate._backward._finish()
        self.qeventHub.publishEvents()
        
        self.assertCurrentState(light.Continue)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        # Forward motion
        cstate._upward._finish()
        self.qeventHub.publishEvents()
        
        self.assertCurrentState(light.Continue)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        
        # Finish motion
        cstate._forward._finish()
        self.qeventHub.publishEvents()
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)
        self.assertCurrentMotion(type(None))