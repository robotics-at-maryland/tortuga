# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/bin.py

# Python Imports
import unittest

# Project Imports
import ram.ai.bin as bin
import ext.core as core
import ext.vision as vision

import ram.motion as motion
import ram.motion.common

import ram.test.ai.support as aisupport

class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 0, 0)
        self.assertCurrentState(bin.Seeking)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.binDetector)
        
def binFoundHelper(self):
    self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 0, 0,
                     x = 0.5, y = -0.5)
        
    self.assertLessThan(self.controller.speed, 0)
    self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
    self.assertEqual(self.controller.yawChange, 0)
        
class TestSeeking(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.common.Hover)
        
    def testBinFound(self):
        """Make sure new found events move the vehicle"""
        binFoundHelper(self)
    
    def testBinLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.BIN_LOST)
        self.assertCurrentState(bin.Searching)
        
class TestCentering(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.Centering)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(bin.Centering)
        
        self.assertCurrentMotion(motion.common.Hover)
        
        self.releaseTimer(bin.Centering.SETTLED)
        self.assertCurrentState(bin.Dive)

        
    def testBinLost(self):
        """Make sure we search when we lose the bin"""
        self.injectEvent(vision.EventType.BIN_LOST)
        self.assertCurrentState(bin.Searching)
    
    def testBinFound(self):
        """Make sure the loop back works"""
        binFoundHelper(self)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(bin.Centering.SETTLED)
        self.assertCurrentState(bin.Dive)
        
class TestDive(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(bin.Dive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.common.Hover, motion.basic.RateChangeDepth, None))
        #self.assertGreaterThan(self.controller.depth, 0)
                
    def testBinFound(self):
        """Make sure the loop back works"""
        # Need to add multi-motion support
        binFoundHelper(self)
        
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(bin.DropMarker)
        
class TestDropMarker(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.DropMarker)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.common.Hover)
        
        self.releaseTimer(bin.DropMarker.DROPPED)
        self.assertCurrentState(bin.Surface)
        
    def testBinFound(self):
        """Make sure the loop back works"""
        binFoundHelper(self)
        
    def testDropped(self):
        """Make sure we move on after settling"""
        self.injectEvent(bin.DropMarker.DROPPED)
        self.assertCurrentState(bin.Surface)
        
class TestSurface(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 10
        self.machine.start(bin.Surface)
    
    def testStart(self):
        """Make sure we start surfacing and are still hovering"""
        self.assertCurrentMotion(
            (motion.common.Hover, motion.basic.RateChangeDepth, None))
        
        #self.assertLessThan(self.controller.depth, 10)
        
    def testBinFound(self):
        """Make sure the loop back works"""
        binFoundHelper(self)
        
    def testDiveFinished(self):
        # Subscribe to end event
        self._binComplete = False
        def binComplete(event):
            self._binComplete = True
        self.qeventHub.subscribeToType(bin.COMPLETE, binComplete)
        
        # Finish the state machine
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._binComplete)
