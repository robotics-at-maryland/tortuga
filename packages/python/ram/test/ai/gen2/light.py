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

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

# Initializes the state estimator with a name : pos in the map
def initializeConfig(name, pos):
    cfg = {
        'Vehicle' : {
            'StateEstimator' : {
                name : pos
                }
            }
        }
    return cfg

class TestStart(support.AITestCase):
    def testSuccess(self):
        """Makes sure the state continues when it has the proper config"""
        cfg = initializeConfig('buoy', [0, 0, 0, 0])

        support.AITestCase.setUp(self, cfg = cfg)
        
        self._success = False
        def _handler(event):
            self._success = True

        self.qeventHub.subscribeToType(light.Start.SUCCESS, _handler)
        self.machine.start(light.Start)

        self.qeventHub.publishEvents()
        self.qeventHub.publishEvents()
        self.assert_(self._success)

    def testFailure(self):
        support.AITestCase.setUp(self)

        self._failure = False
        def _handler(event):
            self._failure = True
        self.qeventHub.subscribeToType(light.Start.FAIL, _handler)
        self.machine.start(light.Start)

        self.qeventHub.publishEvents()
        self.assert_(self._failure)

    def testInit(self):
        cfg = initializeConfig('buoy', [0, 0, 0, 0])
        support.AITestCase.setUp(self, cfg = cfg)

        self.machine.start(light.Start)

        # Interstate data has been declared
        self.assert_(self.ai.data.has_key('light'))

class TestDive(support.AITestCase):
    def setUp(self):
        cfg = initializeConfig('buoy', [0, 0, 4, 0])
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(light.Dive)

    def testStart(self):
        """Make sure we are diving with no detector on"""
        self.assertFalse(self.visionSystem.redLightDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertEqual(4, self.vehicle.getDepth('buoy'))

    def testFinish(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(light.DeterminePath)

class TestDeterminePath(support.AITestCase):
    def setUp(self):
        # Delay when setUp is called
        pass

    def testMap(self):
        # The path should never lead to following the map directly for now
        cfg = initializeConfig('buoy', [2, 2, 4, 0])
        support.AITestCase.setUp(self, cfg = cfg)

    def testForward(self):
        """ Map agrees with pointed direction """
        cfg = initializeConfig('buoy', [0, 5, 4, 0])
        support.AITestCase.setUp(self, cfg = cfg)
        
        # Initialize the vehicle to be pointed north
        self.vehicle.depth = 4
        self.vehicle.position = math.Vector2(0, 0)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(0), math.Vector3.UNIT_Z)

        # Start the machine
        self.machine.start(light.DeterminePath)
        self.qeventHub.publishEvents()

        self.assertCurrentState(light.SearchForward)
        self.assertCurrentMotion(motion.basic.MoveDistance)

    def testZigZag(self):
        """ Map disagrees with pointed direction """
        cfg = initializeConfig('buoy', [5, 0, 4, 0])
        support.AITestCase.setUp(self, cfg = cfg)

        # Initialize the vehicle to be pointed north
        self.vehicle.depth = 4
        self.vehicle.position = math.Vector2(0, 0)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(0), math.Vector3.UNIT_Z)

        # Start the machine
        self.machine.start(light.DeterminePath)
        self.qeventHub.publishEvents()

        self.assertCurrentState(light.SearchZigZag)
        self.assertCurrentMotion(motion.search.ForwardZigZag)

    def testConfig(self):
        stateConfig = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.gen2.light.DeterminePath' : {
                        'threshold' : 45
                        }
                    }
                }
            }
        cfg = initializeConfig('buoy', [0, 5, 4, 0])
        cfg.update(stateConfig)

        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(light.DeterminePath)
