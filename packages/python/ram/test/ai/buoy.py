# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/buoy.py

import ext.vision as vision
import ext.math as math

import ram.ai.buoy as buoy
import ram.motion as motion
import ram.motion.basic

import ram.test.ai.support as support

class BuoyTrackingTest(object):
    """
    Base class to test any state that uses BuoyTrackingState.
    This must be used in combination with support.AITestCase, otherwise
    it will crash. Use multiple inheritance to do this.

    The reason this had to be done was because if this inherited from
    support.AITestCase, it would get run as its own set of tests.
    """
    def setUp(self, myState, foundState, lostState,
              cfg = None, buoyData = None):
        if cfg is None:
            cfg = {
                'Ai' : {
                    'config' : {
                        'buoyDepth' : 5,
                        'targetBuoys' : ['yellow', 'green']
                        }
                    }
                }
        
        self._myState = myState
        self._foundState = foundState
        self._lostState = lostState
        support.AITestCase.setUp(self, cfg = cfg)

        if buoyData is not None:
            self.ai.data['buoyData'] = buoyData

        # Setup a handler if the event finishes
        self._complete = False
        def handler(event):
            self._complete = True
        self.qeventHub.subscribeToType(buoy.COMPLETE, handler)

        self.machine.start(myState)

    def testStart(self):
        self.assert_(self.visionSystem.buoyDetector)
        self.assert_(self.ai.data.has_key('buoyData'))
        self.assertEqual(vision.Color.YELLOW,
                         self.machine.currentState()._desiredColor)

        # Make sure there are no lingering events
        self.qeventHub.publishEvents()
        self.assert_(not self._complete)

    def testNoMoreBuoys(self):
        self.machine.stop()
        self.ai.data['buoysHit'] = 2
        self.machine.start(self._myState)

        self.qeventHub.publishEvents()
        self.assert_(self._complete)

    def testCorrectBuoy(self):
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         -0.5, 0.5, vision.Color.YELLOW)

        self.assert_(self.ai.data['buoyData'].has_key(vision.Color.YELLOW))
        self.assertEqual(-0.5, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(0.5, self.ai.data['buoyData'][vision.Color.YELLOW].y)
        self.assertEqual(vision.Color.YELLOW,
                         self.ai.data['buoyData'][vision.Color.YELLOW].color)
        self.assertCurrentState(self._foundState)

    def testIncorrectBuoy(self):
        """
        Incorrect color events should still have their data stored, but
        they should not change the state
        """
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         color = vision.Color.RED)

        self.assert_(self.ai.data['buoyData'].has_key(vision.Color.RED))
        self.assertEqual(vision.Color.RED,
                         self.ai.data['buoyData'][vision.Color.RED].color)
        self.assertCurrentState(self._myState)

    def testBuoyLost(self):
        """
        Buoy data should be deleted and the state should change to the
        lost state
        """
        # Setup the dictionary as if we had received a found event
        event = vision.BuoyEvent(0.0, 0.0, vision.Color.YELLOW)
        self.ai.data['buoyData'][vision.Color.YELLOW] = event

        self.injectEvent(vision.EventType.BUOY_LOST, vision.BuoyEvent,
                         color = vision.Color.YELLOW)
        #self.assert_(not self.ai.data['buoyData'].has_key(vision.Color.YELLOW))
        self.assertCurrentState(self._lostState)

    def testIncorrectBuoyLost(self):
        """
        Buoy data for incorrect buoys should be deleted, but the state
        should not be changed
        """
        event = vision.BuoyEvent(0.0, 0.0, vision.Color.RED)
        self.ai.data['buoyData'][vision.Color.RED] = event

        self.injectEvent(vision.EventType.BUOY_LOST, vision.BuoyEvent,
                         color = vision.Color.RED)
        #self.assert_(not self.ai.data['buoyData'].has_key(vision.Color.RED))
        self.assertCurrentState(self._myState)

