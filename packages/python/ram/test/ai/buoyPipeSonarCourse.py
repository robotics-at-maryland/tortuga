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
# TODO: Clean up these imports
import ext.math as math
import ext.vision as vision

import ram.ai.course as course
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.bin as bin
import ram.ai.sonar as sonar
import ram.ai.sonarSafe as sonarSafe
import ram.ai.safe as safe
import ram.ai.buoyPipeSonarCourse as buoyPipeSonarCourse

import ram.motion as motion
import ram.motion.basic
import ram.motion.pipe

import ram.test.ai.support as support
from ram.test.motion.support import MockTimer

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
        self.machine.start(buoyPipeSonarCourse.Gate)
        
    def testStart(self):
        self.assertCurrentState(buoyPipeSonarCourse.Gate)
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
        # Make sure we branched to the right state machine
        self.assertCurrentBranches([gate.Dive])
        
    def testStoreDesiredQuaternion(self):
        # Setup a desired orientation
        expected = math.Quaternion(math.Degree(45), math.Vector3.UNIT_Z)
        self.controller.desiredOrientation = expected
        
        # Restart state machine so it loads the orientation
        self.machine.stop()
        self.machine.start(buoyPipeSonarCourse.Gate)
        
        # Make sure we have the desired quaterion saved properly
        self.assertAIDataValue('gateOrientation', expected)
        
    # Test transitions
    def testPipeOn(self):
        self.releaseTimer(buoyPipeSonarCourse.Gate.PIPE_ON)
        self.assert_(self.visionSystem.pipeLineDetectorOn)
        
    def testFoundPipe(self):
        self.injectEvent(vision.EventType.PIPE_FOUND)
        self.assertCurrentState(buoyPipeSonarCourse.Pipe)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoyPipeSonarCourse.Light)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
     
class TestPipe(PipeTestCase):
    def setUp(self):
        PipeTestCase.setUp(self, pipe.Dive)
        self.machine.start(buoyPipeSonarCourse.Pipe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, buoyPipeSonarCourse.Pipe)
        #self.assert_(self.visionSystem.pipeLineDetector)
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, buoyPipeSonarCourse.Light)
        
    def testPipeLost(self):
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(buoyPipeSonarCourse.Pipe)
        
        self.qeventHub.publishEvents()
        self.assertCurrentState(buoyPipeSonarCourse.Light)
        self.assertEqual(expected, self.controller.desiredOrientation)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(buoyPipeSonarCourse.Pipe)
        
        # Release timer
        self.assertEqual(
            20, MockTimer.LOG[buoyPipeSonarCourse.Pipe.TIMEOUT]._sleepTime)
        self.releaseTimer(buoyPipeSonarCourse.Pipe.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(buoyPipeSonarCourse.Light)
        
        #self.qeventHub.publishEvents()
        #self.assertCurrentState(buoyPipeSonarCourse.Light)
        self.assertFalse(self.machine.branches.has_key(pipe.Dive))
        self.assertFalse(self.visionSystem.pipeLineDetector)
        self.assertEqual(expected, self.controller.desiredOrientation)
        
class TestLight(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(buoyPipeSonarCourse.Light)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoyPipeSonarCourse.Light)
        
        self.assertCurrentBranches([light.Dive])
        #self.assert_(self.visionSystem.redLightDetector)
        
    def testLightHit(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(light.LIGHT_HIT, sendToBranches = True)
        self.assertCurrentState(buoyPipeSonarCourse.PingerDive)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(light.Searching))
        self.assertFalse(self.visionSystem.redLightDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(buoyPipeSonarCourse.Light)
        
        # Release timer
        self.assertEqual(
            40, MockTimer.LOG[buoyPipeSonarCourse.Light.TIMEOUT]._sleepTime)
        self.releaseTimer(buoyPipeSonarCourse.Light.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(buoyPipeSonarCourse.PingerDive)
        self.assertFalse(self.machine.branches.has_key(light.Dive))
        self.assertFalse(self.visionSystem.redLightDetector)
          
class TestPingerDiver(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(buoyPipeSonarCourse.PingerDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(buoyPipeSonarCourse.Pinger)

class TestPinger(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(buoyPipeSonarCourse.Pinger)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoyPipeSonarCourse.Pinger)
        self.assertCurrentBranches([sonar.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(sonar.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoyPipeSonarCourse.Safe)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Searching))
        
class TestSafe(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(buoyPipeSonarCourse.Safe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoyPipeSonarCourse.Safe)
        self.assertCurrentBranches([sonarSafe.Settling])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoyPipeSonarCourse.Octagaon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Settling))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(buoyPipeSonarCourse.Safe)
        
        # Release timer
        self.releaseTimer(buoyPipeSonarCourse.Safe.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(buoyPipeSonarCourse.Octagaon)
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Dive))

class TestOctagon(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(buoyPipeSonarCourse.Octagaon)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentState(buoyPipeSonarCourse.Octagaon)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
