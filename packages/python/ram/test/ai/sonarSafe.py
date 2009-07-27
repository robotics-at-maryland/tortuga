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
import ext.vehicle.device

import ram.motion as motion
import ram.motion.common
import ram.motion.pipe

import ram.test.ai.support as aisupport
import ram.test.ai.sonar as sonarsupport

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
        self.assertCurrentState(sonarSafe.PingerLostSettling)

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
        self.assertCurrentState(sonarSafe.PingerLostDive)

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
        self.assertCurrentState(sonarSafe.PingerLostPreGrabSettling)

        self.assertAlmostEqual(0, self.controller.speed, 3)
 
class TestGrabbing(aisupport.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'safeDepth' : 8,
                    'safeDepthOffset' : 2,
                    },
                },
            'StateMachine' : {
                'States' : {
                    'ram.ai.sonarSafe.Grabbing' : {
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

class TestPingerLostSettling(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerLostSettling)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonarSafe.SafeCloseSeeking)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.Settling)

class TestPingerLostDive(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerLostDive)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonarSafe.Settling)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.Dive)

class TestPingerLostPreGrabSettling(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonarSafe.PingerLostPreGrabSettling)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonarSafe.Settling)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonarSafe.PreGrabSettling)

class TestSafeCloseSeeking(sonarsupport.TransSeekingTestCase,
                           aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonarSafe.SafeCloseSeeking
        self.machine.start(sonarSafe.SafeCloseSeeking)
        
    def testClose(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.9),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        
        # Make sure we enter Settling
        self.assertCurrentState(sonarSafe.Settling)

    def testPingerLost(self):
        # Should not change states
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonarSafe.SafeCloseSeeking)

        self.assertAlmostEqual(0, self.controller.speed, 3)
