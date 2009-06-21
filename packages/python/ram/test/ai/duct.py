# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.duct
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support
        
class TestSearching(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(ram.ai.duct.Searching)
    
    def tearDown(self):
        support.AITestCase.tearDown(self)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.ductDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testDuctFound(self):
        # Now change states
        self.injectEvent(vision.EventType.DUCT_FOUND, 
                         vision.DuctEvent, 0, 0, 0, 0, False, False)
        self.assertCurrentState(ram.ai.duct.SeekingToRange)
        self.assertEqual(0, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(0, self.ai.data['lastDuctEvent'].y)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.ductDetector)
        
class TestFindAttempt(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(ram.ai.duct.FindAttempt)

    def testDuctFound(self):
        self.assertCurrentState(ram.ai.duct.FindAttempt)
        self.injectEvent(vision.EventType.DUCT_FOUND, vision.DuctEvent, 5, 
                         6, 0, 0, False, False)
        self.assertCurrentState(ram.ai.duct.SeekingToRange)
        self.assertEqual(5, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(6, self.ai.data['lastDuctEvent'].y)

    def testTimeout(self):
        self.assertCurrentState(ram.ai.duct.FindAttempt)
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(ram.ai.duct.Searching)
        
class TestSeekingToRange(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(ram.ai.duct.SeekingToRange)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testDuctFound(self):
        """Make sure new found events move the vehicle"""
        self.injectEvent(vision.EventType.DUCT_FOUND, 
                         vision.DuctEvent, 0, 0, 0, 0, False, False,
                         x = 0.5, y = -0.5, range = 0.6, rotation = 90,
                         aligned = False, visible = False)
        self.assertEqual(0.5, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(-0.5, self.ai.data['lastDuctEvent'].y)
        self.assertEqual(0.6, self.ai.data['lastDuctEvent'].range)
        self.assertEqual(90, self.ai.data['lastDuctEvent'].rotation)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].aligned)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].visible)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertLessThan(self.controller.yawChange, 0)
    
    def testDuctLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.DUCT_LOST)
        self.assertCurrentState(ram.ai.duct.FindAttempt)
        
    def testInRange(self):
        # Subscribe to in range event
        self._inRange = False
        def inRange(event):
            self._inRange = True
        self.qeventHub.subscribeToType(ram.ai.duct.SeekingToRange.IN_RANGE, 
                                       inRange)
        
        # Inject and event which has the duct ahead, and at the needed range
        self.injectEvent(vision.EventType.DUCT_FOUND, 
                         vision.DuctEvent, 0, 0, 0, 0, False, False,
                         x = 0.05, y = -0.1, range = 0.31, rotation = 90,
                         aligned = False, visible = False)
        self.assertEqual(0.05, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(-0.1, self.ai.data['lastDuctEvent'].y)
        self.assertEqual(0.31, self.ai.data['lastDuctEvent'].range)
        self.assertEqual(90, self.ai.data['lastDuctEvent'].rotation)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].aligned)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].visible)
        
        # Make sure we get the IN_RANGE event
        self.qeventHub.publishEvents()
        self.assert_(self._inRange)
        self.assertCurrentState(ram.ai.duct.SeekingToAligned)

class AlignmentTest(object):
    def testStart(self):
        self.assertCurrentMotion(motion.duct.DuctSeekAlign)
    
    def testDuctFound(self):
        """Make sure new found events move the vehicle"""
        # Target to the right, below, and duct misalligned left
        self.injectEvent(vision.EventType.DUCT_FOUND, 
                         vision.DuctEvent, 0, 0, 0, 0, False, False,
                         x = 0.5, y = -0.5, range = 0.6, alignment = 90,
                         aligned = False, visible = False)
        self.assertEqual(0.5, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(-0.5, self.ai.data['lastDuctEvent'].y)
        self.assertEqual(0.6, self.ai.data['lastDuctEvent'].range)
        self.assertEqual(90, self.ai.data['lastDuctEvent'].alignment)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].aligned)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].visible)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertLessThan(self.controller.yawChange, 0)
    
    def testDuctLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.DUCT_LOST)
        self.assertCurrentState(ram.ai.duct.FindAttempt)

class TestSeekingToAligned(AlignmentTest, support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(ram.ai.duct.SeekingToAligned)

    def testAligned(self):
        # Subscribe to in range event
        self._aligned = False
        def aligned(event):
            self._aligned = True
        self.qeventHub.subscribeToType(ram.ai.duct.SeekingToAligned.ALIGNED, 
                                       aligned)
        
        # Inject and event which has the duct ahead, and at the needed range
        self.injectEvent(vision.EventType.DUCT_FOUND, 
                         vision.DuctEvent, 0, 0, 0, 0, False, False,
                         x = 0.05, y = -0.1, range = 0.31, rotation = 2,
                         aligned = True, visible = False)
        self.assertEqual(0.05, self.ai.data['lastDuctEvent'].x)
        self.assertEqual(-0.1, self.ai.data['lastDuctEvent'].y)
        self.assertEqual(0.31, self.ai.data['lastDuctEvent'].range)
        self.assertEqual(2, self.ai.data['lastDuctEvent'].rotation)
        self.assertEqual(True, self.ai.data['lastDuctEvent'].aligned)
        self.assertEqual(False, self.ai.data['lastDuctEvent'].visible)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assert_(self._aligned)
        self.assertCurrentState(ram.ai.duct.Aligning)
        
class TestAligning(AlignmentTest, support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(ram.ai.duct.Aligning)
        
    def testStart(self):
        AlignmentTest.testStart(self)
        self.releaseTimer(ram.ai.duct.Aligning.SETTLED)
        self.assertCurrentState(ram.ai.duct.Through)
        
    def testSettle(self):
        self.injectEvent(ram.ai.duct.Aligning.SETTLED)
        self.assertCurrentState(ram.ai.duct.Through)

class Through(support.AITestCase):       
    def testStart(self):
        # Turn this on, so we make sure it goes off
        self.visionSystem.ductDetector = True
        
        # Make sure we start driving forward
        self.machine.start(ram.ai.duct.Through)
        self.assert_(self.controller.speed > 0)
        self.assertEqual(False, self.visionSystem.ductDetector)

        # Subscribe to end events
        self._throughDuct = False
        def throughDuct(event):
            self._throughDuct = True
        self.qeventHub.subscribeToType(ram.ai.duct.Through.FORWARD_DONE, 
                                       throughDuct)
        
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(ram.ai.duct.THROUGH_DUCT, complete)
        
        # Now make sure we stop
        self.releaseTimer(ram.ai.duct.Through.FORWARD_DONE)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._throughDuct)
        self.assert_(self._complete)
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)