class TestStart(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'buoyDepth' : 7
                    }
                },
            'StateMachine' : {
                'ram.ai.buoy.Start' : {
                    'speed' : 0.5
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)
        self.vehicle.orientation = math.Quaternion(math.Degree(45),
                                                   math.Vector3.UNIT_Z)
        self.machine.start(buoy.Start)
        
    def testStart(self):
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAlmostEqual(45, self.ai.data['buoyStartOrientation'], 5)
        self.assertAIDataValue('firstSearching', True)
        self.assertEqual(7, self.motionManager.currentMotion.desiredDepth)

    def testFinish(self):
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(buoy.Searching)

class TestSearching(support.AITestCase, BuoyTrackingTest):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'Buoy' : {
                        'forwardDuration' : 4,
                        'forwardSpeed' : 5,
                    },
                    'targetBuoys' : ['yellow', 'green']
                },
            },
            'StateMachine' : {
                'States' : {
                    'ram.ai.buoy.Searching' : {
                        'legTime' : 10,
                        'sweepAngle'  : 9,
                        'speed' : 8,
                    },
                }
            }
        }
        BuoyTrackingTest.setUp(self, buoy.Searching,
                               buoy.Align, buoy.Searching,
                               cfg = cfg)
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        BuoyTrackingTest.testStart(self)

        self.assert_(self.visionSystem.buoyDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertAIDataValue('buoyStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()
        self.ai.data['firstSearching'] = True

        # Now set the initial direction to something other than 0
        self.ai.data['buoyStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(buoy.Searching)
        self.assert_(self.visionSystem.buoyDetector)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Finish that motion and continue to the ForwardZigZag
        self.machine.currentState()._forwardMotion._finish()

        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('buoyStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)

    def testConfig(self):
        self.assertEqual(10, self.machine.currentState()._legTime)
        self.assertEqual(9, self.machine.currentState()._sweepAngle)
        self.assertEqual(8, self.machine.currentState()._speed)
        self.assertEqual(4, self.machine.currentState()._duration)
        self.assertEqual(5, self.machine.currentState()._forwardSpeed)

    def testBuoyFound(self):
        # Now change states
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         x = 5, y = 6, id = 1, color = vision.Color.YELLOW)
        self.assertCurrentState(buoy.Align)
        self.qeventHub.publishEvents()

        self.assert_(self.ai.data['buoyData'].has_key(vision.Color.YELLOW))
        self.assertEqual(5, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(6, self.ai.data['buoyData'][vision.Color.YELLOW].y)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.buoyDetector)

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
        self.machine.start(buoy.Searching)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

class TestAlign(support.AITestCase, BuoyTrackingTest):
    def setUp(self):
        BuoyTrackingTest.setUp(self, buoy.Align, buoy.Align, buoy.FindAttempt)
    
    def testStart(self):
        self.assertCurrentMotion(motion.seek.SeekPointToRange)
        
    def testBuoyFound(self):
        """Make sure new found events move the vehicle"""
        # Light  dead ahead and below us
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = -0.5,
                         azimuth = math.Degree(15))
        
        # Bigger numbers = deeper, and we want to go deeper
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(-0.5, self.ai.data['buoyData'][vision.Color.YELLOW].y)
        
        # Smaller numbers = shallow, and we want to go shallower
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5,
                         azimuth = math.Degree(15))
        self.assertLessThan(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(0.5, self.ai.data['buoyData'][vision.Color.YELLOW].y)
    
    def testBuoyLost(self):
        """Make sure losing the light goes back to search"""
        # Put a fake event in the buoyData
        event = vision.BuoyEvent(-0.5, 0.5, vision.Color.YELLOW)
        self.ai.data['buoyData'][vision.Color.YELLOW] = event

        self.injectEvent(vision.EventType.BUOY_LOST, vision.BuoyEvent,
                         color = vision.Color.YELLOW)
        self.assertCurrentState(buoy.FindAttempt)
        
    def testSeek(self):
        """Make sure we try to hit the light when close"""
        # Test that the state doesn't change until it gets a POINT_ALIGNED
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5,
                         azimuth = math.Degree(15))
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5,
                         azimuth = math.Degree(15))
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Align)

        # Inject the POINT_ALIGNED event and see if it holds depth
        self.injectEvent(ram.motion.seek.SeekPoint.POINT_ALIGNED)
        self.qeventHub.publishEvents()
        #self.assertEqual(1, self.controller.depthHolds)

        # Now give another light event with too high of a change on only one
        # axis
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.RedLightEvent, 0,
                         0, y = 0, azimuth = math.Degree(15),
                         color = vision.Color.YELLOW)
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Align)

        # Now inject the same event so the change is 0
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0,
                         azimuth = math.Degree(15))
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Seek)

