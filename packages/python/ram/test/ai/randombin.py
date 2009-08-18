# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/bin.py

# Python Imports
import unittest

# Project Imports
import ram.ai.randombin as randombin
import ram.ai.tracking as tracking
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.common
import ram.motion.pipe

import ram.test.ai.support as aisupport
from ram.test import Mock
from ram.test.motion.support import MockTimer
        
# Helper functions

class BinTestCase(aisupport.AITestCase):
    def injectBinFound(self, **kwargs):
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), **kwargs)
    def injectBinDropped(self, **kwargs):
        self.injectEvent(vision.EventType.BIN_DROPPED, vision.BinEvent,
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), **kwargs)
    def injectMultiBinAngle(self, **kwargs):
        self.injectEvent(vision.EventType.MULTI_BIN_ANGLE, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), **kwargs)
                             
    def binFoundHelper(self, shouldRotate = True, useMultiAngle = False):
        # Set our expected ID
        self.ai.data['binData']['currentID'] = 6
        
        # Test improper bin
        self.assertEqual(0, self.controller.speed)
        self.injectBinFound(x = 0.5, y = -0.5, id = 3, angle = math.Degree(0))
        self.assertEqual(0, self.controller.speed)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
        # Test proper bin
        self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(0))
            
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
        # Test with some rotation
        self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(15))
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
        if shouldRotate:
            self.assertGreaterThan(self.controller.yawChange, 0)
            currentYawChange = self.controller.yawChange
    
    
            # Test large rotational filtering
            self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(-80))
            self.assertAlmostEqual(currentYawChange, self.controller.yawChange, 3)
            
            # Test back with a normal angle
            self.injectBinFound(x = 0.5, y = -0.5, id = 6, angle = math.Degree(30))
            self.assertGreaterThan(self.controller.yawChange, currentYawChange)
            
            
        else:
            self.assertEqual(self.controller.yawChange, 0)
           

        if useMultiAngle:
            # Reset values
            self.controller.yawChange = 0 
            self.controller.speed = 0
            self.controller.sidewaysSpeed = 0
            
            # Test Left (First multi bin event, then the found to start us moving)
            self.injectMultiBinAngle(x = 0.5, y = -0.5, id = 6, 
                                     angle = math.Degree(15))
            # This found angle should be ignored
            self.injectBinFound(x = 0, y = 0, id = 6, angle = math.Degree(0))
            
            # Ensure that the x & y are ignored and we yaw properly
            self.assertAlmostEqual(0, self.controller.speed, 3)
            self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
            self.assertGreaterThan(self.controller.yawChange, 0)
            
            # Test Right
            self.injectMultiBinAngle(x = 0.5, y = -0.5, id = 6, 
                                     angle = math.Degree(-15))
            self.injectBinFound(x = 0, y = 0, id = 6, angle = math.Degree(0))
            
            # Ensure that the x & y are ignored and we yaw properly
            self.assertAlmostEqual(0, self.controller.speed, 3)
            self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
            self.assertLessThan(self.controller.yawChange, 0)

        # Drop bin id 3 and 6 since they're the ones left
        self.injectBinDropped(id = 3)
        self.injectBinDropped(id = 6)
            
    def publishQueuedBinFound(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.BIN_FOUND, 
                                vision.BinEvent,
                                0, 0, vision.Symbol.UNKNOWN, math.Degree(0),
                                **kwargs)
    def publishQueuedBinDropped(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.BIN_DROPPED, 
                                vision.BinEvent,
                                0, 0, vision.Symbol.UNKNOWN, math.Degree(0),
                                **kwargs)
            
    def binTrackingHelper(self):
        """
        Tests that state proper tracks with BIN_FOUND and BIN_DROPPED
        """
            
        # Add some and test
        self.publishQueuedBinFound(x = 0.5, y = -0.5, id = 6,
                                   angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([6]))
        
        self.publishQueuedBinFound(x = 0.3, y = -0.5, id = 2,
                                   angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([2,6]))
        
        self.publishQueuedBinFound(x = 0.2, y = -0.3, id = 3,
                                   angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([2, 3, 6]))
        
        # Check some bin data
        self.assert_(self.ai.data['binData'].has_key('itemData'))
        binData = self.ai.data['binData']['itemData']
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
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([2, 6]))
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 6,
                                angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([2]))
        
        # Add one
        self.publishQueuedBinFound(x = 0.5, y = -0.5, id = 8,
                                angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set([2,8]))
        
        # Remove all
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 2,
                                angle = math.Degree(0))
        self.publishQueuedBinDropped(x = 0.5, y = -0.5, id = 8,
                                angle = math.Degree(0))
        self.assertDataValue(self.ai.data['binData'], 'currentIds', set())
   
