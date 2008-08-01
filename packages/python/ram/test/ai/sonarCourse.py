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
import ram.ai.sonarCourse as sonarCourse

import ram.motion as motion
import ram.motion.basic
import ram.motion.pipe

import ram.test.ai.support as support

class TestGate(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(sonarCourse.Gate)
        
    def testStart(self):
        self.assertCurrentState(sonarCourse.Gate)
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
        # Make sure we branched to the right state machine
        self.assertCurrentBranches([gate.Dive])
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(sonarCourse.PingerDive)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Dive))
        
class TestPingerDiver(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 0
        self.machine.start(sonarCourse.PingerDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(sonarCourse.Pinger)

class TestPinger(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.machine.start(sonarCourse.Pinger)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(sonarCourse.Pinger)
        self.assertCurrentBranches([sonar.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(sonar.COMPLETE, sendToBranches = True)
        self.assertCurrentState(sonarCourse.Safe)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Searching))
        
class TestSafe(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(sonarCourse.Safe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(sonarCourse.Safe)
        self.assertCurrentBranches([sonarSafe.Settling])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(sonarCourse.Octagaon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Settling))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(sonarCourse.Safe)
        
        # Release timer
        self.releaseTimer(sonarCourse.Safe.TIMEOUT)
        
        # Test that the timeout worked properly
        self.assertCurrentState(sonarCourse.Octagaon)
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Dive))

class TestOctagon(support.AITestCase):
    def setUp(self):
        support.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(sonarCourse.Octagaon)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentState(sonarCourse.Octagaon)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
