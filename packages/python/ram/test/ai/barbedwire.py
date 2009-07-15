# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.state as state
import ram.ai.barbedwire as barbedwire
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search
import ram.motion.pipe

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
        self.assertAIDataValue('barbedWireStartOrientation', 0)
        
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
        self.ai.data['lastBarbedWireEvent'] = vision.BarbedWireEvent()
        self.machine.start(barbedwire.FindAttempt)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(type(None))
        self.assertEqual(1, self.controller.headingHolds)
                
    def testTargetFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0.5, -0.5, 0, 0, 0, 0)
        self.assertCurrentState(barbedwire.FarSeekingToRange)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.5)
        
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
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(barbedwire.Searching)
        self.assert_(self.visionSystem.barbedWireDetector)

    def testNoEvent(self):
        # Stop the machine
        self.machine.stop()

        # Remove the barbed wire event
        del self.ai.data['lastBarbedWireEvent']

        # Restart the machine
        self.machine.start(barbedwire.FindAttempt)
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)

        # Now inject an event to make sure it works
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0.5, -0.5, 0, 0, 0, 0)
        self.assertCurrentState(barbedwire.FarSeekingToRange)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.5)
        
class TestSearching(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.Searching)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('barbedWireStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()

        # Now set the initial direction to something other than 0
        self.ai.data['barbedWireStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(barbedwire.Searching)
        self.assert_(self.visionSystem.barbedWireDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('barbedWireStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)
                
    def testTargetFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0)
        self.assertCurrentState(barbedwire.FarSeekingToRange)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, 0)
        
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
    
    def _injectFoundEvent(self, topX, topY, topWidth, 
                          bottomX = 0, bottomY = 0, bottomWidth = -1):
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = topX, topY = topY, topWidth = topWidth,
                         bottomX = bottomX, bottomY = bottomY,
                         bottomWidth = bottomWidth)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testTargetFound(self):
        """Make sure new found events move the vehicle"""
        self._injectFoundEvent(topX = 0.5, topY = -0.5, topWidth = 0.25)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.25)
        
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
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.25)
        
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

class TestFarSeekingToRange(TestRangeXYHold):
    def setUp(self, myState = barbedwire.FarSeekingToRange,
              nextState = barbedwire.FarSeekingToAligned):
        self.myState = myState
        self.nextState = nextState
        TestRangeXYHold.setUp(self, self.myState)
           
    def testInRange(self):
        TestRangeXYHold.testInRange(self)

        # Make sure we ended up in the right place
        self.assertCurrentState(self.nextState)
        
    def testSideBySidePipesLeft(self):
        # Test top on the left
        self._injectFoundEvent(topX = -0.25, topY = 0.5, topWidth = 0.2,
                               bottomX = 0.25, bottomY = 0.25, 
                               bottomWidth = 0.3)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, -0.25)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.2)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomX, 0.25)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomY, 0.25)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomWidth, 0.3)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.nextState)
        
    def testSideBySidePipesRight(self):
        # Test top on the right
        self._injectFoundEvent(topX = 0.25, topY = 0.25, topWidth = 0.2,
                               bottomX = -0.25, bottomY = 0.5, 
                               bottomWidth = 0.3)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.nextState)
        
class TestCloseSeekingToRange(TestFarSeekingToRange):
    def setUp(self):
        TestFarSeekingToRange.setUp(self, myState = barbedwire.CloseSeekingToRange,
                                    nextState = barbedwire.CloseSeekingToAligned)
        
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
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.25)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomX, 0.75)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomY, -0.75)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomWidth, 0.2)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertGreaterThan(self.controller.yawChange, 0)
    
    def testTargetLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.BARBED_WIRE_LOST)
        self.assertCurrentState(barbedwire.FindAttempt)

    def _sendAlignEvent(self, topX, bottomX, topY = 0.5):
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = topX, topY = topY, topWidth = 0.5,
                         bottomX = bottomX, bottomY = -0.5,
                         bottomWidth = 0.3)
        
    def testAlignemntMove(self):
        # Inject and event which has the target ahead, and at the needed range
        # Top pipe on right, bottom pipe on left, vehicle should go right
        self._sendAlignEvent(topX = 0, bottomX = -0.5)
        self.assertEqual(self.controller.depth, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.yawChange)

        # Inject and event which has the target ahead, and at the needed range
        # Top pipe on left, bottom pipe on right, vehicle should go left
        self._sendAlignEvent(topX = 0, bottomX = 0.5)
        self.assertEqual(self.controller.depth, 0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.yawChange)

    def testDepth(self):
        # Ensure no change when the gain is zero
        state = self.machine.currentState()
        state._depthGain = 0

        self._sendAlignEvent(topX = 0, bottomX = 0, topY = 0.1)
        self.assertEqual(self.controller.depth, 0)
        
        # Ensure we have change when the gain is not zero
        state._depthGain = 1

        # Top pipe is a little low, so we need to dive more
        self._sendAlignEvent(topX = 0, bottomX = 0, topY = 0.1)
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)


