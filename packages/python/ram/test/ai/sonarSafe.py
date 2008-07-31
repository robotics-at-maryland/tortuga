# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/sonarSafe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.safe as safe
import ram.ai.sonarSafe as sonarSafe
import ext.core as core
import ext.vision as vision

import ram.motion as motion
import ram.motion.common
import ram.motion.pipe

import ram.test.ai.support as aisupport


class TestDive(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.Dive)

    def testStart(self):
        self.assertCurrentMotion((motion.pipe.Hover, 
                                  ram.motion.basic.RateChangeDepth,
                                  motion.pipe.Hover))
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assertCurrentState(sonarSafe.Grabbing)

 
class TestGrabbing(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(sonarSafe.Grabbing)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(sonarSafe.Grabbing)
        
        self.assertCurrentMotion(ram.motion.basic.RateChangeDepth)
        
        self.releaseTimer(sonarSafe.Grabbing.GRABBED)
        self.assertCurrentState(sonarSafe.Surface)
        
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(sonarSafe.Grabbing.GRABBED)
        self.assertCurrentState(sonarSafe.Surface)
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assertCurrentState(sonarSafe.Surface)

      
class TestSurface(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.vehicle.depth = 5
        self.machine.start(sonarSafe.Surface)
        
    def testStart(self):
        self.assertCurrentMotion(ram.motion.basic.RateChangeDepth)

    def testComplete(self):
        # Subscribe to end event
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(safe.COMPLETE, complete)
        
        # Start up and inject event
        self.machine.start(sonarSafe.Surface)
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.qeventHub.publishEvents()
        
        # Check to make sure it worked
        self.assert_(self.machine.complete)
        self.assert_(self._complete)
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assert_(self.machine.complete)
