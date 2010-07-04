# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/window.py

# Python Imports
import unittest

# Project Imports
import ram.ai.state as state
import ram.ai.window as window
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

class WindowTest(support.AITestCase):
    def setUp(self, cfg = None):
        support.AITestCase.setUp(self, cfg = cfg)
        self.ai.data['windowData'] = {}
        
class TestStart(WindowTest):
    DEPTH = 7
    OFFSET = 0.5
    DIVE_SPEED = 2
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'windowDepth' : TestStart.DEPTH,
                    'windowOffset' : TestStart.OFFSET,
                },
            },
            'StateMachine' : {
                'States' : {
                    'ram.ai.window.Start' : {
                        'diveSpeed' : TestStart.DIVE_SPEED,
                    },
                }
            }
        }
        WindowTest.setUp(self, cfg = cfg)
        self.machine.start(window.Start)
        
    def testStart(self):
        """Make sure we are diving with no window detector on"""
        self.assertFalse(self.visionSystem.windowDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAIDataValue('windowStartOrientation', 0)

    def testConfig(self):
        # Test the config settings
        m = self.motionManager.currentMotion
        self.assertEqual(TestStart.DEPTH, m.desiredDepth)
        self.assertEqual(TestStart.OFFSET,
                         self.ai.data['windowOffset'])
        self.assertEqual(TestStart.DIVE_SPEED, m.speed)
        
    def testFinish(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(window.Searching)

class TestFindAttempt(support.AITestCase):
    TIMEOUT = 2
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['red', 'yellow']
                    }
                },
            'StateMachine' : {
                'States' : {
                    'ram.ai.window.FindAttempt' : {
                        'timeout' : TestFindAttempt.TIMEOUT,
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(window.FindAttempt)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.windowDetector)
        self.assertCurrentMotion(type(None))
        self.assertEqual(1, self.controller.headingHolds)
                
    def testWindowFound(self):
        # Now change states
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, color = vision.Color.RED)
        self.assertCurrentState(window.SeekingToCentered)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.windowDetector)

    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(window.FindAttempt)

        # Make sure the timer has the correct value
        self.assertEquals(self.machine.currentState()._timeout,
                          TestFindAttempt.TIMEOUT)
        
        # For Recover
        self.ai.data['windowData'][vision.Color.RED] = vision.WindowEvent()

        # Release timer
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(window.Recover)
        self.assert_(self.visionSystem.windowDetector)

class TestRecover(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['red', 'yellow']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        self.ai.data['windowData'] = { vision.Color.RED : vision.WindowEvent() }

    def testWindowFound(self):
        self.machine.start(window.Recover)
        self.assertCurrentState(window.Recover)
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0.5, 0, 0, 0,
                         color = vision.Color.RED)
        self.qeventHub.publishEvents()
        self.assertCurrentState(window.SeekingToCentered)
        self.assertEqual(0.5, self.ai.data['windowData'][vision.Color.RED].x)
        self.assertEqual(0, self.ai.data['windowData'][vision.Color.RED].y)
        
    def testBackwardsMovement(self):        
        # Set the range to a close enough value
        self.ai.data['windowData'][vision.Color.RED].range = 0.4
        
        # Restart the machine
        self.machine.start(window.Recover)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), -3.0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

        # Restart the machine, test light below
        self.machine.stop()
        self.ai.data['windowData'][vision.Color.RED].x = 0
        self.ai.data['windowData'][vision.Color.RED].y = 1
        self.machine.start(window.Recover)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), -3.0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

        # Now inject an event to cause it to change depth
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, color = vision.Color.RED)
        self.assertCurrentState(window.SeekingToCentered)

    def testNoEvent(self):
        # Get rid of the event that was created
        del self.ai.data['windowData'][vision.Color.RED]

        self.machine.start(window.Recover)
        self.assertCurrentMotion(type(None))
        self.assertAlmostEqual(0, self.controller.getSpeed(), 5)
        self.assertAlmostEqual(0, self.controller.getSidewaysSpeed(), 5)

        # Check that injecting an event doesn't do anything bad
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, color = vision.Color.RED)
        self.assertCurrentState(window.SeekingToCentered)
        
