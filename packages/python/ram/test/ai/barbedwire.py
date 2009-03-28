# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.barbedwire as barbedwire
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support
        
class TestStart(support.AITestCase):
    DEPTH = 7
    DIVE_SPEED = 2
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.barbedwire.Start' : {
                        'diveDepth' : TestStart.DEPTH,
                        'diveSpeed' : TestStart.DIVE_SPEED,
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(barbedwire.Start)
        
    def testStart(self):
        """Make sure we are diving with no target detector on"""
        self.assertFalse(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
    def testConfig(self):
        # Test the config settings
        m = self.motionManager.currentMotion
        self.assertEqual(TestStart.DEPTH, m.desiredDepth)
        self.assertEqual(TestStart.DIVE_SPEED, m.speed)
        
    def testFinish(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(barbedwire.FindAttempt)

class TestFindAttempt(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.FindAttempt)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(type(None))
        self.assertEqual(1, self.controller.headingHolds)
                
    def testTargetFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0)
        self.assertCurrentState(barbedwire.SeekingToRange)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.barbedWireDetector)

    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(barbedwire.FindAttempt)
        
        # Release timer
        self.releaseTimer(barbedwire.FindAttempt.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(barbedwire.Searching)
        self.assert_(self.visionSystem.barbedWireDetector)

        
class TestSearching(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.Searching)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testTargetFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0)
        self.assertCurrentState(barbedwire.SeekingToRange)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.barbedWireDetector)
        
class TestRangeXYHold(support.AITestCase):
    def setUp(self, stateType = barbedwire.RangeXYHold, cfg = None):
        if cfg is None:
            cfg = {}
        self._stateType = stateType
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(stateType)
    
        # Subscribe to in range event
        self._inRange = False
        def inRange(event):
            self._inRange = True
        self.qeventHub.subscribeToType(barbedwire.SeekingToRange.IN_RANGE, 
                                       inRange)
    
    def _injectFoundEvent(self, topX, topY, topWidth):
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = topX, topY = topY, topWidth = topWidth,
                         bottomWidth = -1)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testTargetFound(self):
        """Make sure new found events move the vehicle"""
        self._injectFoundEvent(topX = 0.5, topY = -0.5, topWidth = 0.25)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
    
    def testTargetLost(self):
        """Make sure losing the target goes back to search"""
        self.injectEvent(vision.EventType.BARBED_WIRE_LOST)
        self.assertCurrentState(barbedwire.FindAttempt)
        
    def testInRange(self):
        # Inject and event which has the duct ahead, and at the needed range
        self._injectFoundEvent(topX = 0.05, topY = 0.5, topWidth = 0.53)

        # Make sure we get the IN_RANGE event
        self.qeventHub.publishEvents()
        self.assert_(self._inRange)

class TestRangeXYHoldNoDepth(TestRangeXYHold):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.barbedwire.RangeXYHold' : {
                        'yZero' : 0,
                        'depthGain' : 0,
                    },
                }
            }
        }
        TestRangeXYHold.setUp(self, cfg = cfg)
        
    def testTargetFound(self):
        self._injectFoundEvent(topX = 0.5, topY = -0.5, topWidth = 0.25)
        
        # Bigger numbers = deeper
        self.assertEqual(self.controller.depth, 0)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
        
    def testInRange(self):
        # Inject and event which has the duct ahead, and at the needed range
        self._injectFoundEvent(topX = 0.05, topY = -0.5, topWidth = 0.53)
        
        # Make sure we get the IN_RANGE event
        self.qeventHub.publishEvents()
        self.assert_(self._inRange)

class TestSeekingToRange(TestRangeXYHold):
    def setUp(self):
        TestRangeXYHold.setUp(self, barbedwire.SeekingToRange)
           
    def testInRange(self):
        TestRangeXYHold.testInRange(self)

        # Make sure we ended up in the right place
        self.assertCurrentState(barbedwire.SeekingToAligned)
        
class AlignmentTest(object):
    def testStart(self):
        self.assertCurrentMotion(motion.duct.DuctSeekAlign)
    
    def testTargetFound(self):
        """Make sure new found events move the vehicle"""
        # Target to the left, below, and target misalligned left
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = -0.5, topY = -0.5, topWidth = 0.25,
                         bottomX = 0.75, bottomY = -0.75, bottomWidth = 0.2)
        
        # Bigger numbers = deeper
        #self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertGreaterThan(self.controller.yawChange, 0)
    
    def testTargetLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.BARBED_WIRE_LOST)
        self.assertCurrentState(barbedwire.FindAttempt)

    def _sendAlignEvent(self, topX, bottomX):
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = topX, topY = -0.1, topWidth = 0.5,
                         bottomX = bottomX, bottomY = -0.5,
                         bottomWidth = 0.3)
        
    def testAlignemntMove(self):
        # Inject and event which has the target ahead, and at the needed range
        # Top pipe on right, bottom pipe on left, vehicle should go right
        self._sendAlignEvent(topX = 0, bottomX = -0.5)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.yawChange)

        # Inject and event which has the target ahead, and at the needed range
        # Top pipe on left, bottom pipe on right, vehicle should go left
        self._sendAlignEvent(topX = 0, bottomX = 0.5)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.yawChange)


class TestSeekingToAligned(AlignmentTest, support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.SeekingToAligned)

    def testAligned(self):
        # Subscribe to in range event
        self._aligned = False
        def aligned(event):
            self._aligned = True
        self.qeventHub.subscribeToType(barbedwire.SeekingToAligned.ALIGNED, 
                                       aligned)
        
        # Inject and event which has the target ahead, and at the needed range
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = 0.05, topY = -0.1, topWidth = 0.5,
                         bottomX = 0.03, bottomY = -0.5, bottomWidth = 0.3)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assert_(self._aligned)
        self.assertCurrentState(barbedwire.Aligning)
        
class TestAligning(AlignmentTest, support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.Aligning)
        
    def testStart(self):
        AlignmentTest.testStart(self)
        self.releaseTimer(barbedwire.Aligning.SETTLED)
        self.assertCurrentState(barbedwire.Through)
        
    def testSettle(self):
        self.injectEvent(barbedwire.Aligning.SETTLED)
        self.assertCurrentState(barbedwire.Through)

class Through(support.AITestCase):       
    def testStart(self):
        # Turn this on, so we make sure it goes off
        self.visionSystem.barbedWireDetector = True
        
        # Make sure we start driving forward
        self.machine.start(barbedwire.Through)
        self.assert_(self.controller.speed > 0)
        self.assertEqual(False, self.visionSystem.barbedWireDetector)

        # Subscribe to end events
        self._throughDuct = False
        def throughDuct(event):
            self._throughDuct = True
        self.qeventHub.subscribeToType(barbedwire.Through.FORWARD_DONE, 
                                       throughDuct)
        
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(barbedwire.COMPLETE, complete)
        
        # Now make sure we stop
        self.releaseTimer(barbedwire.Through.FORWARD_DONE)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._throughDuct)
        self.assert_(self._complete)
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)
