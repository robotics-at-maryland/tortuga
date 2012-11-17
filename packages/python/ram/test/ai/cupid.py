# Copyright (C) 2012 Maryland Robotics Club
# Copyright (C) 2012 Stephen Christian and Jack Lotkowski
# All rights reserved.
#
# Author: Stephen Christian and Jack Lotkowski
# File:  packages/python/ram/test/ai/cupid.py

# Python Imports
import unittest

# Project Imports
import ram.ai.course as course
import ram.ai.cupid as cupid

import ram.motion as motion
import ram.motion.basic

import ram.test.ai.support as support

from ram.test.motion.support import MockTimer

class TestGate(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'windowX' : 0,
                    'windowY' : 0,
                    'windowDepth' : 10,
                    'windowOrientation' : 10,
                    'heartSize' : 'large'
                    },
                },
            'StateMachine' : {
                'States' : {
                    'ram.ai.cupid.Start' : {

                        'distance' : 2,
                    },
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
    
    def testStart(self):
        self.machine.start(cupid.Start)
        
        self.assertCurrentMotion(motion.basic.ChangeDepth)
        
        t = self.motionManager.currentMotion._trajectory
        self.assertEqual(10, t._finalValue)

        self.injectEvent(motion.basic.MotionManager.FINISHED)   
        self.assertCurrentState(cupid.Aim)

    def testAim(self):
        self.machine.start(cupid.Aim)
        
        self.assertCurrentMotion(motion.basic.ChangeDepth)
        
        t = self.motionManager.currentMotion._trajectory
        self.assertEqual(10, t._finalValue)
                
        self.injectEvent(motion.basic.MotionManager.FINISHED)   
        self.assertCurrentState(cupid.Fire)

    def testFire(self):
        self.machine.start(cupid.Fire)
        
        self.injectEvent(cupid.NONE_FOUND)   
        self.assertCurrentState(cupid.Fire)
        
        self.injectEvent(cupid.Fire.FIRED)   
        self.assertCurrentState(cupid.Rise)

        self.assertEqual(1, self.vehicle.torpedoesFired)

    def testRise(self):
        self.machine.start(cupid.Rise)

        self.assertCurrentMotion(motion.basic.ChangeDepth)
        
        t = self.motionManager.currentMotion._trajectory
        self.assertEqual(3.5, t._finalValue)
                
        self.injectEvent(motion.basic.MotionManager.FINISHED)   
        self.assertCurrentState(cupid.SwitchWindow)

    def testSwitchWindow(self):
        self.machine.start(cupid.SwitchWindow)

        self.assertCurrentMotion(motion.basic.ChangeOrientation)
        
        t = self.motionManager.currentMotion._trajectory
        #self.assertEqual(3.5, t._finalValue)
                
        self.injectEvent(motion.basic.MotionManager.FINISHED)   
        self.assertCurrentState(cupid.Aim2)

    def testAim2(self):
        self.machine.start(cupid.Aim2)
        
        self.assertCurrentMotion(motion.basic.ChangeDepth)
        
        t = self.motionManager.currentMotion._trajectory
        self.assertEqual(10, t._finalValue)
                
        self.injectEvent(motion.basic.MotionManager.FINISHED)   
        self.assertCurrentState(cupid.Fire2)

    def testFire2(self):
        self.machine.start(cupid.Fire2)

        self.injectEvent(cupid.NONE_FOUND)   
        self.assertCurrentState(cupid.Fire2)
        
        self.injectEvent(cupid.Fire2.FIRED)   
        self.assertCurrentState(cupid.Rise2)

        self.assertEqual(1, self.vehicle.torpedoesFired)
    
    def testRise2(self):
        self.machine.start(cupid.Rise2)

        self.assertCurrentMotion(motion.basic.ChangeDepth)
        
        t = self.motionManager.currentMotion._trajectory
        self.assertEqual(4, t._finalValue)

if __name__ == '__main__':
    unittest.main()