class TestCorrectDepth(support.AITestCase, BuoyTrackingTest):
    def setUp(self):
        BuoyTrackingTest.setUp(self, buoy.CorrectDepth, buoy.CorrectDepth,
                               buoy.FindAttempt)
        self.vehicle.depth = 5

    def testCorrectHeight(self):
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.05)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.Align)

    def testBelow(self):
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = -0.5)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.CorrectDepth)
        self.assertGreaterThan(self.controller.depth, self.vehicle.depth)

    def testDownTooFar(self):
        """
        Test if the vehicle is trying to exit the bottom of its bounding box
        """
        self.vehicle.depth = 6.5
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = -0.5)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.CorrectDepth)
        self.assertEqual(6.5, self.controller.depth)

    def testAbove(self):
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.CorrectDepth)
        self.assertLessThan(self.controller.depth, self.vehicle.depth)

    def testUpTooFar(self):
        """
        Test if the vehicle is trying to exit the top of its bounding box
        """
        self.vehicle.depth = 3.5
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.CorrectDepth)
        self.assertEqual(3.5, self.controller.depth)

class TestSeek(support.AITestCase, BuoyTrackingTest):
    def setUp(self):
        BuoyTrackingTest.setUp(self, buoy.Seek, buoy.Seek, buoy.FindAttemptSeek)
    
    def testStart(self):
        BuoyTrackingTest.testStart(self)

        self.assertCurrentMotion(motion.seek.SeekPoint)
        
    def testBuoyFound(self):
        """Make sure new found events move the vehicle"""
        # Light dead ahead and below us
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = -0.5,
                         azimuth = math.Degree(15))
        
        # Bigger numbers = deeper, and the vehicle should not change depth
        self.assertEqual(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(-0.5, self.ai.data['buoyData'][vision.Color.YELLOW].y)
        
        # Smaller numbers = shallow, and the vehicle should not change depth
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent, 0,
                         0, vision.Color.YELLOW, y = 0.5,
                         azimuth = math.Degree(15))
        self.assertEqual(self.controller.depth, self.vehicle.depth)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.assertEqual(0, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(0.5, self.ai.data['buoyData'][vision.Color.YELLOW].y)
    
    def testBuoyLost(self):
        """Make sure losing the light goes back to search"""
        BuoyTrackingTest.testBuoyLost(self)

        # Add a lastLightEvent for FindAttempt
        self.ai.data['buoyData'][vision.Color.YELLOW] = \
            vision.BuoyEvent(0, 0, vision.Color.YELLOW)
        self.injectEvent(vision.EventType.LIGHT_LOST, vision.BuoyEvent,
                         color = vision.Color.YELLOW)
        self.assertCurrentState(buoy.FindAttemptSeek)

        # Test that it reenters Seek when it's refound
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         0, 0, vision.Color.YELLOW)
        self.assertCurrentState(buoy.Seek)
        
    def testHit(self):
        """Make sure finding the light changes to hit"""
        self.injectEvent(vision.EventType.BUOY_ALMOST_HIT)
        self.assertCurrentState(buoy.Hit)

class TestHit(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'targetBuoys' : ['yellow', 'green']
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)

        # Needed for Reposition
        self.ai.data['buoyStartOrientation'] = 0
        
        self._buoyHit = False
        def handler(event):
            self._buoyHit = True
        self.qeventHub.subscribeToType(buoy.BUOY_HIT, handler)

    def testStart(self):
        self.machine.start(buoy.Hit)
        self.assert_(not self.visionSystem.buoyDetector)
        self.assertEqual(3, self.controller.speed)

    def testReposition(self):
        self.machine.start(buoy.Hit)

        # Release the reposition timer
        self.releaseTimer(buoy.Hit.REPOSITION)
        self.assertCurrentState(buoy.Reposition)
        self.assert_(self._buoyHit)

    def testContinue(self):
        self.ai.data['buoysHit'] = 1
        self.machine.start(buoy.Hit)
        
        # Release the continue timer
        self.releaseTimer(buoy.Hit.CONTINUE)
        self.assertCurrentState(buoy.Continue)
        self.assert_(self._buoyHit)

