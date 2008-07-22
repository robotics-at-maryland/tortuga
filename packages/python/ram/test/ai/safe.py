# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/safe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.safe as safe 
import ext.core as core
import ext.vision as vision

import ram.motion as motion
import ram.motion.common

import ram.test.ai.support as aisupport

class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.downwardSafeDetector)
        #self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent, 0, 
                         0) #0)
        self.assertCurrentState(safe.Seeking)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.downwardSafeDetector)
        
def safeFoundHelper(self):
    self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                     x = 0.5, y = -0.5)#, angle = math.Degree(15.0))
        
    self.assertLessThan(self.controller.speed, 0)
    self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
    self.assertAlmostEqual(0, self.controller.yawChange, 3)

class TestSeeking(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.common.Hover)
        
    def testPipeFound(self):
        """Make sure new found events move the vehicle"""
        safeFoundHelper(self)
        self.assertCurrentState(safe.Seeking)
    
    def testSafePipeLost(self):
        """Make sure losing the light goes back to search"""
        self.injectEvent(vision.EventType.SAFE_LOST)
        self.assertCurrentState(safe.Searching)
        
    def testCentering(self):
        self.assertCurrentState(safe.Seeking)
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                         x = 0.1, y = -0.1)#, angle = math.Degree(15.0))
        self.qeventHub.publishEvents()
        self.assertCurrentState(safe.Dive)
        
class TestDive(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Dive)
        
    def testPipeFound(self):
        safeFoundHelper(self)
        
    def testStart(self):
        self.assertCurrentMotion((motion.common.Hover, 
                                  ram.motion.basic.RateChangeDepth, None))
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assertCurrentState(safe.Offseting)
        
class TestOffseting(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Offseting)

    def testPipeFound(self):
        # Safe above us
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                     x = 0.5, y = -0.5)#, angle = math.Degree(15.0))
        
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        # Make sure event when  is centered we still want to move
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                         x = 0.5, y = -0.7)#, angle = math.Degree(15.0))
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        # Safe below us
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                         x = 0.5, y = -0.9)#, angle = math.Degree(15.0))
        
        self.assertLessThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        self.assertCurrentState(safe.Offseting)
        
    def testCentering(self):
        self.assertCurrentState(safe.Offseting)
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                         x = 0.1, y = -0.75)#, angle = math.Degree(15.0))
        self.qeventHub.publishEvents()
        self.assertCurrentState(safe.Settling)
        
        
class TestSettling(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Settling)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(safe.Settling)
        
        self.assertCurrentMotion(motion.common.Hover)
        
        self.releaseTimer(safe.Settling.SETTLED)
        self.assertCurrentState(safe.Grabbing)
        
    def testSafeFound(self):
        """Make sure the loop back works"""
        
        # Offset centering test
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent,0,0, 
                         x = 0.5, y = -0.7)#, angle = math.Degree(15.0))
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(safe.Settling.SETTLED)
        self.assertCurrentState(safe.Grabbing)
        
class TestGrabbing(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(safe.Grabbing)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(safe.Grabbing)
        
        self.assertCurrentMotion(ram.motion.basic.RateChangeDepth)
        
        self.releaseTimer(safe.Grabbing.GRABBED)
        self.assertCurrentState(safe.Surface)
        
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(safe.Grabbing.GRABBED)
        self.assertCurrentState(safe.Surface)
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assertCurrentState(safe.Surface)
        
class TestSurface(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(safe.Surface)
        
    def testStart(self):
        self.assertCurrentMotion(ram.motion.basic.RateChangeDepth)
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
