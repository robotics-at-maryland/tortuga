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
import ext.vision as vision

import ram.ai.course as course
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.bin as bin
import ram.ai.sonar as sonar
import ram.ai.sonarSafe as sonarSafe
import ram.ai.safe as safe
import ram.ai.buoySonarCourse as buoySonarCourse

import ram.motion as motion
import ram.motion.basic
import ram.motion.pipe

import ram.test.ai.support as support
from ram.test.motion.support import MockTimer

class TestGate(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(buoySonarCourse.Gate)
        
    def testStart(self):
        self.assertCurrentState(buoySonarCourse.Gate)
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
        # Make sure we branched to the right state machine
        self.assertCurrentBranches([gate.Dive])
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoySonarCourse.Light)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
        
class TestLight(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(buoySonarCourse.Light)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoySonarCourse.Light)
        
        self.assertCurrentBranches([light.Dive])
        #self.assert_(self.visionSystem.redLightDetector)
        
    def testLightHit(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(light.LIGHT_HIT, sendToBranches = True)
        self.assertCurrentState(buoySonarCourse.PingerDive)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(light.Searching))
        self.assertFalse(self.visionSystem.redLightDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(buoySonarCourse.Light)
        
        # Release timer
        self.assertEqual(
            40, MockTimer.LOG[buoySonarCourse.Light.TIMEOUT]._sleepTime)
        self.releaseTimer(buoySonarCourse.Light.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(buoySonarCourse.PingerDive)
        self.assertFalse(self.machine.branches.has_key(light.Dive))
        self.assertFalse(self.visionSystem.redLightDetector)
          
class TestPingerDiver(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(buoySonarCourse.PingerDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(buoySonarCourse.Pinger)

class TestPinger(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(buoySonarCourse.Pinger)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoySonarCourse.Pinger)
        self.assertCurrentBranches([sonar.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(sonar.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoySonarCourse.Safe)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Searching))
        
class TestSafe(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(buoySonarCourse.Safe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(buoySonarCourse.Safe)
        self.assertCurrentBranches([sonarSafe.Settling])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(buoySonarCourse.Octagaon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Settling))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(buoySonarCourse.Safe)
        
        # Release timer
        self.releaseTimer(buoySonarCourse.Safe.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(buoySonarCourse.Octagaon)
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Dive))

class TestOctagon(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(buoySonarCourse.Octagaon)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentState(buoySonarCourse.Octagaon)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