class TestReposition(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'buoyDepth' : 7,
                    'targetBuoys' : ['yellow', 'red']
                    }
                }
            }

        support.AITestCase.setUp(self, cfg = cfg)

        # Initial values
        self.vehicle.depth = 7.8
        self.ai.data['buoyStartOrientation'] = 15
        self.ai.data['firstSearching'] = False
        
        self.machine.start(buoy.Reposition)

    def testMotion(self):
        """
        Tests if the vehicle moves backwards, followed by changing depth
        """
        self.assert_(not self.visionSystem.buoyDetector)
        self.assertCurrentState(buoy.Reposition)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        # Release the timer to move to the downward motion
        self.releaseTimer(motion.basic.TimedMoveDirection.COMPLETE)

        self.assertCurrentState(buoy.Reposition)
        self.assertCurrentMotion(motion.basic.RateChangeHeading)
        self.assertAlmostEqual(15, self.ai.data['buoyStartOrientation'])
        desiredHeading = self.motionManager.currentMotion._desiredHeading
        self.assertAlmostEqual(15, desiredHeading)

        # Finish the change heading motion
        self.motionManager.currentMotion._finish()

        self.assertCurrentState(buoy.Reposition)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertEqual(7, self.motionManager.currentMotion.desiredDepth)

    def testFinish(self):
        """
        Tests the transition for all motions finishing.
        """
        self.assert_(not self.visionSystem.buoyDetector)
        self.assertCurrentState(buoy.Reposition)

        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.Searching)

class TestContinue(support.AITestCase):
    def testStart(self):
        # Start the state machine
        self.ai.data['buoyStartOrientation'] = 45
        self.machine.start(buoy.Continue)
        
        # Check to make sure it is in the first motion
        self.assertCurrentState(buoy.Continue)
        self.assertCurrentMotion(motion.basic.RateChangeHeading)
        
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()

        self.assertCurrentState(buoy.Continue)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertLessThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        
        # Upward motion
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()
        
        self.assertCurrentState(buoy.Continue)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        # Forward motion
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()
        
        self.assertCurrentState(buoy.Continue)
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertAlmostEqual(self.controller.sidewaysSpeed, 0, 5)
        
        # Finish motion
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()
        
        # Make sure hit the end state
        self.assert_(self.machine.complete)
        self.assertCurrentMotion(type(None))

class TestRecover(support.AITestCase, BuoyTrackingTest):
    def setUp(self):
        buoyData = { vision.Color.YELLOW : vision.BuoyEvent() }
        BuoyTrackingTest.setUp(self, buoy.Recover, buoy.Recover, buoy.Searching,
                               buoyData = buoyData)

    def testNoMoreBuoys(self):
        """This is an invalid test for Recover"""
        pass

    def testBuoyFound(self):
        self.assertCurrentState(buoy.Recover)
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         0.5, 0, vision.Color.YELLOW, range = 7)
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Align)
        self.assertEqual(0.5, self.ai.data['buoyData'][vision.Color.YELLOW].x)
        self.assertEqual(0, self.ai.data['buoyData'][vision.Color.YELLOW].y)
        
    def testBackwardsMovement(self):
        # Stop the machine
        self.machine.stop()
        
        # Set the range to a close enough value
        self.ai.data['buoyData'][vision.Color.YELLOW].range = 4
        
        # Restart the machine
        self.machine.start(buoy.Recover)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), -4.0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

        # Restart the machine, test buoy below
        self.machine.stop()
        self.ai.data['buoyData'][vision.Color.YELLOW].x = 0
        self.ai.data['buoyData'][vision.Color.YELLOW].y = 1
        self.machine.start(buoy.Recover)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(self.controller.getSpeed(), -4.0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)

        # Now inject an event to cause it to change depth
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         0, 1, vision.Color.YELLOW)
        self.qeventHub.publishEvents()
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAlmostEqual(self.controller.getSpeed(), 0, 5)
        self.assertAlmostEqual(self.controller.getSidewaysSpeed(), 0, 5)
        self.assertCurrentState(buoy.Recover)

        # Restart the machine
        self.machine.stop()
        self.ai.data['buoyData'][vision.Color.YELLOW].x = 0
        self.ai.data['buoyData'][vision.Color.YELLOW].y = -1
        self.ai.data['buoyData'][vision.Color.YELLOW].range = 4
        self.machine.start(buoy.Recover)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(-4.0, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)
        
        # Now inject an event to cause it to change depth
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         0, 1, vision.Color.YELLOW)
        self.qeventHub.publishEvents()
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)
        self.assertCurrentState(buoy.Recover)
        
        # Now inject one in the yThreshold
        self.injectEvent(vision.EventType.BUOY_FOUND, vision.BuoyEvent,
                         0, 0, vision.Color.YELLOW)
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoy.Align)
