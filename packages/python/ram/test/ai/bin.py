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
import ext.math as math

import ram.motion as motion
import ram.motion.common
import ram.motion.pipe

import ram.test.ai.support as aisupport
from ram.test import Mock
        
# Helper functions

class BinTestCase(aisupport.AITestCase):
    def injectBinFound(self, **kwargs):
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Suit.UNKNOWN, math.Degree(0), **kwargs)
                             
    def binFoundHelper(self, shouldRotate = True):
        # Set our expected ID
        self.ai.data['currentBinID'] = 6
        
        # Test improper bin
        self.assertEqual(0, self.controller.speed)
        self.injectBinFound(x = 0.5, y = -0.5, id = 3, angle = math.Degree(0))
        self.assertEqual(0, self.controller.speed)
        
        # Test proper bin
        self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(0))
            
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
        
        # Test with some rotation
        self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(15))
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        if shouldRotate:
            self.assertGreaterThan(self.controller.yawChange, 0)
            currentYawChange = self.controller.yawChange
    
    
            # Test large rotational filtering
            self.injectBinFound(    x = 0.5, y = -0.5, id = 6, angle = math.Degree(-80))
            self.assertAlmostEqual(currentYawChange, self.controller.yawChange, 
                                   3)
            
            # Test back with a normal angle
            self.injectBinFound(    x = 0.5, y = -0.5, id = 6, angle = math.Degree(30))
            self.assertGreaterThan(self.controller.yawChange, currentYawChange)
            
            
        else:
            self.assertEqual(self.controller.yawChange, 0)
            
    def publishQueuedBinFound(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                                vision.BinEvent,
                                0, 0, vision.Suit.UNKNOWN, math.Degree(0),
                                **kwargs)
    def publishQueuedBinDropped(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                                vision.BinEvent,
                                0, 0, vision.Suit.UNKNOWN, math.Degree(0),
                                **kwargs)
            
    def binTrackingHelper(self):
        """
        Tests that state proper tracks with BIN_FOUND and BIN_DROPPED
        """
            
        # Add some and test
        self.publishQueuedBinFound(x = 0.5, y = -0.5, id = 6,
                                   angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([6]))
        
        self.publishQueuedBinFound(x = 0.3, y = -0.5, id = 2,
                                   angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([2,6]))
        
        self.publishQueuedBinFound(x = 0.2, y = -0.3, id = 3,
                                   angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([2, 3, 6]))
        
        # Check some bin data
        self.assert_(self.ai.data.has_key('binData'))
        binData = self.ai.data['binData']
        self.assertEqual(0.5, binData[6].x)
        self.assertEqual(0.3, binData[2].x)
        self.assertEqual(0.2, binData[3].x)
        
        # Make sure its updated
        self.publishQueuedBinFound(x = 0.7, y = -0.5, id = 2,
                                angle = math.Degree(0))
        self.assertEqual(0.7, binData[2].x)
            
        # Remove some
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 3,
                                angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([2, 6]))
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 6,
                                angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([2]))
        
        # Add one
        self.publishQueuedBinFound(x = 0.5, y = -0.5, id = 8,
                                angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set([2,8]))
        
        # Remove all
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 2,
                                angle = math.Degree(0))
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 8,
                                angle = math.Degree(0))
        self.assertAIDataValue('currentBins', set())
   
class TestTracking(BinTestCase):
    def testEnsureBinTracking(self):
        bin.ensureBinTracking(self.qeventHub, self.ai)
        self.assertAIDataValue('binTrackingEnabled', True)
    
    def testBinFoundDropped(self):
        bin.ensureBinTracking(self.qeventHub, self.ai)
        self.binTrackingHelper()
    
