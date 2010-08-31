# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/pipe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.sonar as sonar
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device

import ram.motion as motion
import ram.motion.pipe

import ram.test.ai.support as aisupport

class TestStart(aisupport.AITestCase):
    DEPTH = 10
    DIVE_SPEED = 3.2
    
    def setUp(self):
        cfg = {
            'Ai' : {
                'config' : {
                    'sonarDepth' : TestStart.DEPTH,
                },
            },
            'StateMachine' : {
                'States' : {
                    'ram.ai.sonar.Start' : {
                        'diveSpeed' : TestStart.DIVE_SPEED,
                    },
                }
            },
        }
        aisupport.AITestCase.setUp(self, cfg = cfg)
        self.estimator.depth = 0
        self.machine.start(sonar.Start)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
    def testConfig(self):
        m = self.motionManager.currentMotion
        self.assertEqual(TestStart.DEPTH, m.desiredDepth)
        self.assertEqual(TestStart.DIVE_SPEED, m.speed)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(sonar.Searching)

class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.Searching)
    
    def testUpdateLeft(self):
        # To the left of the vehicle
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.5, 0.8, -0.1),
                         pingTimeUSec = 1)
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
        self.assertGreaterThan(self.controller.yawChange, 0)
        
        self.releaseTimer(sonar.Searching.CHANGE)
        self.assertCurrentState(sonar.FarSeeking)
    
    def testUpdateRight(self):
        # To the right
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.8, -0.5, -0.1),
                         pingTimeUSec = 2)
        
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
        self.assertLessThan(self.controller.yawChange, 0)
        
        self.releaseTimer(sonar.Searching.CHANGE)
        self.assertCurrentState(sonar.FarSeeking)
        
class TransSeekingTestCase(object):
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)     
        
    def testUpdateLeft(self):
        # To the left of the vehicle
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(0.5, 0.8, -0.1),
                         pingTimeUSec = 9)
        
        # TODO: Figure out why I have to comment this out
        self.assertGreaterThan(self.controller.speed, 0)
        #self.assertLessThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
                
        self.assertCurrentState(self._myClass)
        
    def testUpdateRight(self):
        # To the right
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.8, -0.5, -0.1),
                         pingTimeUSec = 10)
        
        # TODO: Figure out why I have to comment this out
        self.assertLessThan(self.controller.speed, 0)
        #self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        self.assertCurrentState(self._myClass)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonar.PingerLost)

        self.assertAlmostEqual(0, self.controller.speed, 3)
        
class TestFarSeeking(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.sonar.FarSeeking' : {
                        'maxSpeed' : 7,
                        'sidewaysSpeedGain' : 8,
                    },
                }
            }
        }
        aisupport.AITestCase.setUp(self, cfg = cfg)
        self._myClass = sonar.FarSeeking
        self.machine.start(sonar.FarSeeking)
        
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        hoverMotion = self.motionManager.currentMotion

        self.assertEquals(7, hoverMotion._maxSpeed)
        self.assertEquals(8, hoverMotion._sidewaysSpeedGain)
        
    def testClose(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.8),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        self.assertCurrentState(sonar.CloseSeeking)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonar.PingerLost)

        self.assertAlmostEqual(0, self.controller.speed, 3)
        
class TestCloseSeeking(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonar.CloseSeeking
        self.machine.start(sonar.CloseSeeking)
        
    def testClose(self):
        # Subscribe to end event
        self._complete = False
        def complete(event):
            self._complete = True
        self.qeventHub.subscribeToType(sonar.COMPLETE, complete)
        
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.9),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        
        # Make sure we hit the end state and give off the event
        self.assert_(self.machine.complete)
        self.assert_(self._complete)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonar.PingerLostClose)

        self.assertAlmostEqual(0, self.controller.speed, 3)

class TestHovering(TransSeekingTestCase, aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self._myClass = sonar.Hovering
        self.machine.start(sonar.Hovering)
        
    def testClose(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE, vehicle.SonarEvent, 
                         direction = ext.math.Vector3(-0.1, -0.1, -0.9),
                         pingTimeUSec = 11)
        self.qeventHub.publishEvents()
        self.assertCurrentState(sonar.Hovering)

    def testPingerLost(self):
        self.releaseTimer(sonar.PingerState.TIMEOUT)
        self.assertCurrentState(sonar.PingerLostHovering)

        self.assertAlmostEqual(0, self.controller.speed, 3)

class TestPingerLost(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.PingerLost)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonar.FarSeeking)

class TestPingerLostClose(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.PingerLostClose)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonar.CloseSeeking)

class TestPingerLostHovering(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(sonar.PingerLostHovering)

    def testStart(self):
        self.assertAlmostEqual(0, self.controller.speed, 3)

    def testTimeout(self):
        self.releaseTimer(sonar.PingerLost.TIMEOUT)
        self.assertCurrentState(sonar.Searching)

    def testPingerFound(self):
        self.injectEvent(vehicle.device.ISonar.UPDATE)
        self.assertCurrentState(sonar.Hovering)
