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
        self.assertCurrentMotion(motion.basic.ChangeDepth)
                
        # Now make sure we go to forward after diving
        self.injectEvent(motion.basic.Motion.FINISHED)   
        self.assertCurrentState(gate.Forward)   
                
        #self.assert_(self.motionManager.stopped)
        
    def testForward(self):
        # Make our timer blocks in the background
        self.timerBlock = True
        
        # Make sure we start driving forward
        self.machine.start(gate.Forward)
        self.assert_(self.controller.speed > 0)
        
        # Now make sure we stop
        self.releaseTimer(self.machine.currentState().timer)
        self.assertEqual(0, self.controller.speed)
        
        self.assertCurrentState(gate.End)