class TestSearching(WindowTest):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['yellow', 'green']
                    }
                }
            }

        WindowTest.setUp(self, cfg = cfg)
        self.machine.start(window.Searching)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.windowDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertAIDataValue('windowStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()
        self.ai.data['firstSearching'] = True

        # Now set the initial direction to something other than 0
        self.ai.data['windowStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(window.Searching)
        self.assert_(self.visionSystem.windowDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Finish that motion and continue to the ForwardZigZag
        self.machine.currentState()._forwardMotion._finish()

        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('windowStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)

    def testCorrectWindowFound(self):
        # Now change states
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0, vision.Color.YELLOW)
        self.assertCurrentState(window.Approach)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.windowDetector)

    def testWrongWindowFound(self):
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0, vision.Color.BLUE)
        self.assertCurrentState(window.Searching)

        # Make sure the detector is still on and searching
        self.assert_(self.visionSystem.windowDetector)

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
        self.machine.start(window.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

class TestCorrectHeight(WindowTest):
    DEPTH = 5

    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['yellow', 'green'],
                    'windowDepth' : TestCorrectHeight.DEPTH
                    }
                }
            }
        WindowTest.setUp(self, cfg = cfg)
        self.vehicle.depth = TestCorrectHeight.DEPTH

    def testDownwardMotion(self):
        # Create a fake event with the y position positive
        event = vision.WindowEvent()
        event.x, event.y, event.color = (0, -0.75, vision.Color.YELLOW)
        self.ai.data['windowData'][vision.Color.YELLOW] = event

        # Start the machine
        self.machine.start(window.CorrectHeight)

        # Check if the dive motion is downwards (and follows the offset)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertCurrentState(window.CorrectHeight)
        desiredDepth = self.motionManager.currentMotion.desiredDepth
        self.assertAlmostEqual(5.3, desiredDepth, 5)

    def testUpwardMotion(self):
        # Create a fake event with the y position positive
        event = vision.WindowEvent()
        event.x, event.y, event.color = (0, 0.75, vision.Color.YELLOW)
        self.ai.data['windowData'][vision.Color.YELLOW] = event

        # Start the machine
        self.machine.start(window.CorrectHeight)

        # Check if the dive motion is downwards (and follows the offset)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertCurrentState(window.CorrectHeight)
        desiredDepth = self.motionManager.currentMotion.desiredDepth
        self.assertAlmostEqual(4.7, desiredDepth, 5)
        
    def testFinish(self):
        # Create a fake event with the y position positive
        event = vision.WindowEvent()
        event.x, event.y, event.color = (0, -0.75, vision.Color.YELLOW)
        self.ai.data['windowData'][vision.Color.YELLOW] = event

        # Start the machine
        self.machine.start(window.CorrectHeight)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)

        # Test motion finished
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(window.SeekingToCentered)

class TestAlternateSearching(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'Window' : {
                        'forwardDuration' : 0
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(window.Searching)

    def testStart(self):
        self.assertCurrentState(window.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        
class TestRangeXYHold(support.AITestCase):
    def setUp(self, stateType = window.RangeXYHold,
              lostState = window.FindAttempt,
              recoverState = window.SeekingToCentered,
              color = vision.Color.RED, cfg = None):
        if cfg is None:
            cfg = {
                'Ai' : {
                    'config' : {
                        'targetWindows' : ['red', 'green']
                        }
                    }
                }

        self._stateType = stateType
        self._lostState = lostState
        self._recoverState = recoverState
        self._color = color
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(stateType)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testWindowFound(self):
        """Make sure new found events move the vehicle"""
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 4, squareNess = 1,
                         color = self._color)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)
    
    def testWindowLost(self):
        """Make sure losing the window goes back to search"""
        self.injectEvent(vision.EventType.WINDOW_LOST, vision.WindowEvent,
                         color = self._color)
        self.assertCurrentState(self._lostState)

        # Check that finding the window moves back
        self.injectEvent(vision.EventType.WINDOW_FOUND, vision.WindowEvent,
                         color = self._color)
        self.assertCurrentState(self._recoverState)

    def testIncorrectWindow(self, squareNess = 1, color = vision.Color.YELLOW):
        """
        Sends an event for an incorrectly colored target. The IN_RANGE
        event should not be received because the event should be vetoed
        before that can happen.
        """
        self._inRange = False
        def inRange(event):
            self._inRange = True
        self.qeventHub.subscribeToType(window.SeekingToRange.IN_RANGE,
                                       inRange)

        # Inject an event with the wrong target ahead, but everything else good
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.05, y = -0.1, range = 0.53,
                         squareNess = squareNess, color = color)

        self.qeventHub.publishEvents()
        self.assert_(not self._inRange)
        
    def testInRange(self, squareNess = 1, color = vision.Color.RED):
        # Subscribe to in range event
        self._inRange = False
        def inRange(event):
            self._inRange = True
        self.qeventHub.subscribeToType(window.SeekingToRange.IN_RANGE, 
                                       inRange)
        
        # Inject and event which has the duct ahead, and at the needed range
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.05, y = -0.1, range = 0.53, 
                         squareNess = squareNess, color = color)
        
        # Make sure we get the IN_RANGE event
        self.qeventHub.publishEvents()
        self.assert_(self._inRange)

