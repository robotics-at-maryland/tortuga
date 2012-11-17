# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/gen2/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.gen2.light as light
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math
from ext.estimation import Obstacle

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

# class TestStart(support.AITestCase):
#     def testSuccess(self):
#         """Makes sure the state continues when it has the proper config"""
#         support.AITestCase.setUp(self, cfg = cfg)
#         self.estimator._obstacles = {'buoy' : Obstacle()}
#         self.estimator._obstacles.setDepth(0)
#         self.estimator._obstacles.setPosition(math.Vector2(0, 0))
        
#         self._success = False
#         def _handler(event):
#             self._success = True

#         self.qeventHub.subscribeToType(light.Start.SUCCESS, _handler)
#         self.machine.start(light.Start)

#         self.qeventHub.publishEvents()
#         self.qeventHub.publishEvents()
#         self.assert_(self._success)

#     def testFailure(self):
#         support.AITestCase.setUp(self)

#         self._failure = False
#         def _handler(event):
#             self._failure = True
#         self.qeventHub.subscribeToType(light.Start.FAIL, _handler)
#         self.machine.start(light.Start)

#         self.qeventHub.publishEvents()
#         self.assert_(self._failure)

#     def testInit(self):
#         support.AITestCase.setUp(self, cfg = cfg)
#         self.estimator._obstacles = {'buoy' : Obstacle()}
#         self.estimator._obstacles.setDepth(0)
#         self.estimator._obstacles.setPosition(math.Vector2(0, 0))

#         self.machine.start(light.Start)

#         # Interstate data has been declared
#         self.assert_(self.ai.data.has_key('light'))

# class TestDive(support.AITestCase):
#     def setUp(self):
#         support.AITestCase.setUp(self, cfg = cfg)
#         self.estimator._obstacles = {'buoy' : Obstacle()}
#         self.estimator._obstacles.setDepth(4)
#         self.estimator._obstacles.setPosition(math.Vector2(0, 0))
#         self.machine.start(light.Dive)

#     def testStart(self):
#         """Make sure we are diving with no detector on"""
#         self.assertFalse(self.visionSystem.redLightDetector)
#         self.assertCurrentMotion(motion.basic.RateChangeDepth)
#         self.assertEqual(4, self.estimator.getObstacleDepth('buoy'))

#     def testFinish(self):
#         self.injectEvent(motion.basic.MotionManager.FINISHED)
#         self.assertCurrentState(light.DeterminePath)

# class TestDeterminePath(support.AITestCase):
#     def setUp(self):
#         # Delay when setUp is called
#         pass

#     def testMap(self):
#         # The path should never lead to following the map directly for now
#         cfg = initializeConfig('buoy', [2, 2, 4, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#     def testForward(self):
#         """ Map agrees with pointed direction """
#         cfg = initializeConfig('buoy', [0, 5, 4, 0])
#         support.AITestCase.setUp(self, cfg = cfg)
        
#         # Initialize the vehicle to be pointed north
#         self.estimator.depth = 4
#         self.estimator.position = math.Vector2(0, 0)
#         self.estimator.orientation = \
#             math.Quaternion(math.Degree(0), math.Vector3.UNIT_Z)

#         # Start the machine
#         self.machine.start(light.DeterminePath)
#         self.qeventHub.publishEvents()

#         self.assertCurrentState(light.SearchForward)
#         self.assertCurrentMotion(motion.basic.MoveDistance)

#     def testZigZag(self):
#         """ Map disagrees with pointed direction """
#         cfg = initializeConfig('buoy', [5, 0, 4, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#         # Initialize the vehicle to be pointed north
#         self.estimator.depth = 4
#         self.estimator.position = math.Vector2(0, 0)
#         self.estimator.orientation = \
#             math.Quaternion(math.Degree(0), math.Vector3.UNIT_Z)

#         # Start the machine
#         self.machine.start(light.DeterminePath)
#         self.qeventHub.publishEvents()

#         self.assertCurrentState(light.SearchZigZag)
#         self.assertCurrentMotion(motion.search.ForwardZigZag)

#     def testConfig(self):
#         stateConfig = {
#             'StateMachine' : {
#                 'States' : {
#                     'ram.ai.gen2.light.DeterminePath' : {
#                         'threshold' : 45
#                         }
#                     }
#                 }
#             }
#         cfg = initializeConfig('buoy', [0, 5, 4, 0])
#         cfg.update(stateConfig)

