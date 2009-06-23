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
import ram.ai.sonar as sonar
import ram.ai.sonarSafe as sonarSafe
import ext.core as core
import ext.vision as vision
import ext.vehicle as vehicle

import ram.motion as motion
import ram.motion.common
import ram.motion.pipe

import ram.test.ai.support as aisupport

class GainsTestCase(object):
    def setUp(self, stateName):
        cfg = {
            'StateMachine' : {
                'States' : {
                    stateName : {
                        'maxSpeed' : 10,
                        'maxSidewaysSpeed'  : 9,
                        'speedGain' : 8,
                        'sidewaysSpeedGain' : 7
                    },
                }
            }
        }
        aisupport.AITestCase.setUp(self, cfg = cfg)
        
    def getCurrentMotion(self):
        return self.motionManager.currentMotion        
    def testGains(self):
        # Test config options & gains
        m = self.getCurrentMotion()
        self.assertEqual(10, m._maxSpeed)
        self.assertEqual(9, m._maxSidewaysSpeed)
        self.assertEqual(8, m._speedGain)
        self.assertEqual(7, m._sidewaysSpeedGain)
        
class TestSettling(GainsTestCase, aisupport.AITestCase):
    def setUp(self):
        GainsTestCase.setUp(self, 'ram.ai.sonarSafe.Settling')
        self.machine.start(sonarSafe.Settling)

    def testStart(self):
        # Make sure we are doing the normal pinger state stuff
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testStartTimer(self):
        self.machine.stop()
        self.machine.start(sonarSafe.Settling)

        self.releaseTimer(sonarSafe.Settling.SETTLED)
        self.assertCurrentState(sonarSafe.Dive)

    def testSettle(self):
        self.injectEvent(sonarSafe.Settling.SETTLED)
        self.assertCurrentState(sonarSafe.Dive)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonarSafe.PingerSettling)

        self.assertAlmostEqual(0, self.controller.speed, 3)

class TestDive(GainsTestCase, aisupport.AITestCase):
    def getCurrentMotion(self):
        return self.motionManager.currentMotion[0]
    
    def setUp(self):
        GainsTestCase.setUp(self, 'ram.ai.sonarSafe.Dive')
        self.machine.start(sonarSafe.Dive)

    def testStart(self):
        self.assertCurrentMotion((motion.pipe.Hover, 
                                  ram.motion.basic.RateChangeDepth,
                                  motion.pipe.Hover))
        
    def testFinished(self):
        self.injectEvent(ram.motion.basic.Motion.FINISHED)
        self.assertCurrentState(sonarSafe.PreGrabSettling)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonarSafe.PingerDive)

        self.assertAlmostEqual(0, self.controller.speed, 3)

class TestPreGrabSettling(GainsTestCase, aisupport.AITestCase):
    def setUp(self):
        GainsTestCase.setUp(self, 'ram.ai.sonarSafe.PreGrabSettling')
        self.machine.start(sonarSafe.PreGrabSettling)

    def testStart(self):
        # Make sure we are doing the normal pinger state stuff
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testStartTimer(self):
        self.machine.stop()
        self.machine.start(sonarSafe.PreGrabSettling)

        self.releaseTimer(sonarSafe.PreGrabSettling.SETTLED)
        #self.assertCurrentState(sonarSafe.Grabbing)

    def testSettle(self):
        self.injectEvent(sonarSafe.PreGrabSettling.SETTLED)
        #self.assertCurrentState(sonarSafe.Grabbing)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonarSafe.PingerPreGrabSettling)

        self.assertAlmostEqual(0, self.controller.speed, 3)
 
class TestGrabbing(aisupport.AITestCase):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.sonarSafe.Grabbing' : {
                        'safeDepth' : 8,
                        'depthOffset' : 2,
                        'diveRate' : 0.8,
                    },
                }
            }
        }
        aisupport.AITestCase.setUp(self, cfg = cfg)
        self.vehicle.depth = 5
        self.machine.start(sonarSafe.Grabbing)

    def testConfig(self):
        m = self.motionManager.currentMotion
        self.assertEqual(8 + 2, m._desiredDepth)
        self.assertAlmostEqual(0.8, m._speed, 5)
        
    #def testDive(self):
    #    for i in xrange(0, 300):
    #        self.releaseTimer(motion.basic.RateChangeDepth.NEXT_DEPTH)
    #        self.qeventHub.publishEvents()
    #    self.assertAlmostEqual(10, self.controller.depth)
        
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

class TestPingerSettling(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerSettling)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.Settling)

class TestPingerDive(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerDive)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.Dive)

class TestPingerPreGrabSettling(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerPreGrabSettling)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.PreGrabSettling)