class TestFarSeekingToAligned(AlignmentTest, support.AITestCase):
    def setUp(self, myState = barbedwire.FarSeekingToAligned,
              nextState = barbedwire.CloseSeekingToRange):
        support.AITestCase.setUp(self)
        self.myState = myState
        self.nextState = nextState
        self.machine.start(self.myState)

        # Subscribe to in range event
        self._aligned = False
        def aligned(event):
            self._aligned = True
        self.qeventHub.subscribeToType(barbedwire.SeekingToAligned.ALIGNED, 
                                       aligned)

    def testAligned(self):
        # Inject and event which has the target ahead, and at the needed range
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = 0.05, topY = -0.1, topWidth = 0.5,
                         bottomX = 0.03, bottomY = -0.5, bottomWidth = 0.3)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.05)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.1)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomX, 0.03)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomY, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomWidth, 0.3)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assert_(self._aligned)
        self.assertCurrentState(self.nextState)
        
    def testAlignedBadRange(self):
        # Inject and event which has the target ahead, and at the needed range
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = 0.05, topY = -0.1, topWidth = 0.8,
                         bottomX = 0.03, bottomY = -0.5, bottomWidth = 0.3)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assertFalse(self._aligned)
        self.assertCurrentState(self.myState)

    def testAlignedNoBottom(self):
        # Make sure we aren't called aligned if there is no bottom pipe
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = 0.05, topY = -0.1, topWidth = 0.5,
                         bottomX = 0, bottomY = 0, bottomWidth = -1)
        
        # Make sure we didn't get the aligned event and stayed in the same state
        self.qeventHub.publishEvents()
        self.assertFalse(self._aligned)
        self.assertCurrentState(self.myState)
        
class TestCloseSeekingToAligned(TestFarSeekingToAligned):
    def setUp(self):
        TestFarSeekingToAligned.setUp(self, myState = barbedwire.CloseSeekingToAligned,
                                      nextState = barbedwire.Aligning)
        
    def testAligned(self):
        # Inject and event which has the target ahead, and at the needed range
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = 0.05, topY = -0.1, topWidth = 0.75,
                         bottomX = 0.03, bottomY = -0.5, bottomWidth = 0.3)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topX, 0.05)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topY, -0.1)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].topWidth, 0.75)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomX, 0.03)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomY, -0.5)
        self.assertEqual(self.ai.data['lastBarbedWireEvent'].bottomWidth, 0.3)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assert_(self._aligned)
        self.assertCurrentState(self.nextState)
        
class TestAligning(AlignmentTest, support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.Aligning)
        
    def testStart(self):
        AlignmentTest.testStart(self)
        self.releaseTimer(barbedwire.Aligning.SETTLED)
        self.assertCurrentState(barbedwire.Under)
        
    def testSettle(self):
        self.injectEvent(barbedwire.Aligning.SETTLED)
        self.assertCurrentState(barbedwire.Under)

class TestUnder(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(barbedwire.Under)

    def _injectFoundEvent(self, topX, topY, topWidth, 
                          bottomX = 0, bottomY = 0, bottomWidth = -1):
        self.injectEvent(vision.EventType.BARBED_WIRE_FOUND, 
                         vision.BarbedWireEvent, 0, 0, 0, 0, 0, 0,
                         topX = topX, topY = topY, topWidth = topWidth,
                         bottomX = bottomX, bottomY = bottomY,
                         bottomWidth = bottomWidth)

    def testStraight(self):
        # Inject the BarbedWire event
        self._injectFoundEvent(0, 0, 0.5)

        # Test to make sure its only going straight
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        self.assertAlmostEqual(self.controller.yawChange, 0, 5)

    def testOffCenter(self):
        # Inject the BarbedWire event to the right of the vehicle
        self._injectFoundEvent(0.5, 0, 0.5)

        # Test to make sure it strafes and moves forward
        # It does NOT yaw
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(self.controller.yawChange, 0, 5)

        # Inject one on the left
        self._injectFoundEvent(-0.5, 0, 0.5)
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(self.controller.yawChange, 0, 5)

        # Inject one above, it should not change depth
        self._injectFoundEvent(0, -0.5, 0.5)
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        self.assertAlmostEqual(self.controller.yawChange, 0, 5)
        self.assertAlmostEqual(self.controller.depth, 0, 5)

        # Inject one below, it should not change depth
        self._injectFoundEvent(0, 0.5, 0.5)
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        self.assertAlmostEqual(self.controller.yawChange, 0, 5)
        self.assertAlmostEqual(self.controller.depth, 0, 5)
        
    def testTooHigh(self):
        # Inject one barbed wire above the y threshold
        self._injectFoundEvent(topX = -0.5, topY = 0.9, topWidth = 0.5)
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)
        
        # Inject a full barbed wire event and make sure it only follows the
        # bottom wire
        self._injectFoundEvent(topX = -0.5, topY = 0.9, topWidth = 0.5,
                               bottomX = 0.5, bottomY = 0.7, bottomWidth = 0.5)
        self.assertCurrentMotion(motion.pipe.Follow)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)

class TestThrough(support.AITestCase):       
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
