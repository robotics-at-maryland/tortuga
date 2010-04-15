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
import ram.ai.hedge as hedge
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
            'Ai' : {
                'config' : {
                    'hedgeDepth' : TestStart.DEPTH,
                },
            },
            'StateMachine' : {
                'States' : {
                    'ram.ai.hedge.Start' : {
                        'diveSpeed' : TestStart.DIVE_SPEED,
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(hedge.Start)
        
    def testStart(self):
        """Make sure we are diving with no hedge detector on"""
        self.assertFalse(self.visionSystem.hedgeDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAIDataValue('hedgeStartOrientation', 0)

    def testConfig(self):
        # Test the config settings
        m = self.motionManager.currentMotion
        self.assertEqual(TestStart.DEPTH, m.desiredDepth)
        self.assertEqual(TestStart.DIVE_SPEED, m.speed)
        
    def testFinish(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(hedge.Searching)

# class TestFindAttempt(support.AITestCase):
#     TIMEOUT = 2
#     def setUp(self):
#         cfg = {
#             'StateMachine' : {
#                 'States' : {
#                     'ram.ai.hedge.FindAttempt' : {
#                         'timeout' : TestFindAttempt.TIMEOUT,
#                     },
#                 }
#             }
#         }
#         support.AITestCase.setUp(self, cfg = cfg)
#         self.machine.start(hedge.FindAttempt)
    
#     def testStart(self):
#         """Make sure we have the detector on when starting"""
#         self.assert_(self.visionSystem.hedgeDetector)
#         self.assertCurrentMotion(type(None))
#         self.assertEqual(1, self.controller.headingHolds)
                
#     def testHedgeFound(self):
#         # Now change states
#         self.injectEvent(vision.EventType.HEDGE_FOUND, 
#                          vision.HedgeEvent, 0, 0, 0, 0)
#         self.assertCurrentState(hedge.SeekingToCentered)
        
#         # Leave and make sure its still on
#         self.assert_(self.visionSystem.hedgeDetector)

#     def testTimeout(self):
#         """
#         Make sure that the timeout works properly
#         """
#         # Restart with a working timer
#         self.machine.stop()
#         self.machine.start(hedge.FindAttempt)

#         # Make sure the timer has the correct value
#         self.assertEquals(self.machine.currentState()._timeout,
#                           TestFindAttempt.TIMEOUT)
        
#         # For Recover
#         self.ai.data['lastHedgeEvent'] = vision.HedgeEvent()

#         # Release timer
#         self.releaseTimer(state.FindAttempt.TIMEOUT)
        
#         # Test that the timeout worked properly
#         self.assertCurrentState(hedge.Recover)
#         self.assert_(self.visionSystem.hedgeDetector)

# class TestRecover(support.AITestCase):
#     def setUp(self):
#         support.AITestCase.setUp(self)
#         self.ai.data['lastHedgeEvent'] = vision.HedgeEvent()

#     def testLightFound(self):
#         self.machine.start(hedge.Recover)
#         self.assertCurrentState(hedge.Recover)
#         self.injectEvent(vision.EventType.HEDGE_FOUND,
#                          vision.HedgeEvent, 0.5, 0, 0, 0)
#         self.qeventHub.publishEvents()
#         self.assertCurrentState(hedge.SeekingToCentered)
#         self.assertEqual(0.5, self.ai.data['lastHedgeEvent'].x)
#         self.assertEqual(0, self.ai.data['lastHedgeEvent'].y)
        
#     def testBackwardsMovement(self):        
#         # Set the range to a close enough value
#         self.ai.data['lastHedgeEvent'].range = 0.4
        
#         # Restart the machine
#         self.machine.start(hedge.Recover)
#         self.assertCurrentMotion(motion.basic.MoveDirection)
#         self.assertAlmostEqual(self.controller.getSpeed(), -3.0, 5)
#         self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

#         # Restart the machine, test light below
#         self.machine.stop()
#         self.ai.data['lastHedgeEvent'].x = 0
#         self.ai.data['lastHedgeEvent'].y = 1
#         self.machine.start(hedge.Recover)
#         self.assertCurrentMotion(motion.basic.MoveDirection)
#         self.assertAlmostEqual(self.controller.getSpeed(), -3.0, 5)
#         self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

#         # Now inject an event to cause it to change depth
#         self.injectEvent(vision.EventType.HEDGE_FOUND)
#         self.assertCurrentState(hedge.SeekingToCentered)

#     def testNoEvent(self):
#         # Get rid of the event that was created
#         del self.ai.data['lastHedgeEvent']

#         self.machine.start(hedge.Recover)
#         self.assertCurrentMotion(type(None))
#         self.assertAlmostEqual(0, self.controller.getSpeed(), 5)
#         self.assertAlmostEqual(0, self.controller.getSidewaysSpeed(), 5)

#         # Check that injecting an event doesn't do anything bad
#         self.injectEvent(vision.EventType.HEDGE_FOUND)
#         self.assertCurrentState(hedge.SeekingToCentered)
        
class TestSearching(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(hedge.Searching)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.hedgeDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertAIDataValue('hedgeStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()
        self.ai.data['firstSearching'] = True

        # Now set the initial direction to something other than 0
        self.ai.data['hedgeStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(hedge.Searching)
        self.assert_(self.visionSystem.hedgeDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Finish that motion and continue to the ForwardZigZag
        self.machine.currentState()._forwardMotion._finish()

        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('hedgeStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)

    def testHedgeFound(self):
        # Now change states
        self.injectEvent(vision.EventType.HEDGE_FOUND, 
                         vision.HedgeEvent, 0, 0, 0, 0)
        self.assertCurrentState(hedge.SeekingToCentered)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.hedgeDetector)

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
        self.machine.start(hedge.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

class TestAlternateSearching(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'Hedge' : {
                        'forwardDuration' : 0
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(hedge.Searching)

    def testStart(self):
        self.assertCurrentState(hedge.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        
class TestRangeXYHold(support.AITestCase):
    def setUp(self, stateType = hedge.RangeXYHold,
              lostState = hedge.FindAttempt,
              recoverState = hedge.SeekingToCentered):
        self._stateType = stateType
        self._lostState = lostState
        self._recoverState = recoverState
        support.AITestCase.setUp(self)
        self.machine.start(stateType)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testHedgeFound(self):
        """Make sure new found events move the vehicle"""
        self.injectEvent(vision.EventType.HEDGE_FOUND, 
                         vision.HedgeEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 4, squareNess = 2)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
    
    def testHedgeLost(self):
        """Make sure losing the hedge goes back to search"""
        self.injectEvent(vision.EventType.HEDGE_LOST)
        self.assertCurrentState(self._lostState)

        # Check that finding the hedge moves back
        self.injectEvent(vision.EventType.HEDGE_FOUND)
        self.assertCurrentState(self._recoverState)
        
    def testInRange(self, squareNess = 2):
        # Subscribe to in range event
        self._inRange = False
        def inRange(event):
            self._inRange = True
        self.qeventHub.subscribeToType(hedge.SeekingToRange.IN_RANGE, 
                                       inRange)
        
        # Inject and event which has the duct ahead, and at the needed range
        self.injectEvent(vision.EventType.HEDGE_FOUND, 
                         vision.HedgeEvent, 0, 0, 0, 0,
                         x = 0.05, y = -0.1, range = 0.53, 
                         squareNess = squareNess)
        
        # Make sure we get the IN_RANGE event
        self.qeventHub.publishEvents()

# class TestSeekingToCentered(TestRangeXYHold):
#     def setUp(self):
#         TestRangeXYHold.setUp(self, hedge.SeekingToCentered)

#     def testPointAligned(self):
#         self._aligned = False
#         def aligned(event):
#             self._aligned = True
#         self.qeventHub.subscribeToType(ram.motion.seek.SeekPoint.POINT_ALIGNED,
#                                        aligned)

#         # Inject an event which does not have the point aligned
#         self.injectEvent(vision.EventType.HEDGE_FOUND,
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 1.0, y = 1.0, range = 3.0, squareNess = 1)
#         self.qeventHub.publishEvents()
#         self.assertCurrentState(hedge.SeekingToCentered)

#         # Inject an event with the point aligned, set large values to range
#         # make sure that the range has nothing to do with it
#         self.injectEvent(vision.EventType.HEDGE_FOUND,
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 0.0, y = 0.0, range = 3.0, squareNess = 1)
#         self.qeventHub.publishEvents()
#         self.assertCurrentState(hedge.SeekingToRange)
#         self.assertEquals(1, self.controller.depthHolds)

#     def testHedgeFound(self):
#         """Make sure new found events move the vehicle"""
#         self.injectEvent(vision.EventType.HEDGE_FOUND, 
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 0.5, y = -0.5, range = 4, squareNess = 1)
        
#         # Bigger numbers = deeper
#         self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
#         # TODO: Take a close look at range seeking here
#         #self.assertEqual(self.controller.speed, 0)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
#         self.assertEqual(self.controller.yawChange, 0)

#     def testInRange(self):
#         TestRangeXYHold.testInRange(self)

#         # Make sure the event was ignored
#         self.assertCurrentState(hedge.SeekingToCentered)

# class TestSeekingToRange(TestRangeXYHold):
#     def setUp(self):
#         TestRangeXYHold.setUp(self, hedge.SeekingToRange,
#                               hedge.FindAttemptRange,
#                               hedge.SeekingToRange)
           
#     def testInRange(self):
#         TestRangeXYHold.testInRange(self)

#         # Make sure we ended up in the right place
#         self.assertCurrentState(hedge.SeekingToAligned)

# class AlignmentTest(object):
#     # This is not a real setUp function, but it must be called anyways
#     def setUp(self, myState, lostState, recoverState = None):
#         self._myState = myState
#         self._lostState = lostState
#         if recoverState is None:
#             self._recoverState = myState
#         else:
#             self._recoverState = recoverState

#     def testStart(self):
#         self.assertCurrentMotion(motion.duct.DuctSeekAlign)
    
#     def testHedgeFound(self):
#         """Make sure new found events move the vehicle"""
#         # Hedge to the right, below, and hedge misalligned right
#         self.injectEvent(vision.EventType.HEDGE_FOUND, 
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 0.5, y = -0.5, range = 3.5, squareNess = 0.5)
        
#         # Bigger numbers = deeper
#         self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
#         self.assertGreaterThan(self.controller.speed, 0)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
#         self.assertLessThan(self.controller.yawChange, 0)
    
#     def testHedgeLost(self):
#         """Make sure losing the light goes back to search"""
#         self.injectEvent(vision.EventType.HEDGE_LOST)
#         self.assertCurrentState(self._lostState)

#         # Check that finding the hedge moves back
#         self.injectEvent(vision.EventType.HEDGE_FOUND)
#         self.assertCurrentState(self._recoverState)

# class TestSeekingToAligned(AlignmentTest, support.AITestCase):
#     def setUp(self):
#         support.AITestCase.setUp(self)
#         AlignmentTest.setUp(self, hedge.SeekingToAligned,
#                             hedge.FindAttemptAligned)
#         self.machine.start(hedge.SeekingToAligned)

#     def testAligned(self):
#         # Subscribe to in range event
#         self._aligned = False
#         def aligned(event):
#             self._aligned = True
#         self.qeventHub.subscribeToType(hedge.SeekingToAligned.ALIGNED, 
#                                        aligned)
        
#         # Inject and event which has the hedge ahead, and at the needed range
#         self.injectEvent(vision.EventType.HEDGE_FOUND, 
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 0.05, y = -0.1, range = 0.31, squareNess = 1)
        
#         # Make sure we get the ALIGNED event
#         self.qeventHub.publishEvents()
#         self.assert_(self._aligned)
#         self.assertCurrentState(hedge.FireTorpedos)
        
#     def _sendSquareNessEvent(self, squareNess):
#         self.injectEvent(vision.EventType.HEDGE_FOUND, 
#                          vision.HedgeEvent, 0, 0, 0, 0,
#                          x = 0, y = 0, range = 3, squareNess = squareNess)
        
#     def testCheckSquareNessBad(self):
#         # Inject and event which has the hedge ahead, and at the needed range
#         self._sendSquareNessEvent(0.75)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
#         # Squareness getting worse
#         self._sendSquareNessEvent(0.5)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
#         # Check on squareness 
#         self.releaseTimer(hedge.SeekingToAligned.CHECK_DIRECTION)
        
#         # Inject another update and make sure the sign has flipped
#         self._sendSquareNessEvent(0.5)
#         self.assertLessThan(self.controller.sidewaysSpeed, 0)
        
#     def testCheckSquareNessGood(self):
#         # Inject and event which has the hedge ahead, and at the needed range
#         self._sendSquareNessEvent(0.5)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
#         # Squareness getting worse
#         self._sendSquareNessEvent(0.75)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
#         # Check on squareness 
#         self.releaseTimer(hedge.SeekingToAligned.CHECK_DIRECTION)
        
#         # Inject another update and make sure the sign hasn't flipped
#         self._sendSquareNessEvent(0.75)
#         self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
# class TestAligning(AlignmentTest, support.AITestCase):
#     def setUp(self):
#         support.AITestCase.setUp(self)
#         AlignmentTest.setUp(self, hedge.Aligning,
#                             hedge.FindAttempt,
#                             hedge.SeekingToCentered)
#         self.machine.start(hedge.Aligning)
        
#     def testStart(self):
#         AlignmentTest.testStart(self)
#         self.releaseTimer(hedge.Aligning.SETTLED)
#         self.assertCurrentState(hedge.Through)
        
#     def testSettle(self):
#         self.injectEvent(hedge.Aligning.SETTLED)
#         self.assertCurrentState(hedge.Through)

class Through(support.AITestCase):
    def setUp(self):
        # Leave set up for each individual test
        pass

    def testStart(self):
        support.AITestCase.setUp(self)

        # Turn this on, so we make sure it goes off
        self.visionSystem.hedgeDetector = True
        
        # Make sure we start driving forward
        self.machine.start(hedge.Through)
        self.assert_(self.controller.speed > 0)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertEqual(False, self.visionSystem.hedgeDetector)

        # Subscribe to end events
        self._throughHedge = False
        def throughHedge(event):
            self._throughHedge = True
        self.qeventHub.subscribeToType(
            motion.basic.MotionManager.FINISHED, throughHedge)
        
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(hedge.COMPLETE, complete)
        
        # Now make sure we stop
        self.releaseTimer(motion.basic.TimedMoveDirection.COMPLETE)
        self.qeventHub.publishEvents()
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we get the final event
        self.qeventHub.publishEvents()
        self.assert_(self._throughHedge)
        self.assert_(self._complete)
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)

    def testDVLMotion(self):
        # Replace the current test vehicle with a new one that has the DVL
        cfg = {
            'Vehicle' : {
                'Devices' : {
                    'DVL' : {
                        'type' : 'MockDVL'
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)

        self.machine.start(hedge.Through)
        self.assert_(self.controller.speed > 0)
        self.assertCurrentMotion(motion.basic.MoveDistance)