class TestApproach(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['yellow', 'blue']
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(window.Approach)

    def testWindowFound(self):
        self.assertEqual(vision.Color.YELLOW,
                         self.machine.currentState()._desiredColor)

        self.injectEvent(vision.EventType.WINDOW_FOUND, vision.WindowEvent,
                         x = -0.25, y = 0.5, range = 5,
                         color = vision.Color.RED)
        self.assertGreaterThan(self.controller.speed, 0.0)
        self.assertLessThan(self.controller.sidewaysSpeed, 0.0)

        # Find another window with a large positive x
        self.injectEvent(vision.EventType.WINDOW_FOUND, vision.WindowEvent,
                         x = 0.75, y = 0.5, range = 5,
                         color = vision.Color.GREEN)
        self.assertGreaterThan(self.controller.speed, 0.0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0.0)

    def testInRange(self):
        # Wrong window type doesn't transfer states
        self.assertCurrentState(window.Approach)
        self.injectEvent(vision.EventType.WINDOW_FOUND, vision.WindowEvent,
                         x = 0.0, y = 0.0, range = 0.5,
                         color = vision.Color.RED)
        self.qeventHub.publishEvents()
        # If this assertion is false, the program will crash (no failure)
        self.assertCurrentState(window.Approach)

        # Find the window and check for the publish
        self.injectEvent(vision.EventType.WINDOW_FOUND, vision.WindowEvent,
                         x = 0.0, y = 0.0, range = 0.5,
                         color = vision.Color.YELLOW)
        self.qeventHub.publishEvents()

        self.assertCurrentState(window.ApproachAligning)

class TestSeekingToCentered(TestRangeXYHold):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['blue', 'yellow']
                    }
                }
            }
        TestRangeXYHold.setUp(self, window.SeekingToCentered, cfg = cfg,
                              color = vision.Color.BLUE)

    def pointAlignedTemplate(self, color, succeed = True):
        self._aligned = False
        def aligned(event):
            self._aligned = True
        self.qeventHub.subscribeToType(ram.motion.seek.SeekPoint.POINT_ALIGNED,
                                       aligned)

        # Inject an event which does not have the point aligned
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 1.0, y = 1.0, range = 3.0,
                         squareNess = 1, color = color)
        self.qeventHub.publishEvents()
        # Same state for both success and failure
        self.assertCurrentState(window.SeekingToCentered)

        # Inject an event with the point aligned, set large values to range
        # make sure that the range has nothing to do with it
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.0, y = 0.0, range = 3.0,
                         squareNess = 1, color = color)
        self.qeventHub.publishEvents()
        if succeed:
            # On success (correct color)
            self.assertCurrentState(window.SeekingToRange)
            self.assertEquals(1, self.controller.depthHolds)
        else:
            # On failure (incorrect color)
            self.assertCurrentState(window.SeekingToCentered)
            self.assertEquals(0, self.controller.depthHolds)

    def testPointAligned_correct(self):
        """
        Tests the point aligned events if the correct color
        is found
        """
        self.pointAlignedTemplate(vision.Color.BLUE, True)
        self.assert_(self._aligned)

    def testPointAligned_incorrect(self):
        """
        Tests the point aligned events if the incorrect color
        is found
        """
        self.pointAlignedTemplate(vision.Color.RED, False)
        self.assert_(not self._aligned)

    def testWindowFound(self):
        """Make sure new found events move the vehicle"""
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 4,
                         squareNess = 1, color = vision.Color.BLUE)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        # TODO: Take a close look at range seeking here
        #self.assertEqual(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(self.controller.yawChange, 0)

    def testIncorrectWindow(self):
        TestRangeXYHold.testIncorrectWindow(self, color = vision.Color.YELLOW)

        # Make sure the event was ignored
        self.assertCurrentState(window.SeekingToCentered)

    def testInRange(self):
        TestRangeXYHold.testInRange(self, color = vision.Color.BLUE)

        # Make sure the event was ignored
        self.assertCurrentState(window.SeekingToCentered)

