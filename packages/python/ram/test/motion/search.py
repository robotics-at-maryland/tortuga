# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest
import math as pmath

# Project Imports
import ext.math as math
import ram.motion as motion
import ram.motion.search
import ram.timer as timer
import ram.test.motion.support as support
from ram.test.motion.support import MockTimer

class TestForwardZigZag(support.MotionTest):
    def testType(self):
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35, 
                                        speed = 8)
        expType = motion.basic.Motion.IN_PLANE | \
            motion.basic.Motion.ORIENTATION
        self.assertEquals(expType, m.type)
    
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
        
        # Complete on short leg
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
        
        # Ensure we only go full time of the rest of the legs
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

    def testDirection(self):
        # Once it is in the correct direction,
        # then it acts like a normal zigZag motion
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35,
                                        speed = 8, direction = 60)

        # Start the motion
        self.motionManager.setMotion(m)

        # Ensure we only turn half way on the first turn and the
        # direction is correct
        expectedYaw = 60 + 35/2.0
        self.assertEqual(expectedYaw, self.controller.yawChange)
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

    def testOppositeDirection(self):
        # Once it is in the correct direction, 
        # then it acts like a normal zigZag motion
        m = motion.search.ForwardZigZag(legTime = 6, sweepAngle = 35,
                                        speed = 8, direction = -60)

        # Start the motion
        self.motionManager.setMotion(m)

        # Ensure we only turn half way on the first turn and the
        # direction is correct
        expectedYaw = -60 - 35/2.0
        self.assertEqual(expectedYaw, self.controller.yawChange)
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

class TestZigZag(support.MotionTest):
    def testType(self):
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        expType = motion.basic.Motion.IN_PLANE
        self.assertEquals(expType, m.type)
    
    def testStraightSpeeds(self):
        self.vehicle.orientation = math.Quaternion.IDENTITY;
        
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 0, speed = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(5, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        
    def testRightLeftSpeeds(self):
        # Vehicle pointed striagh ahead
        self.vehicle.orientation = math.Quaternion.IDENTITY
        
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 180, speed = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(5, self.controller.sidewaysSpeed, 5)
        
        
    def testNormal(self):
        self.vehicle.orientation = math.Quaternion.IDENTITY
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 90, speed = 8)
        
        # Start the motion
        self.motionManager.setMotion(m)
        
        # Ensure we only turn half way on the first turn
        expectedSpeed = pmath.sqrt(2)/2.0 * 8
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(expectedSpeed, self.controller.sidewaysSpeed, 6)
        
        # Ensure we only go half time of the first leg and at the right speed
        mockTimer = MockTimer.LOG[motion.search.ZigZag.LEG_COMPLETE]
        self.assert_(mockTimer.started)
        self.assertEqual(3, mockTimer.sleepTime)

        mockTimer.finish()
        self.qeventHub.publishEvents()

        # We should be facing the opposite direction this time
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(-1 * expectedSpeed, 
                               self.controller.sidewaysSpeed, 6)
        
        # Ensure we only go full time of the rest of the legs
        mockTimer2 = MockTimer.LOG[motion.search.ZigZag.LEG_COMPLETE]
        self.assertNotEqual(mockTimer, mockTimer2)
        self.assert_(mockTimer2.started)
        self.assertEqual(6, mockTimer2.sleepTime)
    
    def testOrientationUpdate(self):
        """Make sure we update when we get an orientation event"""
        
        self.vehicle.orientation = math.Quaternion.IDENTITY
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 90, speed = 8)
        
        # Start the motion
        self.motionManager.setMotion(m)
        
        orientation = math.Quaternion(math.Degree(-90), math.Vector3.UNIT_Z)
        self.vehicle.publishOrientationUpdate(orientation)
        self.qeventHub.publishEvents()
        
        # Make sure the speeds result from the updated orientation not the
        # starting one
        expectedSpeed = pmath.sqrt(2)/2.0 * 8
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(-expectedSpeed, self.controller.sidewaysSpeed, 6)
        
        self.motionManager.stopCurrentMotion()
        
        # Another test
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 180, speed = 8)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 6)
        self.assertAlmostEqual(8, self.controller.sidewaysSpeed, 6)
        
        # Flip the vehicle around
        orientation = math.Quaternion(math.Degree(180), math.Vector3.UNIT_Z)
        self.vehicle.publishOrientationUpdate(orientation)
        self.qeventHub.publishEvents()
        
        self.assertAlmostEqual(0, self.controller.speed, 6)
        self.assertAlmostEqual(-8, self.controller.sidewaysSpeed, 6)

    
    def testNoEventAfterStop(self):
        # Register for the leg complete event
        self.legComplete = False
        def handler(event):
            self.legComplete = True
        self.eventHub.subscribeToType(motion.search.ZigZag.LEG_COMPLETE,
                                      handler)

        # Create motion (automatically starts leg)
        m = motion.search.ZigZag(legTime = 6, sweepAngle = 35, speed = 8)
        self.motionManager.setMotion(m)
        
        # Stop motion
        self.motionManager.stopCurrentMotion()
        
        # Attempt to fire off
        mockTimer = MockTimer.LOG[motion.search.ZigZag.LEG_COMPLETE]
        mockTimer.finish()
        self.qeventHub.publishEvents()
        
        # Make sure we didn't get the event
        self.assert_(mockTimer.started)
        self.assertEqual(3, mockTimer.sleepTime)
        self.assertEquals(False, self.legComplete)
    
    # TODO: Test not allow repeat events to make it turn or angle again
if __name__ == '__main__':
    unittest.main()
        