class TestStart(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(randombin.Start)
    
    def testStart(self):
        """Make sure we are diving with no detector on"""
        self.assertFalse(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAIDataValue('binStartOrientation', 0)
        
    def testFinish(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(randombin.Searching)
   
class TestTracking(BinTestCase):
    def testEnsureBinTracking(self):
        randombin.ensureBinTracking(self.qeventHub, self.ai)
        self.assertDataValue(self.ai.data['binData'], 'trackingEnabled', True)
    
    def testBinFoundDropped(self):
        randombin.ensureBinTracking(self.qeventHub, self.ai)
        self.binTrackingHelper()
    
class TestSearching(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.controller.depth = 5
        self.machine.start(randombin.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertAIDataValue('preBinCruiseDepth', self.controller.depth)
        self.assertAIDataValue('binStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()
        self.ai.data['firstSearching'] = True

        # Now set the initial direction to something other than 0
        self.ai.data['binStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(randombin.Searching)
        self.assert_(self.visionSystem.binDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Finish that motion and continue to the ForwardZigZag
        self.machine.currentState()._forwardMotion._finish()

        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('binStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)
                
    def testBinFound(self):
        # Now change states
        self.injectBinFound(id = 5)
        self.assertCurrentState(randombin.Seeking)
        
        # Make sure we record the current bin proper, and add it to our set
        self.assertDataValue(self.ai.data['binData'], 'currentID', 5)
        self.assertDataValue(self.ai.data['binData'], 'trackingEnabled', True)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.binDetector)
    
    def testBinTracking(self):
        self.binTrackingHelper()

    def testMultiMotion(self):
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Now finish the motion and make sure it enters the next one
        self.machine.currentState()._forwardMotion._finish()

        self.assertCurrentMotion(motion.search.ForwardZigZag)

    def testReenter(self):
        # Restart the state with firstSearching set to False
        self.machine.stop()
        self.ai.data['firstSearching'] = False

        # Make sure it skips the TimedMoveDirection
        self.machine.start(randombin.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

class TestAlternateSearching(BinTestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'Bin' : {
                        'forwardDuration' : 0
                    },
                }
            }
        }
        BinTestCase.setUp(self, cfg = cfg)
        self.machine.start(randombin.Searching)

    def testStart(self):
        self.assertCurrentState(randombin.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

class TestSeeking(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(randombin.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testBinFound(self):
        """Make sure new found events move the vehicle"""
        # We need to inject a closer event of id 6 for binFoundHelper
        self.publishQueuedBinFound(x = 0, y = 0, id = 6)
        self.binFoundHelper(False)

    def testBinTracking(self):
        self.binTrackingHelper()
    
    def _centered(self, event):
        self._center = True
        
    def testCentered(self):
        # Set up the binData
        self.ai.data['binData']['currentID'] = 0
        self.ai.data['binData']['currentIds'] = set([0])
        self.ai.data['binData']['itemData'] = {0 : Mock(x = -1, y = -1)}
        
        # Subscribe to the centered event
        self._center = False
        self.qeventHub.subscribeToType(randombin.Seeking.BIN_CENTERED, self._centered)
        
        # Send in a bin event that should be uncentered
        self.publishQueuedBinFound(x = 1, y = 1, id = 1)
        
        # Check to make sure you aren't centered
        self.assertFalse(self._center)
        
        # Send in a bin event that should be centered
        self.publishQueuedBinFound(x = 0, y = 0, id = 2)
        self.assertTrue(self._center)
    
    def testBinLost(self):
        """Make sure losing the bin goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverSeeking)
        
class TestRecover(aisupport.AITestCase):
    class MockRecover(randombin.Recover):
        @staticmethod
        def transitions():
            return randombin.Recover.transitions(TestRecover.MockRecover, TestRecover.MockFoundState)
        
    class MockFoundState(state.State):
        @staticmethod
        def transitions():
            return {"NO" : "TRANSITION"}
    
    def testStart(self):
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(TestRecover.MockRecover)
        
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(randombin.Start)
        
    def testFound(self):
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(TestRecover.MockRecover)
        
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0))
        self.releaseTimer(randombin.Recover.RETURN)
        self.assertCurrentState(TestRecover.MockFoundState)
        
class TestLostCurrentBin(aisupport.AITestCase):
    class MockRecover(randombin.Recover):
        @staticmethod
        def transitions():
            return randombin.Recover.transitions(TestLostCurrentBin.MockRecover,
                                           TestLostCurrentBin.MockFoundState)
        
    class MockLostCurrentBin(randombin.LostCurrentBin):
        @staticmethod
        def transitions():
            return randombin.LostCurrentBin.transitions(
                    TestLostCurrentBin.MockLostCurrentBin,
                    TestLostCurrentBin.MockRecover,
                    TestLostCurrentBin.MockFoundState)
        
    class MockFoundState(state.State):
        @staticmethod
        def transitions():
            return {"NO" : "TRANSITION"}
        
    def setUp(self, myState = None, recover = None, foundState = None):
        aisupport.AITestCase.setUp(self)

        if myState is None:
            self.myState = TestLostCurrentBin.MockLostCurrentBin
        else:
            self.myState = myState

        if recover is None:
            self.recover = TestLostCurrentBin.MockRecover
        else:
            self.recover = recover

        if foundState is None:
            self.foundState = TestLostCurrentBin.MockFoundState
        else:
            self.foundState = foundState
        
        # Data. currentID must be something that it won't accidently create.
        self.ai.data['lastBinX'] = 0.25
        self.ai.data['lastBinY'] = -0.25
        self.ai.data['startSide'] = randombin.BinSortingState.RIGHT
        
        self.machine.start(self.myState)
        
        self.ai.data['binData']['currentID'] = 10
        self.ai.data['binData']['currentIds'] = set([3,4])
        self.ai.data['binData']['itemData'] = {4 : Mock(x = -1, y = 1, id = 4),
                                               3 : Mock(x = 0, y = 0, id = 3)}
        
    def testStart(self):
        # Inject an old event
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), id = 4)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.myState)
        self.assertDataValue(self.ai.data['binData'], 'currentID', 10)
        
        # Now a new one outside the threshold
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), id = 5)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.myState)
        self.assertDataValue(self.ai.data['binData'], 'currentID', 10)
        
        # Now the correct bin
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0), x = 0.30,
                         y = -0.30, id = 6)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.foundState)
        self.assertDataValue(self.ai.data['binData'], 'currentID', 6)

    def testBinsLost(self):
        # Inject BINS_LOST
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(self.recover)
        
    def testTimeout(self):
        self.ai.data['startSide'] = randombin.BinSortingState.RIGHT
        
        self.assertCurrentState(self.myState)
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(self.foundState)
        self.assertDataValue(self.ai.data['binData'], 'currentID', 3)

class TestLostCurrentBinDive(TestLostCurrentBin):
    def setUp(self):
        TestLostCurrentBin.setUp(self, myState = randombin.LostCurrentBinDive,
                                 recover = randombin.RecoverDive,
                                 foundState = randombin.Dive)

class TestLostCurrentBinAligning(TestLostCurrentBin):
    def setUp(self):
        TestLostCurrentBin.setUp(self, myState = randombin.LostCurrentBinAligning,
                                 recover = randombin.RecoverDive,
                                 foundState = randombin.Aligning)

class TestLostCurrentBinCloserLook(TestLostCurrentBin):
    def setUp(self):
        TestLostCurrentBin.setUp(self, myState = randombin.LostCurrentBinCloserLook,
                                 recover = randombin.RecoverCloserLook,
                                 foundState = randombin.CloserLook)

class TestLostCurrentBinSurfaceToMove(TestLostCurrentBin):
    def setUp(self):
        TestLostCurrentBin.setUp(self, myState = randombin.LostCurrentBinSurfaceToMove,
                                 recover = randombin.RecoverSurfaceToMove,
                                 foundState = randombin.SurfaceToMove)

class TestLostCurrentBinNextBin(TestLostCurrentBin):
    def setUp(self):
        TestLostCurrentBin.setUp(self, myState = randombin.LostCurrentBinNextBin,
                                 recover = randombin.RecoverNextBin,
                                 foundState = randombin.Dive)

class TestRecoverSeeking(aisupport.AITestCase):
    def testStart(self):
        self.vehicle.depth = 9
        self.controller.depth = 9
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(randombin.RecoverSeeking)
        
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        #self.assertLessThan(self.controller.depth, 9)

        # Make sure timer works
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(randombin.Start)
        
    def testFound(self):
        self.ai.data["lastBinX"] = 0.5
        self.ai.data["lastBinY"] = -0.5
        
        self.machine.start(randombin.RecoverSeeking)
        
        self.injectEvent(vision.EventType.BIN_FOUND, vision.BinEvent, 
                         0, 0, vision.Symbol.UNKNOWN, math.Degree(0))
        self.releaseTimer(randombin.Recover.RETURN)
        self.assertCurrentState(randombin.Seeking)


class TestCentering(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(randombin.Centering)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(randombin.Centering)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        # Make sure timer works
        self.releaseTimer(randombin.Centering.SETTLED)
        self.assertCurrentState(randombin.Dive)
        
    def testBinLost(self):
        """Make sure we search when we lose the bin"""
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverCentering)
    
    def testBinTracking(self):
        self.binTrackingHelper()
    
    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper(False, useMultiAngle = True)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        # Inject settled event
        self.injectEvent(randombin.Centering.SETTLED)
        self.assertCurrentState(randombin.Dive)

    def testStoreDesiredQuaternion(self):
        # Setup a desired orientation                                       
        expected = math.Quaternion(math.Degree(45), math.Vector3.UNIT_Z)
        self.controller.desiredOrientation = expected

        # Send SETTLED event
        self.injectEvent(randombin.Centering.SETTLED)

        # Make sure we have the desired quaterion saved properly             
        self.assertAIDataValue('binArrayOrientation', expected)
        self.assertEqual(1, self.controller.headingHolds)
        
class TestAligning(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.machine.start(randombin.Aligning)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(randombin.Aligning)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        # Make sure timer works
        self.releaseTimer(randombin.Aligning.ALIGNED)
        self.assertCurrentState(randombin.CloserLook)
        
    def testBinLost(self):
        """Make sure we search when we lose the bin"""
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverDive)
    
    def testBinTracking(self):
        self.binTrackingHelper()
    
    def testBinFound(self, shouldRotate = True):
        """Make sure the loop back works"""
        self.binFoundHelper(shouldRotate = shouldRotate)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        # Inject settled event
        self.injectEvent(randombin.Aligning.ALIGNED)
        self.assertCurrentState(randombin.CloserLook)

class TestAlternateAligning(TestAligning):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'Bin' : {
                        'adjustAngle' : False,
                    },
                },
            },
        }
        BinTestCase.setUp(self, cfg = cfg)
        self.machine.start(randombin.Aligning)

    def testBinFound(self):
        """Make sure the loop back works"""
        self.binFoundHelper(shouldRotate = False)

class DiveTestCase(object):
    """
    Holds common test dive cases
    """
    
    def setUp(self, myState, nextState):
        self.myState = myState
        self.nextState = nextState
        self.vehicle.depth = 0
        self.machine.start(self.myState)

    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth, motion.pipe.Hover))
        #self.assertGreaterThan(self.controller.depth, 0)

    def testBinArrayOrientation(self):
        # Setup data for turn hold
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['binArrayOrientation'] = expected

        # Restart the state machine
        self.machine.stop()
        self.machine.start(self.myState)

        # Make sure we have the proper orientation
        self.assertEqual(expected, self.controller.desiredOrientation)
                
    def testBinFound(self):
        """Make sure the loop back works"""
        # Need to add multi-motion support
        self.binFoundHelper(False, useMultiAngle = False)
        
    def testBinTracking(self):
        self.binTrackingHelper()
        
    def testDiveFinished(self):
        #self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(self.nextState)

class TestDive(DiveTestCase, BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        DiveTestCase.setUp(self, myState = randombin.Dive, nextState = randombin.Aligning)
        
    def testBinLost(self):
        """Make sure losing the bin goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverDive)
        self.assertAIDataValue('dive_offsetTheOffset', 0.25)
        
    def testBinLostTooMuch(self):
        """Make sure losing the too many times stops the search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        # For RecoverDive
        self.ai.data['dive_offsetTheOffset'] = 1
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.qeventHub.publishEvents()
        self.assertCurrentState(randombin.SurfaceToMove)
        self.assertAIDataValue('dive_offsetTheOffset', 1.25)

class TestCloserLook(DiveTestCase, BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        DiveTestCase.setUp(self, myState = randombin.CloserLook, 
                           nextState = randombin.DropMarker)
        
    def testBinLost(self):
        """Make sure losing the bin goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverCloserLook)
        self.assertAIDataValue('closerlook_offsetTheOffset', 0.25)
        
    def testBinLostTooMuch(self):
        """Make sure losing the too many times stops the search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        # For RecoverDive
        self.ai.data['closerlook_offsetTheOffset'] = 1
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.qeventHub.publishEvents()
        self.assertCurrentState(randombin.SurfaceToMove)
        self.assertAIDataValue('closerlook_offsetTheOffset', 1.25)
                
class TestSurfaceToMove(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.vehicle.depth = 10
        # Set the start side to the left
        self.ai.data['startSide'] = randombin.BinSortingState.LEFT
        self.ai.data['preBinCruiseDepth'] = 5.0
        self.machine.start(randombin.SurfaceToMove)
    
    def testStart(self):
        """Make sure we start surfacing and are still hovering"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth, motion.pipe.Hover))
        
        #self.assertLessThan(self.controller.depth, 10)
        
    def testDiveFinished(self):        
        # Make sure we go the right place
        self.ai.data['binData']['currentID'] = 3
        self.ai.data['binData']['currentIds'] = set([3])
        
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(randombin.NextBin)
        
    def testBinLost(self):
        """Make sure losing the bin goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverSurfaceToMove)

class TestNextBin(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        # Set the startSide to the left so it knows that the routine started
        # on the left side
        self.ai.data['startSide'] = randombin.BinSortingState.LEFT
        self.ai.data['binData'] = dict()
        randombin.ensureBinTracking(self.qeventHub, self.ai)
        
        self.ai.data['binData']['currentID'] = 4
        self.ai.data['binData']['currentIds'] = set([3,4])
        self.ai.data['binData']['itemData'] = {4 : Mock(x = -1), 3 : Mock(x = 0)}
        self.machine.start(randombin.NextBin)
        self.qeventHub.publishEvents()
        assert type(self.machine.currentState()) == randombin.NextBin
        
        self._centered = False
        self._atEnd = False
        self.qeventHub.subscribeToType(randombin.NextBin.CENTERED_, self._centeredH)
        self.qeventHub.subscribeToType(randombin.NextBin.AT_END, self._atEndH)
        
    def _centeredH(self, event):
        self._centered = True
    def _atEndH(self, event):
        self._atEnd = True
        
    def testEndStart(self):
        self.ai.data['preBinCruiseDepth'] = 5.0

        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        self.ai.data['binData']['currentID'] = 3
        self.ai.data['binData']['currentIds'] = set([3,4])
        self.ai.data['binData']['itemData'] = {4 : Mock(x = -1), 3 : Mock(x = 0)}
        self.machine.start(randombin.NextBin)
        self.qeventHub.publishEvents()
        
        self.assert_(self._atEnd)
        self.assertFalse(self._centered)
        
        self.assertCurrentState(randombin.SurfaceToCruise)
        
    def testStartNotEnd(self):
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        
        self.ai.data['binData']['currentID'] = 6
        self.ai.data['binData']['currentIds'] = set([6,3,5,4])
        self.ai.data['binData']['itemData'] = {6 : Mock(x = -1), 3 : Mock(x = 0),
            5 : Mock(x = 1), 4 : Mock(x = 2)}
        self.machine.start(randombin.NextBin)
        self.qeventHub.publishEvents()
                
        self.assertEqual(3, self.ai.data['binData']['currentID'])
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        self.assertCurrentState(randombin.NextBin)
        
    def testMissingCurrent(self):
        # Make sure when lose the current we do something smart
        self.ai.data['binData']['currentID'] = 8
        self.ai.data['binData']['currentIds'] = set([6,3,5,4])
        self.ai.data['binData']['itemData'] = {6 : Mock(x = -1), 3 : Mock(x = 0),
            5 : Mock(x = 1), 4 : Mock(x = 2)}
        self.machine.start(randombin.NextBin)
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        self.qeventHub.publishEvents()
                
        self.assertCurrentState(randombin.NextBin)
#        self.assertEqual(3, self.ai.data['binData']['currentID'])
#        self.assertFalse(self._atEnd)
#        self.assertFalse(self._centered)
#        self.assertCurrentState(randombin.NextBin)

    def testBinFound(self):
        """Make sure the loop back works"""
        self.ai.data['binData']['currentID'] = 0
        self.ai.data['binData']['currentIds'] = set([6])
        
        self.binFoundHelper(False, useMultiAngle = True)
        
    def testBinFoundCentered(self):
        self.ai.data['binData']['currentID'] = 3
        self.ai.data['binData']['currentIds'] = set([3])
        
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
        self.ai.data['binData']['currentID'] = 0
        self.ai.data['binData']['currentIds'] = set()
        self.binTrackingHelper()
        
    def testCentered(self):
        self.qeventHub.publishEvents()
        self._centered = False
        self._atEnd = False
        self.assertCurrentState(randombin.NextBin)
        
        self.injectEvent(randombin.NextBin.CENTERED_)
        self.assertCurrentState(randombin.Dive)
        
    def testAtEnd(self):
        self.ai.data['preBinCruiseDepth'] = 5.0 # Needed for SurfaceToCruise
        self.injectEvent(randombin.NextBin.AT_END)
        self.assertCurrentState(randombin.SurfaceToCruise)
        
    def testBinLost(self):
        """Make sure losing the bin goes back to search"""
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.injectEvent(vision.EventType.BINS_LOST)
        self.assertCurrentState(randombin.RecoverNextBin)
        
    def testLostFound(self):
        """
        Make sure it doesn't go into CheckDropped unless it's at the end
        after it recovers from a lost event
        """
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        # For Recover
        self.ai.data["lastBinX"] = 1
        self.ai.data["lastBinY"] = 0
        
        self.ai.data['binData']['currentID'] = 3
        self.ai.data['binData']['currentIds'] = set([2,3,4])
        self.ai.data['binData']['itemData'] = {4 : Mock(x = -1), 3 : Mock(x = 0),
                                               2 : Mock(x = 1)}
        self.machine.start(randombin.NextBin)
        self.qeventHub.publishEvents()
        
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        self.publishQueuedBinDropped(id = 4)
        self.publishQueuedBinDropped(id = 2)
        self.publishQueuedBinDropped(id = 3)
        self.injectEvent(vision.EventType.BINS_LOST)
        
        self.assertCurrentState(randombin.RecoverNextBin)
        
        self.publishQueuedBinFound(x = 0, id = 5)
        self.publishQueuedBinFound(x = 1, id = 6)
        self.publishQueuedBinFound(x = -1, id = 7)
        self.qeventHub.publishEvents()
        
        self.assertTrue(self.machine.currentState()._delay is not None)
        
        # Make sure the timeout has been stopped
        self.assertTrue(MockTimer.LOG[state.FindAttempt.TIMEOUT].stopped)
        self.assertFalse(MockTimer.LOG[randombin.Recover.RETURN].stopped)
        
        self.releaseTimer(randombin.Recover.RETURN)
        
        self.assertDataValue(self.ai.data['binData'], 'currentID', 6)
        
        self.assertFalse(self._atEnd)
        self.assertCurrentState(randombin.Dive)
        
    def testLostFoundLost(self):
        """
        Make sure it doesn't go into CheckDropped unless it's at the end
        after it recovers from a lost event
        """
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        # For Recover
        self.ai.data["lastBinX"] = 0
        self.ai.data["lastBinY"] = 0
        
        self.ai.data['binData']['currentID'] = 3
        self.ai.data['binData']['currentIds'] = set([2,3,4])
        self.ai.data['binData']['itemData'] = {4 : Mock(x = -1),
                                               3 : Mock(x = 0),
                                               2 : Mock(x = 1)}
        self.machine.start(randombin.NextBin)
        self.qeventHub.publishEvents()
        
        self.assertFalse(self._atEnd)
        self.assertFalse(self._centered)
        
        self.publishQueuedBinDropped(id = 4)
        self.publishQueuedBinDropped(id = 2)
        self.publishQueuedBinDropped(id = 3)
        self.injectEvent(vision.EventType.BINS_LOST)
        
        self.assertCurrentState(randombin.RecoverNextBin)
        
        self.publishQueuedBinFound(x = -1, id = 5)
        
        self.publishQueuedBinDropped(id = 5)
        self.injectEvent(vision.EventType.BINS_LOST)
        
        self.assertTrue(MockTimer.LOG[randombin.Recover.RETURN].stopped)
        self.assertFalse(MockTimer.LOG[state.FindAttempt.TIMEOUT].stopped)
        
        self.assertFalse(self._atEnd)
        self.assertCurrentState(randombin.RecoverNextBin)
        
        # Make sure it's able to start the delay timer again
        self.publishQueuedBinFound(id = 6)
        self.assertFalse(MockTimer.LOG[randombin.Recover.RETURN].stopped)
        
class TestDropMarker(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.ai.data['droppingSymbol'] = vision.Symbol.CLUB
        self.machine.start(randombin.DropMarker)
        self.ai.data['binData']['currentID'] = 0

        def binComplete(event):
            self._binComplete = True
        self.qeventHub.subscribeToType(randombin.COMPLETE, binComplete)
    
    def testStart(self):
        """Make sure we start diving, and drop a marker """
        self.ai.data['preBinCruiseDepth'] = 5.0

        # Check to make sure it doesn't drop without two hits first
        self.assertCurrentState(randombin.DropMarker)
        #self.publishQueuedBinFound(id = 0, x = 0, y = 0,
        #                           angle = math.Degree(0))
        self.releaseTimer(randombin.DropMarker.DROP)
        self.qeventHub.publishEvents()

        self.assertCurrentState(randombin.SurfaceToCruise)
        
    def testBinTracking(self):
        self.binTrackingHelper()

    def testDroppedFirst(self):
        """Make sure we move on after dropping the second marker"""

        self.ai.data['preBinCruiseDepth'] = 5.0

        # Try an invalid event first
        self.publishQueuedBinFound(id = 0, x = 0.1, y = 0.1,
                                   angle = math.Degree(0.5))

        # Inject event and test the response
        self.publishQueuedBinFound(id = 0, x = 0, y = 0,
                                   angle = math.Degree(0.5))
        self.qeventHub.publishEvents()

        #self.publishQueuedBinFound(id = 0, x = 0, y = 0,
        #                           angle = math.Degree(0.5))
        self.releaseTimer(randombin.DropMarker.DROP)
        self.qeventHub.publishEvents()

        self.assertCurrentState(randombin.SurfaceToCruise)
        
    def testFinished(self):
        """Ensure FINISHED -> SurfaceToCruise"""
        # Needed for SurfaceToCruise
        self.ai.data['preBinCruiseDepth'] = 5.0 

        self.injectEvent(randombin.DropMarker.FINISHED)

        self.assertCurrentState(randombin.SurfaceToCruise)

    def testTimeout(self):
        """Ensure the timeout drops the marker anyways"""
        
        self.ai.data['preBinCruiseDepth'] = 5.0

        # Publish a bad BIN_FOUND to make sure it doesn't cause the drop
        self.publishQueuedBinFound(id = 0, x = 0.5, y = -0.5,
                                   angle = math.Degree(0.5))
        self.qeventHub.publishEvents()
        self.assertCurrentState(randombin.DropMarker)

        # Now release the timer and check that the marker has dropped
        self.releaseTimer(randombin.DropMarker.DROP)
        self.qeventHub.publishEvents()

        # Make sure the marker was dropped
        self.assertCurrentState(randombin.SurfaceToCruise)
                
class TestSurface(BinTestCase):
    def setUp(self):
        BinTestCase.setUp(self)
        self.vehicle.depth = 10
        self.ai.data['preBinCruiseDepth'] = 5.0
        self.machine.start(randombin.SurfaceToCruise)
    
    def testStart(self):
        """Make sure we start surfacing and are still hovering"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        #self.assertLessThan(self.controller.depth, 10)
        
    def testDiveFinished(self):
        # Subscribe to end event
        self._binComplete = False
        def binComplete(event):
            self._binComplete = True
        self.qeventHub.subscribeToType(randombin.COMPLETE, binComplete)
        
        # Finish the state machine
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assert_(self.machine.complete)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._binComplete)

        # Test the offset data has been deleted
        self.assertFalse(self.ai.data.has_key(
                'dive_offsetTheOffset'))
        self.assertFalse(self.ai.data.has_key(
                'closerlook_offsetTheOffset'))
