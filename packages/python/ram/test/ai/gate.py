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

from ram.test.motion.support import MockTimer

class TestGate(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'gateDepth' : 7,
                    },
                },
            'StateMachine' : {
                'States' : {
                    'ram.ai.gate.Start' : {
                        'speed' : 2,
                    },
                    'ram.ai.gate.Forward' : {
                        'speed' : 5,
                         'time' : 15
                    }
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
    
    def testDive(self):
        self.machine.start(gate.Start)
        
        # Ensure we actually started diving
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        # Test the config settings
        m = self.motionManager.currentMotion
        self.assertEqual(7, m._desiredDepth)
        self.assertEqual(2, m._speed)
                
        # Now make sure we go to forward after diving
        self.injectEvent(motion.basic.Motion.FINISHED)   
        self.assertCurrentState(gate.Forward)   

        
    def testForward(self):
        # Make sure we start driving forward
        self.machine.start(gate.Forward)
        
        # Check config settings
        self.assertEqual(5, self.controller.speed)
        self.assertEqual(15, MockTimer.LOG[gate.Forward.DONE]._sleepTime)
        
        
        # Now make sure we stop
        self.releaseTimer(gate.Forward.DONE)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we hit the end state
        self.assert_(self.machine.complete)
    
    def testEndEvent(self):
        # Clear anything in the queue
        self.qeventHub.publishEvents()
        
        # Subscribe to end event
        self._complete = False
        def finished(event):
            self._complete = True
        self.qeventHub.subscribeToType(gate.COMPLETE, finished)
            
        # Make sure we get the final event
        self.machine.start(gate.Forward)
        self.releaseTimer(gate.Forward.DONE)
        self.qeventHub.publishEvents()
        self.assert_(self._complete)
