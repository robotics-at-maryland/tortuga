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
from ram.test.motion.support import MockTimer

class TestForwardZigZag(support.MotionTest):
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
        
        # Now test the that normal part of the leg completes 
        
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
        
    def testNoEventAfterStop(self):
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        self.motionManager.setMotion(m)
        
        # Register for the leg complete event
        self.legComplete = False
        def handler(event):
            self.legComplete = True
        self.eventHub.subscribeToType(motion.search.ForwardZigZag.LEG_COMPLETE,
                                      handler)
        
        # Start the short leg
        orientation = math.Quaternion(math.Degree(35/2.0),
                                      math.Vector3.UNIT_Z)
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        
        # Stop motion
        self.motionManager.stopCurrentMotion()
        
        mockTimer = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        mockTimer.finish()
        self.qeventHub.publishEvents()
        
        # Make sure we didn't get the event
        self.assert_(mockTimer.started)
        self.assertEqual(3, mockTimer.sleepTime)
        self.assertEquals(False, self.legComplete)
        
    def testDuplicateAtOrientations(self):
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        self.motionManager.setMotion(m)
        
        # Comlete on short leg (with duplicate AT_ORIENTATION events)
        orientation = math.Quaternion(math.Degree(35/2.0),
                                      math.Vector3.UNIT_Z)
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        mockTimerA = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        
        self.controller.publishAtOrientation(orientation)
        self.qeventHub.publishEvents()
        mockTimerB = MockTimer.LOG[motion.search.ForwardZigZag.LEG_COMPLETE]
        
        self.assertEquals(mockTimerA, mockTimerB)

    # TODO: Test not allow repeat events to make it turn or angle again
if __name__ == '__main__':
    unittest.main()
        