#         support.AITestCase.setUp(self, cfg = cfg)
#         self.machine.start(light.DeterminePath)

# class TestSearchForward(support.AITestCase):
#     def setUp(self):
#         cfg = initializeConfig('buoy', [-5, 0, 4, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#         # Point the vehicle west
#         self.estimator.orientation = math.Quaternion(math.Degree(90),
#                                                    math.Vector3.UNIT_Z)
#         self.machine.start(light.SearchForward)

#     def testStart(self):
#         # Make sure the red light detector is on
#         self.assert_(self.visionSystem.redLightDetector)
#         self.assertCurrentMotion(motion.basic.MoveDistance)

#     def testFound(self):
#         # Assert current state before found event
#         self.assertCurrentState(light.SearchForward)

#         # Publish found event
#         self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent,
#                          0, 0)
#         self.qeventHub.publishEvents()

#         # Make sure light has entered the Align state
#         self.assertCurrentState(light.Align)
#         self.assertCurrentMotion(motion.seek.SeekPointToRange)

#     def testFinish(self):
#         """ MoveDistance has finished """
#         self.assertCurrentState(light.SearchForward)

#         # Finish motion
#         self.motionManager.publish(motion.basic.MotionManager.FINISHED,
#                                    core.Event())
#         self.qeventHub.publishEvents()

#         # Search has changed to Zig Zag search
#         self.assertCurrentState(light.SearchZigZag)
#         self.assertCurrentMotion(motion.search.ForwardZigZag)

# class TestSearchZigZag(support.AITestCase):
#     def setUp(self):
#         cfg = initializeConfig('buoy', [-2.5, 2.5, 4, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#         # Point the vehicle west
#         self.estimator.orientation = math.Quaternion(math.Degree(90),
#                                                      math.Vector3.UNIT_Z)
#         self.machine.start(light.SearchZigZag)

#     def testStart(self):
#         # Check the red light detector is on
#         self.assert_(self.visionSystem.redLightDetector)
#         self.assertCurrentMotion(motion.search.ForwardZigZag)

#     def testFound(self):
#         self.assertCurrentState(light.SearchZigZag)

#         # Inject light found event
#         self.injectEvent(vision.EventType.LIGHT_FOUND, vision.RedLightEvent,
#                          0, 0)
#         self.qeventHub.publishEvents()

#         # Check the state and motion
#         self.assertCurrentState(light.Align)
#         self.assertCurrentMotion(motion.seek.SeekPointToRange)

#     def testTimeout(self):
#         self.assertCurrentState(light.SearchZigZag)

#         # Release timer
#         self.releaseTimer(light.SearchZigZag.TIMEOUT)

#         self.assertCurrentState(light.SearchMap)

# class TestComplete(support.AITestCase):
#     def testStart(self):
#         cfg = initializeConfig('buoy', [0, 0, 0, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#         # Needed for Complete
#         self.ai.data['light'] = {}

#         self._complete = False
#         def _handler(event):
#             self._complete = True

#         conn = self.qeventHub.subscribeToType(light.COMPLETE, _handler)

#         # Check that this is false
#         self.assert_(not self._complete)

#         self.machine.start(light.Complete)

#         # Should complete the state
#         self.qeventHub.publishEvents()
#         self.assert_(self._complete)

#         # Make sure light interstate data deleted
#         self.assert_(not self.ai.data.has_key('light'))

# class TestFailure(support.AITestCase):
#     def testStart(self):
#         cfg = initializeConfig('buoy', [0, 0, 0, 0])
#         support.AITestCase.setUp(self, cfg = cfg)

#         # Needed for Failure
#         self.ai.data['light'] = {}

#         self._failure = False
#         def _handler(event):
#             self._failure = True

#         conn = self.qeventHub.subscribeToType(light.FAILURE, _handler)

#         # Check that this is false
#         self.assert_(not self._failure)

#         self.machine.start(light.Failure)

#         # Should complete the state
#         self.qeventHub.publishEvents()
#         self.assert_(self._failure)

#         # Make sure light interstate data deleted
#         self.assert_(not self.ai.data.has_key('light'))