class TestSeekingToRange(TestRangeXYHold):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['green', 'blue']
                    }
                }
            }

        TestRangeXYHold.setUp(self, window.SeekingToRange,
                              window.FindAttemptRange,
                              window.SeekingToRange,
                              color = vision.Color.GREEN, cfg = cfg)
           
    def testInRange(self):
        TestRangeXYHold.testInRange(self, color = vision.Color.GREEN)

        # Make sure we ended up in the right place
        self.assertCurrentState(window.FireTorpedos)

    def testIncorrectWindow(self):
        TestRangeXYHold.testIncorrectWindow(self, color = vision.Color.YELLOW)

        # Make sure we did not change state
        self.assertCurrentState(window.SeekingToRange)

class TestFireTorpedos(TestRangeXYHold):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['blue', 'red']
                    }
                }
            }

        TestRangeXYHold.setUp(self, window.FireTorpedos,
                              window.FindAttemptFireTorpedos,
                              window.FireTorpedos,
                              cfg = cfg, color = vision.Color.BLUE)

    def testInRange(self):
        TestRangeXYHold.testInRange(self, color = vision.Color.BLUE)

    def testIncorrectWindow(self):
        TestRangeXYHold.testIncorrectWindow(self, color = vision.Color.GREEN)

    def testArmTorpedos(self):
        self.assertTrue(self.machine.currentState().armed)
        #self.releaseTimer(window.FireTorpedos.ARM_TORPEDOS)
        self.assert_(self.machine.currentState().armed)
           
    def testInRangeUnarmed(self):
        """Make sure the nothing is fired and we haven't transitioned"""
        self.machine.currentState()._armed = False
        TestRangeXYHold.testInRange(self, color = vision.Color.BLUE)

        self.assertEqual(0, self.ai.data.get('torpedosFired', 0))
        self.assertCurrentState(window.FireTorpedos)

    def testInRangeArmed(self):
        # Torpedo starts armed
        self.assert_(self.machine.currentState().armed)

        # Fire first torpedo
        TestRangeXYHold.testInRange(self, squareNess = 1,
                                    color = vision.Color.BLUE)
        self.assertEqual(1, self.ai.data['torpedosFired'])
        self.assertEqual(1, self.vehicle.torpedosFired)
        self.qeventHub.publishEvents()
        self.assertCurrentState(window.Reposition)

    def testSecondTorpedo(self):
        # Set the torpedos fired to 1 and restart
        self.machine.stop()

        # Setup
        self.vehicle.torpedosFired = 1
        self.ai.data['torpedosFired'] = 1
        self._finished = False
        def handler(event):
            self._finished = True
        self.qeventHub.subscribeToType(window.COMPLETE, handler)

        # Restart the state machine
        self.machine.start(window.FireTorpedos)

        # Torpedo starts armed
        self.assert_(self.machine.currentState().armed)

        # Fire second torpedo
        TestRangeXYHold.testInRange(self, color = vision.Color.RED)
        self.assertEqual(2, self.ai.data['torpedosFired'])
        self.assertEqual(2, self.vehicle.torpedosFired)
        self.qeventHub.publishEvents()
        self.assert_(self._finished)

    def testNotAligned(self):
        # Torpedo starts armed
        self.assert_(self.machine.currentState().armed)

        # Fire first torpedo
        TestRangeXYHold.testInRange(self, squareNess = 0,
                                    color = vision.Color.BLUE)
        self.qeventHub.publishEvents()
        self.assertEqual(0, self.ai.data.get('torpedosFired', 0))
        self.assertCurrentState(window.SeekingToAligned)

