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
import ext.vision as vision

import ram.ai.course as course
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.bin as bin
import ram.ai.sonar as sonar
import ram.ai.safe as safe

import ram.motion as motion
import ram.motion.basic
import ram.motion.pipe

import ram.test.ai.support as support

class PipeTestCase(support.AITestCase):
    def setUp(self, testState = pipe.Searching):
        support.AITestCase.setUp(self)
        self.testState = testState
    
    def checkStart(self, currentState):
        self.assertCurrentState(currentState)
        self.assertCurrentBranches([self.testState])
        
    def checkSettled(self, nextState = None):
        self.injectEvent(pipe.Centering.SETTLED, sendToBranches = True)
        if not (nextState is None):
            self.assertCurrentState(nextState)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(self.testState))
        
        # Make sure we are not moving
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.sidewaysSpeed)
        
        # For the time being this is off
        self.assertFalse(self.visionSystem.pipeLineDetector)


class TestGate(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Gate)
        
    def testStart(self):
        self.assertCurrentState(course.Gate)
        
        # Make sure we branched to the right state machine
        self.assertCurrentBranches([gate.Dive])
        self.assert_(self.visionSystem.pipeLineDetector)
        self.assertAIDataValue('foundPipeEarly', False)
        
    def testPipeFound(self):
        self.assertCurrentState(course.Gate)
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent, 0, 
                         0, 0)
        self.assertCurrentState(course.Pipe1)
        self.assertAIDataValue('foundPipeEarly', True)
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe1)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
        
        # Make sure we are watching for the pipe
        self.assert_(self.visionSystem.pipeLineDetector)
        
class TestPipe1(PipeTestCase):
    def setUp(self):
        PipeTestCase.setUp(self)
        self.machine.start(course.Pipe1)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.Pipe1)
        self.assert_(self.visionSystem.pipeLineDetector)
        
    def testEarlyEnter(self):
        # Stop machine, and restart as if we found the pipe early
        self.machine.stop()
        
        self.ai.data['foundPipeEarly'] = True
        self.machine.start(course.Pipe1)
        
        self.assertCurrentState(course.Pipe1)
        self.assertCurrentBranches([pipe.Seeking])
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, course.Light)
        
class TestLight(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Light)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Light)
        
        self.assertCurrentBranches([light.Searching])
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
        
class TestPipe2(PipeTestCase):
    def setUp(self):
        PipeTestCase.setUp(self, pipe.Dive)
        self.machine.start(course.Pipe2)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.Pipe2)
        #self.assert_(self.visionSystem.pipeLineDetector)
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, course.Bin)
        
class TestBin(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Bin)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Bin)
        
        self.assertCurrentBranches([bin.Dive])
        #self.assert_(self.visionSystem.binDetector)
        
    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(bin.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe3)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(bin.Dive))
        #self.assertFalse(self.visionSystem.binDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.Bin)
        
        # Release timer
        self.releaseTimer(course.Bin.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe3)
        self.assertFalse(self.machine.branches.has_key(bin.Dive))
        #self.assertFalse(self.visionSystem.binDetector)
        
class TestPipe3(PipeTestCase):
    def setUp(self):
        PipeTestCase.setUp(self, pipe.Dive)
        self.machine.start(course.Pipe3)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.Pipe3)
        #self.assert_(self.visionSystem.pipeLineDetector)
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self) # End of course
	self.assertCurrentState(course.PingerDive)

class TestPingerDiver(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(course.PingerDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(course.Pinger)

class TestPinger(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(course.Pinger)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Pinger)
        self.assertCurrentBranches([sonar.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(sonar.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.SafeDive)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Searching))

class TestSafeDive(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(course.SafeDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth,
             motion.pipe.Hover))
        
        self.assertCurrentState(course.SafeDive)
        self.assertCurrentBranches([sonar.Hovering])
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(course.Safe)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Hovering))
        
class TestSafe(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(course.Safe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Safe)
        self.assertCurrentBranches([safe.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Octagaon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(safe.Searching))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.Safe)
        
        # Release timer
        self.releaseTimer(course.Safe.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Octagaon)
        self.assertFalse(self.machine.branches.has_key(safe.Searching))

class TestOctagon(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(course.Octagaon)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth,
             motion.pipe.Hover))
        
        self.assertCurrentState(course.Octagaon)
        self.assertCurrentBranches([sonar.Hovering])
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Hovering))
