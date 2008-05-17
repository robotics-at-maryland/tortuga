# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/gate.py

# Python Imports
import unittest

# Project Imports
import ram.ai.gate as gate

import ram.motion as motion
import ram.motion.basic

import ram.test.ai.support as support

class TestGate(support.AITestCase):
    def testDive(self):
        self.machine.start(gate.Dive)
        
        # Ensure we actually started diving
        self.assertEquals(motion.basic.ChangeDepth, 
                          type(self.motionManager.currentMotion))
        
        # Now make sure we go to forward after diving
        self.injectEvent(motion.basic.Motion.FINISHED)      
        self.assertEquals(gate.Forward, type(self.machine.currentState()))
        
    def testForward(self):
        # Make our timer blocks in the background
        self.block = True
        
        # Make sure we start driving forward
        self.machine.start(gate.Forward)
        self.assert_(self.controller.speed > 0)
        
        # Now make sure we stop
        self.releaseTimer(self.machine.currentState().timer)
        self.assertEqual(0, self.controller.speed)