class AlignmentTest(object):
    # This is not a real setUp function, but it must be called anyways
    def setUp(self, myState, color, lostState, recoverState = None):
        self._myState = myState
        self._lostState = lostState
        if recoverState is None:
            self._recoverState = myState
        else:
            self._recoverState = recoverState
        self._color = color

    def testStart(self):
        self.assertCurrentMotion(motion.duct.DuctSeekAlign)
    
    def testWindowFound(self):
        """Make sure new found events move the vehicle"""
        # Window to the right, below, and window misalligned right
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 3.5,
                         squareNess = 0.5, color = self._color)
        
        # Bigger numbers = deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertLessThan(self.controller.yawChange, 0)

    def testIncorrectWindow(self, color = vision.Color.UNKNOWN):
        # Test that the filters don't change when an incorrect window is seen
        self.assertEqual(0.0, self.machine.currentState()._filterdX)
        self.assertEqual(0.0, self.machine.currentState()._filterdY)
        self.assertEqual(0.0, self.machine.currentState()._filterdRange)
        self.assertEqual(0.0, self.machine.currentState()._filterdAlign)

        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 3.5,
                         squareNess = 0.5, color = color)

        # Nothing should have changed
        self.assertEqual(0.0, self.machine.currentState()._filterdX)
        self.assertEqual(0.0, self.machine.currentState()._filterdY)
        self.assertEqual(0.0, self.machine.currentState()._filterdRange)
        self.assertEqual(0.0, self.machine.currentState()._filterdAlign)

        # Make sure that correct events are getting through
        self.injectEvent(vision.EventType.WINDOW_FOUND,
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.5, y = -0.5, range = 3.5,
                         squareNess = 0.5, color = self._color)

        self.assertEqual(0.5, self.machine.currentState()._filterdX)
        self.assertEqual(-0.5, self.machine.currentState()._filterdY)
        self.assertEqual(3.5, self.machine.currentState()._filterdRange)
        self.assertEqual(-0.5, self.machine.currentState()._filterdAlign)
    
    def testWindowLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.WINDOW_LOST, color = self._color)
        self.assertCurrentState(self._lostState)

        # Check that finding the window moves back
        self.injectEvent(vision.EventType.WINDOW_FOUND, color = self._color)
        self.assertCurrentState(self._recoverState)

class TestSeekingToAligned(AlignmentTest, support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetWindows' : ['red', 'green']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        AlignmentTest.setUp(self, window.SeekingToAligned,
                            vision.Color.RED,
                            window.FindAttemptAligned)
        self.machine.start(window.SeekingToAligned)

    def testAligned(self):
        # Subscribe to in range event
        self._aligned = False
        def aligned(event):
            self._aligned = True
        self.qeventHub.subscribeToType(window.SeekingToAligned.ALIGNED, 
                                       aligned)
        
        # Inject and event which has the window ahead, and at the needed range
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0.05, y = -0.1, range = 0.31,
                         squareNess = 1, color = self._color)
        
        # Make sure we get the ALIGNED event
        self.qeventHub.publishEvents()
        self.assert_(self._aligned)
        self.assertCurrentState(window.FireTorpedos)
        
    def _sendSquareNessEvent(self, squareNess):
        self.injectEvent(vision.EventType.WINDOW_FOUND, 
                         vision.WindowEvent, 0, 0, 0, 0,
                         x = 0, y = 0, range = 3,
                         squareNess = squareNess,
                         color = self._color)
        
    def testCheckSquareNessBad(self):
        # Inject and event which has the window ahead, and at the needed range
        self._sendSquareNessEvent(0.75)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
        # Squareness getting worse
        self._sendSquareNessEvent(0.5)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
        # Check on squareness 
        self.releaseTimer(window.SeekingToAligned.CHECK_DIRECTION)
        
        # Inject another update and make sure the sign has flipped
        self._sendSquareNessEvent(0.5)
        self.assertLessThan(self.controller.sidewaysSpeed, 0)
        
    def testCheckSquareNessGood(self):
        # Inject and event which has the window ahead, and at the needed range
        self._sendSquareNessEvent(0.5)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
        # Squareness getting worse
        self._sendSquareNessEvent(0.75)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        
        # Check on squareness 
        self.releaseTimer(window.SeekingToAligned.CHECK_DIRECTION)
        
        # Inject another update and make sure the sign hasn't flipped
        self._sendSquareNessEvent(0.75)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)

class TestApproachAligning(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'windowDepth' : 12,
                    'targetWindows' : ['yellow', 'red']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(window.ApproachAligning)

    def testWindowFound(self):
        pass
        
class TestReposition(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'windowDepth' : 12,
                    'targetWindows' : ['yellow', 'red']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)

        # Initial values
        self.vehicle.depth = 11.7
        self.ai.data['windowStartOrientation'] = 15
        self.ai.data['firstSearching'] = False
        
        self.machine.start(window.Reposition)

    def testMotion(self):
        """
        Tests if the vehicle moves backwards, followed by changing depth
        """
        self.assertCurrentState(window.Reposition)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertAIDataValue('windowStartOrientation', 0)
        self.assertAIDataValue('firstSearching', True)

        # Release the timer to move to the downward motion
        self.releaseTimer(motion.basic.TimedMoveDirection.COMPLETE)

        self.assertCurrentState(window.Reposition)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertEqual(12, self.motionManager.currentMotion.desiredDepth)

    def testFinish(self):
        """
        Tests the transition for all motions finishing.
        """
        self.assertCurrentState(window.Reposition)

        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.qeventHub.publishEvents()

        self.assertCurrentState(window.Searching)
