# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/course.py

"""
This module tests the ram.ai.course state Machine
"""

# Python Imports
import unittest

# Project Imports
import ram.ai.course as course
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light

import ram.test.ai.support as support

class TestGate(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Gate)
        
    def testStart(self):
        self.assertCurrentState(course.Gate)
        
        # Make sure we branched to the right state machine
        self.assert_(self.machine.branches.has_key(gate.Dive))
        self.assertEqual(1, len(self.machine.branches))
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe1)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
        
class TestPipe1(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Pipe1)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Pipe1)
        
        self.assert_(self.machine.branches.has_key(pipe.Searching))
        self.assertEqual(1, len(self.machine.branches))
        self.assert_(self.visionSystem.pipeLineDetector)
        
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        
        self.injectEvent(pipe.Centering.SETTLED, sendToBranches = True)
        self.assertCurrentState(course.Light)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(pipe.Searching))
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
class TestLight(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Light)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Light)
        
        self.assert_(self.machine.branches.has_key(light.Searching))
        self.assertEqual(1, len(self.machine.branches))
        self.assert_(self.visionSystem.redLightDetector)
        
    def testLightHit(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(light.LIGHT_HIT, sendToBranches = True)
        self.assertCurrentState(course.Pipe2)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(light.Searching))
        self.assertFalse(self.visionSystem.redLightDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.Light)
        
        # Release timer
        self.releaseTimer(course.Light.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe2)
        self.assertFalse(self.machine.branches.has_key(light.Searching))
        self.assertFalse(self.visionSystem.redLightDetector)
        
class TestPipe2(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Pipe2)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Pipe2)
        
        self.assert_(self.machine.branches.has_key(pipe.Searching))
        self.assertEqual(1, len(self.machine.branches))
        self.assert_(self.visionSystem.pipeLineDetector)
        
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        
        self.injectEvent(pipe.Centering.SETTLED, sendToBranches = True)
        self.assertCurrentState(type(None))
        
        # Make sure the branch is gone
        self.assertFalse(self.machine.branches.has_key(pipe.Searching))
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
        # Make sure we are not moving
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.sidewaysSpeed)