# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest

# Project Imports
import ext.math as math
import ram.motion as motion
import ram.motion.search
import ram.timer as timer
import ram.test.motion.support as support


class MockTimer(timer.Timer):
    LOG = {}
    
    def __init__(self, eventPublisher, eventType, sleepTime):
        timer._origTimer.__init__(self, eventPublisher, eventType, sleepTime)
        
        self.sleepTime = sleepTime
        self.started = False
        
        # Log the timer so we can reference it in our tests
        MockTimer.LOG[eventType] = self
        
    def run(self):
        pass
        
    def start(self):
        self.started = True
        
    def finish(self):
        """
        Fires off the finish event
        """
        self._complete()

class TestForwardZigZag(support.MotionTest):
    def mockSleep(self, seconds):
        self.seconds = seconds
    
    def setUp(self):
        support.MotionTest.setUp(self)
        
        # Replace Timer with out Mock Timer Class
        timer._origTimer = timer.Timer
        timer.Timer = MockTimer
    
    def tearDown(self):
        # Put the original timer class back
        timer.Timer = timer._origTimer
        del timer._origTimer
    
    def testStart(self):
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        
        # Start the motion
        self.motionManager.setMotion(m)
        
        # Ensure we only turn half way on the first turn
        expectedYaw = 35/2.0
        self.assertEqual(expectedYaw , self.controller.yawChange)
        self.assertEqual(0, self.controller.speed)
        
        orientation = math.Quaternion(math.Degree(expectedYaw),
                                      math.Vector3.UNIT_Z)
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        
        # Ensure we only go half time of the first leg and at the right speed
        mockTimer = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        self.assert_(mockTimer.started)
        self.assertEqual(3, mockTimer.sleepTime)
        self.assertEqual(8, self.controller.speed)
        
    def testNormal(self):
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        self.motionManager.setMotion(m)
        
        # Comlete on short leg
        orientation = math.Quaternion(math.Degree(35/2.0),
                                      math.Vector3.UNIT_Z)
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        
        mockTimer = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        mockTimer.finish()
        self.qeventHub.publishEvents()
        
        # Test a normal leg
        
        # Test turn first
        expectedYaw = -35
        self.assertEqual(expectedYaw , self.controller.yawChange)
        self.assertEqual(0, self.controller.speed)
        
        orientation = math.Quaternion(math.Degree(expectedYaw),
                                      math.Vector3.UNIT_Z)
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        
        # Ensure we only go half time of the first leg and at the right speed
        mockTimer = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        self.assert_(mockTimer.started)
        self.assertEqual(6, mockTimer.sleepTime)
        self.assertEqual(8, self.controller.speed)

if __name__ == '__main__':
    unittest.main()
        
