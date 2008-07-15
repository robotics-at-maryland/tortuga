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
        
# Helper functions

def binFoundHelper(self):
    # Set our expected ID
    self.ai.data['currentBinID'] = 6
    
    # Test improper bin
    self.assertEqual(0, self.controller.speed)
    self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 0, 0,
                     x = 0.5, y = -0.5, id = 3)
    self.assertEqual(0, self.controller.speed)
    
    # Test proper bin
    self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 0, 0,
                     x = 0.5, y = -0.5, id = 6)
        
    self.assertLessThan(self.controller.speed, 0)
    self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
    self.assertEqual(self.controller.yawChange, 0)
    
def binTrackingHelper(self):
    """
    Tests that state proper tracks with BIN_FOUND and BIN_DROPPED
    """
        
    # Add some and test
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 6)
    self.assertAIDataValue('currentBins', set([6]))
    
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                            vision.BinEvent, 0, 0, x = 0.3, y = -0.5, id = 2)
    self.assertAIDataValue('currentBins', set([2,6]))
    
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                            vision.BinEvent, 0, 0, x = 0.2, y = -0.3, id = 3)
    self.assertAIDataValue('currentBins', set([2, 3, 6]))
    
    # Check some bin data
    self.assert_(self.ai.data.has_key('binData'))
    binData = self.ai.data['binData']
    self.assertEqual(0.5, binData[6].x)
    self.assertEqual(0.3, binData[2].x)
    self.assertEqual(0.2, binData[3].x)
    
    # Make sure its updated
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                            vision.BinEvent, 0, 0, x = 0.7, y = -0.5, id = 2)
    self.assertEqual(0.7, binData[2].x)
        
    # Remove some
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 3)
    self.assertAIDataValue('currentBins', set([2, 6]))
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 6)
    self.assertAIDataValue('currentBins', set([2]))
    
    # Add one
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 8)
    self.assertAIDataValue('currentBins', set([2,8]))
    
    # Remove all
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 2)
    self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                            vision.BinEvent, 0, 0, x = 0.5, y = -0.5, id = 8)
    self.assertAIDataValue('currentBins', set())
   
class TestTracking(aisupport.AITestCase):
    def testEnsureBinTracking(self):
        bin.ensureBinTracking(self.qeventHub, self.ai)
        self.assertAIDataValue('binTrackingEnabled', True)
    
    def testBinFoundDropped(self):
        bin.ensureBinTracking(self.qeventHub, self.ai)
        binTrackingHelper(self)
    
class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testBinFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 0, 0,
                         id = 5)
        self.assertCurrentState(bin.Seeking)
        
        # Make sure we record the current bin proper, and add it to our set
        self.assertAIDataValue('currentBinID', 5)
        self.assertAIDataValue('binTrackingEnabled', True)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.binDetector)
    
    def testBinTracking(self):
        binTrackingHelper(self)
    
class TestSeeking(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(bin.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.common.Hover)
        
    def testBinFound(self):
        """Make sure new found events move the vehicle"""
        binFoundHelper(self)

    def testBinTracking(self):
        binTrackingHelper(self)
    
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
    
    def testBinTracking(self):
        binTrackingHelper(self)
    
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
        
    def testBinTracking(self):
        binTrackingHelper(self)
        
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
        
    def testBinTracking(self):
        binTrackingHelper(self)
        
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
        
    def testBinTracking(self):
        binTrackingHelper(self)
        
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