class TestSearching(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.controller.depth = 5
        self.machine.start(bin.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('preBinCruiseDepth', self.controller.depth)
                
    def testBinFound(self):
        # Now change states
        self.injectBinFound(id = 5)
        self.assertCurrentState(bin.Seeking)
        
        # Make sure we record the current bin proper, and add it to our set
        self.assertAIDataValue('currentBinID', 5)
        self.assertAIDataValue('binTrackingEnabled', True)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.binDetector)
    
    def testBinTracking(self):
        self.binTrackingHelper()
    
class TestSeeking(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(bin.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testBinFound(self):
        """Make sure new found events move the vehicle"""
        self.binFoundHelper(False)

    def testBinTracking(self):
        self.binTrackingHelper()
    
    def testBinLost(self):
        """Make sure losing the light goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BIN_LOST)
        self.assertCurrentState(bin.Recover)   
        
class TestRecover(aisupport.AITestCase):
    def testStart(self):
        self.vehicle.depth = 9
	self.controller.depth = 9
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(bin.Recover)
        
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertLessThan(self.controller.depth, 9)

        # Make sure timer works
        self.releaseTimer(bin.Recover.TIMEOUT)
        # Its dive, and not SeekEnd because we are already at the end
        self.assertCurrentState(bin.Searching)
        
    def testFound(self):
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(bin.Recover)
        
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Suit.UNKNOWN, math.Degree(0))
        self.assertCurrentState(bin.Seeking)


class TestCentering(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(bin.Centering)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(bin.Centering)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        # Setup for SeekEnd
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        
        # Make sure timer works
        self.releaseTimer(bin.Centering.SETTLED)
        # Its dive, and not SeekEnd because we are already at the end
        self.assertCurrentState(bin.Dive)
        
    def testBinLost(self):
        """Make sure we search when we lose the bin"""
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BIN_LOST)
        self.assertCurrentState(bin.Recover)
    
    def testBinTracking(self):
        self.binTrackingHelper()
    
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper(False)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        # Setup for SeekEnd
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        # Inject settled event
        self.injectEvent(bin.Centering.SETTLED)
        self.assertCurrentState(bin.SeekEnd)
        
class TestAligning(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(bin.Aligning)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(bin.Aligning)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        # Setup for SeekEnd
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        
        # Make sure timer works
        self.releaseTimer(bin.Aligning.ALIGNED)
        self.assertCurrentState(bin.Examine)
        
    def testBinLost(self):
        """Make sure we search when we lose the bin"""
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BIN_LOST)
        self.assertCurrentState(bin.Recover)
    
    def testBinTracking(self):
        self.binTrackingHelper()
    
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper()
    
    def testSettled(self):
        """Make sure we move on after settling"""
        # Setup for SeekEnd
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        # Inject settled event
        self.injectEvent(bin.Aligning.ALIGNED)
        self.assertCurrentState(bin.Examine)
        
class TestSeekEnd(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        
        bin.ensureBinTracking(self.qeventHub, self.ai)
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3,4])
        self.ai.data['binData'] = {4 : Mock(x = -1), 3 : Mock(x = 0)}
        self.machine.start(bin.SeekEnd)
        
        self._centered = False
        self._atEnd = False
        self.qeventHub.subscribeToType(bin.SeekEnd.CENTERED_, self._centeredH)
        self.qeventHub.subscribeToType(bin.SeekEnd.AT_END, self._atEndH)
        
    def _centeredH(self, event):
        self._centered = True
    def _atEndH(self, event):
        self._atEnd = True
        
    def testBinFound(self):
        """Make sure the loop back works"""
        self.ai.data['currentBinID'] = 0
        self.ai.data['currentBins'] = set([6])
        
        # Make sure we repond to bin offset properly, but ignore orientation
        self.binFoundHelper(False)
        
    def testBinFoundCentered(self):
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        
        # Now test centered
        self._centered = False
        
        # Test wrong ID
        self.injectBinFound(x = 0, y = 0, id = 4)
        self.assertFalse(self._centered)
        
        # Proper centered (6 is proper ID changed in binFoundHelper)
        self.injectBinFound(x = 0, y = 0, id = 3)
        self.qeventHub.publishEvents()
        self.assert_(self._centered)
        
    def testBinTracking(self):
        self.ai.data['currentBinID'] = 0
        self.ai.data['currentBins'] = set()
        self.binTrackingHelper()
        
    def testCentered(self):
        self.qeventHub.publishEvents()
        self._centered = False
        self._atEnd = False
        self.assertCurrentState(bin.SeekEnd)
        
        # We are in the center
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([2,3,4])
        self.ai.data['binData'] = {2 : Mock(x = -1), 3 : Mock(x = 0),
                                   4 : Mock(x = 1)}
        
        self.assertFalse(self._centered)
        self.assertFalse(self._atEnd)
        
        self.injectEvent(bin.SeekEnd.CENTERED_)
        self.qeventHub.publishEvents()
        
        self.assertFalse(self._centered)
        self.assertFalse(self._atEnd)
        self.assertEqual(2, self.ai.data['currentBinID'])
        self.assertCurrentState(bin.SeekEnd)
        
        # Now we are on the left, make sure another center, gets us the right
        # result
        self.injectEvent(bin.SeekEnd.CENTERED_)
        self.qeventHub.publishEvents()
        
        self.assertFalse(self._centered)
        self.assert_(self._atEnd)
        self.assertEqual(2, self.ai.data['currentBinID'])
        
    def testCenteredNoBins(self):
        # Try no more bins
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set()
        self.assertCurrentState(bin.SeekEnd)
        self.injectEvent(bin.SeekEnd.CENTERED_)
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.qeventHub.publishEvents()
        self.assertCurrentState(bin.Recover)
        
    def testAtEnd(self):
        self.injectEvent(bin.SeekEnd.AT_END)
        self.assertCurrentState(bin.Dive)
        

class TestDive(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(bin.Dive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth, motion.pipe.Hover))
        #self.assertGreaterThan(self.controller.depth, 0)
                
    def testBinFound(self):
        """Make sure the loop back works"""
        # Need to add multi-motion support
        self.binFoundHelper(False)
        
    def testBinTracking(self):
        self.binTrackingHelper()
        
    def testDiveFinished(self):
        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(bin.Aligning)
        
class TestExamine(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self._targetFound = False
        self.qeventHub.subscribeToType(bin.Examine.FOUND_TARGET, 
                                       self.targetFound)
        self.machine.start(bin.Examine)
        
    def targetFound(self, event):
        self._targetFound = True
        
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.pipe.Hover)
        
        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.releaseTimer(bin.Examine.MOVE_ON)
        self.assertCurrentState(bin.SurfaceToMove)

    def testLoadSuitConfig(self):
        expectedSuits = set([vision.Suit.CLUB, vision.Suit.DIAMOND])
        self.assertEqual(expectedSuits, 
                         self.machine.currentState()._targetSuits)

    def testBinFound(self):
        """Make sure the loop back works"""
        # Need to add multi-motion support
        self.binFoundHelper()
        
        # Test counting variables
        
    def assertSuitCount(self, heart = 0, spade = 0, club = 0, diamond = 0):
        s = self.machine.currentState()
        self.assertEqual(s._hearts, heart)
        self.assertEqual(s._spades, spade)
        self.assertEqual(s._clubs, club)
        self.assertEqual(s._diamonds, diamond)
        
    def testSuitCount(self):
        """
        Make sure the right event if found after we get the right number
        of events
        """
        self.ai.data['currentBinID'] = 3
        
        # Test blank one
        self.injectBinFound(id = 4, suit = vision.Suit.HEART)
        self.assertSuitCount()
        
        # Add some ones to populate
        self.injectBinFound(id = 3, suit = vision.Suit.HEART)
        self.injectBinFound(id = 3, suit = vision.Suit.CLUB)
        self.injectBinFound(id = 3, suit = vision.Suit.SPADE)
        self.injectBinFound(id = 3, suit = vision.Suit.HEART)
        self.injectBinFound(id = 3, suit = vision.Suit.DIAMOND)
        self.injectBinFound(id = 3, suit = vision.Suit.CLUB)
        self.injectBinFound(id = 3, suit = vision.Suit.DIAMOND)
        self.injectBinFound(id = 3, suit = vision.Suit.CLUB)
        self.injectBinFound(id = 3, suit = vision.Suit.HEART)
        self.injectBinFound(id = 3, suit = vision.Suit.CLUB)

        self.assertSuitCount(heart = 3, spade = 1, club = 4, diamond = 2)
        
        # No try for target found
        self.assertFalse(self._targetFound)
        
        # Fire the last club and make sure we got it called
        self.injectBinFound(id = 3, suit = vision.Suit.CLUB)
        self.qeventHub.publishEvents()
        self.assert_(self._targetFound)
        
    def testBinTracking(self):
        self.binTrackingHelper()
        
class TestSurfaceToMove(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.vehicle.depth = 10
        self.ai.data['preBinCruiseDepth'] = 5.0
        self.machine.start(bin.SurfaceToMove)
    
    def testStart(self):
        """Make sure we start surfacing and are still hovering"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth, motion.pipe.Hover))
        
        #self.assertLessThan(self.controller.depth, 10)
        
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper(False)
        
    def testBinTracking(self):
        self.binTrackingHelper()
        
    def testDiveFinished(self):        
        # Make sure we go the right place
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(bin.NextBin)

class TestNextBin(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        bin.ensureBinTracking(self.qeventHub, self.ai)
        
        self.ai.data['currentBinID'] = 4
        self.ai.data['currentBins'] = set([3,4])
        self.ai.data['binData'] = {4 : Mock(x = -1), 3 : Mock(x = 0)}
        self.machine.start(bin.NextBin)
        self.qeventHub.publishEvents()
        assert type(self.machine.currentState()) == bin.NextBin
        
        self._centered = False
        self._atEnd = False
        self.qeventHub.subscribeToType(bin.NextBin.CENTERED_, self._centeredH)
        self.qeventHub.subscribeToType(bin.NextBin.AT_END, self._atEndH)
        
    def _centeredH(self, event):
        self._centered = True
    def _atEndH(self, event):
        self._atEnd = True
        
    def testEndStart(self):
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3,4])
        self.ai.data['binData'] = {4 : Mock(x = -1), 3 : Mock(x = 0)}
        self.machine.start(bin.NextBin)
        self.qeventHub.publishEvents()
        
        self.assert_(self._atEnd)
        self.assertFalse(self._centered)
        
    def testStartNotEnd(self):
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        
        self.ai.data['currentBinID'] = 6
        self.ai.data['currentBins'] = set([6,3,5,4])
        self.ai.data['binData'] = {6 : Mock(x = -1), 3 : Mock(x = 0),
            5 : Mock(x = 1), 4 : Mock(x = 2)}
        self.machine.start(bin.NextBin)
        self.qeventHub.publishEvents()
                
        self.assertEqual(3, self.ai.data['currentBinID'])
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        self.assertCurrentState(bin.NextBin)
        
    def testMissingCurrent(self):
        # Make sure when lose the current we do something smart
        self.ai.data['currentBinID'] = 8
        self.ai.data['currentBins'] = set([6,3,5,4])
        self.ai.data['binData'] = {6 : Mock(x = -1), 3 : Mock(x = 0),
            5 : Mock(x = 1), 4 : Mock(x = 2)}
        self.machine.start(bin.NextBin)
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        self.qeventHub.publishEvents()
                
        self.assertCurrentState(bin.Recover)
#        self.assertEqual(3, self.ai.data['currentBinID'])
#        self.assertFalse(self._atEnd)
#        self.assertFalse(self._centered)
#        self.assertCurrentState(bin.NextBin)

    def testBinFound(self):
        """Make sure the loop back works"""
        self.ai.data['currentBinID'] = 0
        self.ai.data['currentBins'] = set([6])
        
        self.binFoundHelper(False)
        
    def testBinFoundCentered(self):
        self.ai.data['currentBinID'] = 3
        self.ai.data['currentBins'] = set([3])
        
        # Now test centered
        self._centered = False
        
        # Test wrong ID
        self.injectBinFound(x = 0, y = 0, id = 4, angle = math.Degree(0))
        self.assertFalse(self._centered)
        
        # Proper centered (6 is proper ID changed in binFoundHelper)
        self.injectBinFound(x = 0, y = 0, id = 3, angle = math.Degree(0))
        self.qeventHub.publishEvents()
        self.assert_(self._centered)
        
    def testBinTracking(self):
        self.ai.data['currentBinID'] = 0
        self.ai.data['currentBins'] = set()
        self.binTrackingHelper()
        
    def testCentered(self):
        self.qeventHub.publishEvents()
        self._centered = False
        self._atEnd = False
        self.assertCurrentState(bin.NextBin)
        
        self.injectEvent(bin.NextBin.CENTERED_)
        self.assertCurrentState(bin.Dive)
        
    def testAtEnd(self):
        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.injectEvent(bin.NextBin.AT_END)
        self.assertCurrentState(bin.SurfaceToCruise)
        
class TestDropMarker(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(bin.DropMarker)
    
    def testStart(self):
        """Make sure we start diving, and drop a marker """
        self.assertCurrentMotion(motion.pipe.Hover)
        self.assertCurrentState(bin.DropMarker)

#        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.releaseTimer(bin.DropMarker.DROPPED)
        self.assertCurrentState(bin.SurfaceToMove)
        
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper()
        
    def testBinTracking(self):
        self.binTrackingHelper()

    def testDroppedFirst(self):
        """Make sure we move on after dropping the second marker"""

        # Needed to DROPPED transition handler
        self.ai.data['markersDropped'] = 1

        # Inject event and test the response
        self.injectEvent(bin.DropMarker.DROPPED)
        self.qeventHub.publishEvents()
        self.assertCurrentState(bin.SurfaceToMove)
        
    def testDroppedSecond(self):
        """Make sure we move on after dropping the second marker"""

        # Needed to DROPPED transition handler
        self.ai.data['markersDropped'] = 2
        # Needed for SurfaceToCruise
        self.ai.data['preBinCruiseDepth'] = 5.0 

        # Inject event and test the response
        self.injectEvent(bin.DropMarker.DROPPED)
        self.qeventHub.publishEvents()
        self.assertCurrentState(bin.SurfaceToCruise)

    def testFinished(self):
        """Ensure FINISHED -> SurfaceToCruise"""
        # Needed for SurfaceToCruise
        self.ai.data['preBinCruiseDepth'] = 5.0 

        self.injectEvent(bin.DropMarker.FINISHED)
        self.assertCurrentState(bin.SurfaceToCruise)

    def testContinue(self):
        """Ensure CONTINUE -> SurfaceToMove"""
        # Needed for SurfaceToCruise
        self.ai.data['preBinCruiseDepth'] = 5.0 

        self.injectEvent(bin.DropMarker.CONTINUE)
        self.assertCurrentState(bin.SurfaceToMove)
        
class TestSurface(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.vehicle.depth = 10
        self.ai.data['preBinCruiseDepth'] = 5.0
        self.machine.start(bin.SurfaceToCruise)
    
    def testStart(self):
        """Make sure we start surfacing and are still hovering"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth, motion.pipe.Hover))
        
        #self.assertLessThan(self.controller.depth, 10)
        
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper()
        
    def testBinTracking(self):
        self.binTrackingHelper()
        
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
