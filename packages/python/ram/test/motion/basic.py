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
import ext.core as core

import ram.motion as motion
import ram.motion.basic
import ram.test.motion.support as support


class TestMotionManager(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)

    def testSetMotion(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)
        
        self.assertEquals(self.vehicle.getName(), m.vehicle.getName())
        self.assertEquals(self.controller.getName(), m.controller.getName())
        
        m2 = support.MockMotion()
        self.motionManager.setMotion(m2)
        self.assert_(m.stoped)
        self.assertEquals(self.vehicle.getName(), m2.vehicle.getName())
        self.assertEquals(self.controller.getName(), m2.controller.getName())
        
    def testMultiMotion(self):
        m = support.MockMotion()
        mPlane = support.MockMotion(type = motion.basic.Motion.IN_PLANE)
        mDepth = support.MockMotion(type = motion.basic.Motion.DEPTH)
        mOrien = support.MockMotion(type = motion.basic.Motion.ORIENTATION)
        
        _type = motion.basic.Motion.ORIENTATION | motion.basic.Motion.IN_PLANE
        mPlaneOrien = support.MockMotion(type = _type)
        
        # Place an all enclusive motion
        self.motionManager.setMotion(m)
        self.assertEqual(m, self.motionManager.currentMotion)
        
        # Replace with just a depth one
        self.motionManager.setMotion(mPlane)
        self.assertEqual(mPlane, self.motionManager.currentMotion)
        self.assert_(m.stoped)
        self.assert_(mPlane.started)
        
        # Add a depth one and make sure they are both still present
        self.motionManager.setMotion(mDepth)
        self.assertEqual((mPlane,mDepth,None), 
                         self.motionManager.currentMotion)
        self.assert_(mDepth.started)
        
        # Add orientation one
        self.motionManager.setMotion(mOrien)
        self.assertEqual((mPlane,mDepth,mOrien),
                         self.motionManager.currentMotion)
        self.assert_(mOrien.started)


        # Now replace both in plane and depth with a single new motion
        self.motionManager.setMotion(mPlaneOrien)
        self.assertEqual((mPlaneOrien,mDepth,mPlaneOrien),
                         self.motionManager.currentMotion)
        self.assert_(mOrien.stoped)
        self.assert_(mPlane.stoped)
        self.assert_(mPlaneOrien.started)
        
        # Now stop that multimotion by just doing a single type stop
        self.motionManager.stopMotionOfType(motion.basic.Motion.IN_PLANE)
        self.assertEqual(mDepth, self.motionManager.currentMotion)
        self.assert_(mPlaneOrien.stoped)
        
        
        # Now make sure a single multi motion shows up like that
        self.motionManager.stopCurrentMotion()
        mPlaneOrien = support.MockMotion(type = _type)
        self.motionManager.setMotion(mPlaneOrien)
        self.assertEqual(mPlaneOrien, self.motionManager.currentMotion)
        
    def testQueuedMotions(self):
        def _handler(event):
            self._finished = True

        self._finished = False
        self.qeventHub.subscribeToType(
            motion.basic.MotionManager.FINISHED, _handler)

        m1 = motion.basic.RateChangeDepth(9, 0.3)
        m2 = motion.basic.MoveDirection(0, 3)
        
        self.motionManager.setMotion(m1, m2)

        # Make sure it started the first motion
        self.assertEqual(m1, self.motionManager.currentMotion)
        self.assertFalse(self._finished)
        
        # Finish the motion
        m1._finish()
        self.qeventHub.publishEvents()
        self.assertEqual(m2, self.motionManager.currentMotion)
        self.assertFalse(self._finished)

        # Now finish the second motion and make sure it publishes the event
        m2._finish()
        self.qeventHub.publishEvents()
        self.assertEqual(None, self.motionManager.currentMotion)
        self.assert_(self._finished)

    def testStopCurrentMotion(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)

        self.motionManager.stopCurrentMotion()
        self.assert_(m.stoped)
  
        self.assertEqual(None, self.motionManager.currentMotion)
        
    def testMotionFinished(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)
        
        self.assertEqual(m, self.motionManager.currentMotion)
        
        event = core.Event()
        event.motion = m
        self.motionManager.publish(motion.basic.Motion.FINISHED, event)
        self.qeventHub.publishEvents()
        
        self.assertEqual(None, self.motionManager.currentMotion)
  
class TestChangeDepth(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testType(self):
        m = motion.basic.ChangeDepth(10, 5) 
        self.assertEqual(motion.basic.Motion.DEPTH, m.type)
        
    def testDive(self):
        self.vehicle.depth = 5
        m = motion.basic.ChangeDepth(10, 5) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6,11):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertEqual(i, self.controller.depth)
            # Say we have reached the depth to keep going
            self.controller.publishAtDepth(i)
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
    def testSurface(self):
        self.vehicle.depth = 10
        m = motion.basic.ChangeDepth(5, 5) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                      self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in reversed(xrange(5,10)):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertEqual(i, self.controller.depth)
            # Say we have reached the depth to keep going
            self.controller.publishAtDepth(i)
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)

class TestRateChangeDepth(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testType(self):
        m = motion.basic.RateChangeDepth(10, 5) 
        self.assertEqual(motion.basic.Motion.DEPTH, m.type)
        
    def testDive(self):
        self.vehicle.depth = 5
        
        # Go to ten units, at 1 unit a second, with a 10Hz update rate
        m = motion.basic.RateChangeDepth(desiredDepth = 11, speed=2, rate = 10) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.basic.RateChangeDepth.NEXT_DEPTH]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check fifty steps
        expectedDepth = 5
        for i in xrange(0, 30):
            expectedDepth += 0.2
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDepth, self.controller.depth)
        self.assertAlmostEqual(11, self.controller.depth, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(11, self.controller.depth, 3)
        self.assertEqual(True, self.motionFinished)

    def testSurface(self):
        self.vehicle.depth = 10
        
        # Go to ten units, at 1 unit a second, with a 10Hz update rate
        m = motion.basic.RateChangeDepth(desiredDepth = 4, speed=2, rate = 10) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.basic.RateChangeDepth.NEXT_DEPTH]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check fifty steps
        expectedDepth = 10
        for i in xrange(0, 30):
            expectedDepth -= 0.2
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDepth, self.controller.depth)
        self.assertAlmostEqual(4, self.controller.depth, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(4, self.controller.depth, 3)
        self.assertEqual(True, self.motionFinished)
        
class TestChangeHeading(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testLeft(self):
        m = motion.basic.ChangeHeading(30, 5) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6, 36, 6):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertAlmostEqual(6, self.controller.yawChange, 2)
            # Say we have reached the depth to keep going
            self.controller.publishAtOrientation(
                math.Quaternion(math.Radian(math.Degree(i)),
                                    math.Vector3.UNIT_Z))
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
            
    def testRight(self):
        m = motion.basic.ChangeHeading(-30, 5) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # First step
        self.motionManager.setMotion(m)
        
        for i in xrange(6, 36, 6):
            # Make sure we didn't finish early
            self.assert_(not self.motionFinished)
            # Make sure the proper depth was commanded
            self.assertAlmostEqual(-6, self.controller.yawChange, 2)
            # Say we have reached the depth to keep going
            self.controller.publishAtOrientation(
                math.Quaternion(math.Radian(math.Degree(-i)),
                                    math.Vector3.UNIT_Z))
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
class TestRateChangeHeading(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def _getControllerHeading(self):
        return self.controller.desiredOrientation.getYaw(True).valueDegrees()
        
    def testType(self):
        m = motion.basic.RateChangeHeading(50, 5) 
        self.assertEqual(motion.basic.Motion.ORIENTATION, m.type)
        
    def testLeft(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(30),
                                                   math.Vector3.UNIT_Z)
        
        # Go to 60 degrees, at 10 degrees a second, with a 10Hz update rate
        m = motion.basic.RateChangeHeading(desiredHeading = 60, speed = 10, 
                                           rate = 10) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.basic.RateChangeHeading.NEXT_HEADING]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check thirty steps
        expectedHeading = 30
        for i in xrange(0, 30):
            expectedHeading += 1
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertAlmostEqual(expectedHeading, 
                                   self._getControllerHeading(), 3)
        self.assertAlmostEqual(60, self._getControllerHeading(), 1)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(60, self._getControllerHeading(), 1)
        self.assertEqual(True, self.motionFinished)

    def testRight(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(10),
                                                   math.Vector3.UNIT_Z)
        
        # Go to 60 degrees, at 10 degrees a second, with a 10Hz update rate
        m = motion.basic.RateChangeHeading(desiredHeading = -50, speed = 10, 
                                           rate = 10) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.basic.RateChangeHeading.NEXT_HEADING]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check thirty steps
        expectedHeading = 10
        for i in xrange(0, 60):
            expectedHeading -= 1
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertAlmostEqual(expectedHeading, 
                                   self._getControllerHeading(), 3)
        self.assertAlmostEqual(-50, self._getControllerHeading(), 1)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(-50, self._getControllerHeading(), 1)
        self.assertEqual(True, self.motionFinished)

    def testNoTurn(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(10),
                                                   math.Vector3.UNIT_Z)
        
        # Go to 10 degrees, at 10 degrees a second, with a 10Hz update rate
        m = motion.basic.RateChangeHeading(desiredHeading = 10, speed = 10, 
                                           rate = 10) 
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        self.qeventHub.publishEvents()

        self.assertAlmostEqual(10, self._getControllerHeading(), 1)
        self.assertEqual(True, self.motionFinished)
        
    def testRelativeTurn(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(30),
                                                   math.Vector3.UNIT_Z)
        
        # Go to 60 degrees, at 10 degrees a second, with a 10Hz update rate
        m = motion.basic.RateChangeHeading(desiredHeading = 30, speed = 10, 
                                           rate = 10, absolute = False)
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.basic.RateChangeHeading.NEXT_HEADING]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check thirty steps
        expectedHeading = 30
        for i in xrange(0, 30):
            expectedHeading += 1
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertAlmostEqual(expectedHeading, 
                                   self._getControllerHeading(), 3)
        self.assertAlmostEqual(60, self._getControllerHeading(), 1)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(60, self._getControllerHeading(), 1)
        self.assertEqual(True, self.motionFinished)
          
class TestMoveDirection(support.MotionTest):
    def makeClass(self, *args, **kwargs):
        return motion.basic.MoveDirection(*args, **kwargs)
    
    def testType(self):
        m = self.makeClass(desiredHeading = 35, speed = 8)
        expType = motion.basic.Motion.IN_PLANE
        self.assertEquals(expType, m.type)
    
    def testStraight(self):
        self.vehicle.orientation = math.Quaternion.IDENTITY;
        
        m = self.makeClass(desiredHeading = 0, speed = 5)
        self.motionManager.setMotion(m)
        
        self.assertEqual(5, self.controller.speed)
        self.assertEqual(0, self.controller.sidewaysSpeed)
        
    def testRight(self):
        # Vehicle pointed striagh ahead
        self.vehicle.orientation = math.Quaternion.IDENTITY
        
        m = self.makeClass(desiredHeading = 90, speed = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 4)
        self.assertAlmostEqual(-5, self.controller.sidewaysSpeed, 4)
        
    def testOffset(self):
        # Vehicle pointed 30 degrees left
        self.vehicle.orientation = math.Quaternion(math.Degree(30),
                                                   math.Vector3.UNIT_Z)
        
        # Move in a direction 45 degrees left
        m = self.makeClass(desiredHeading = 75, speed = 5)
        self.motionManager.setMotion(m)
        
        expectedSpeed = pmath.sqrt(2)/2.0 * 5
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 4)
        self.assertAlmostEqual(-expectedSpeed, self.controller.sidewaysSpeed,4)

    def testOrientationUpdate(self):
        """Make sure we update when we get an orientation event"""
        
        self.vehicle.orientation = math.Quaternion.IDENTITY
        
        m = self.makeClass(desiredHeading = -90, speed = 6)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(6, self.controller.sidewaysSpeed, 5)

        # Change vehicle orientation
        orientation = math.Quaternion(math.Degree(-45), math.Vector3.UNIT_Z)
        self.vehicle.publishOrientationUpdate(orientation)
        self.qeventHub.publishEvents()
        
        # Make sure the speeds result from the updated orientation not the
        # starting one
        expectedSpeed = pmath.sqrt(2)/2.0 * 6
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(expectedSpeed, self.controller.sidewaysSpeed, 6)
        
        self.motionManager.stopCurrentMotion()
    
    def testRelativeDirection(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(60),
                                          math.Vector3.UNIT_Z);
        
        m = self.makeClass(desiredHeading = 0, speed = 5, absolute = False)
        self.motionManager.setMotion(m)
        
        self.assertEqual(5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        m = self.makeClass(desiredHeading = -180, speed = 5, absolute = False)
        self.motionManager.setMotion(m)

        self.assertEqual(-5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        
    def testDirectionAfterTurn(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(60),
                                                   math.Vector3.UNIT_Z)
        
        m = self.makeClass(desiredHeading = 0, speed = 5, absolute = False)
        self.motionManager.setMotion(m)
        
        self.assertEqual(5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        # Now turn the vehicle 90 degrees and make sure it's still heading
        # the same direction
        self.vehicle.orientation = math.Quaternion(math.Degree(150),
                                                   math.Vector3.UNIT_Z)
        self.vehicle.publishOrientationUpdate(self.vehicle.orientation)
        self.qeventHub.publishEvents()
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertEqual(5, self.controller.sidewaysSpeed)

class TestTimedMoveDirection(TestMoveDirection):
    def makeClass(self, *args, **kwargs):
        if not kwargs.has_key('duration'):
            kwargs['duration'] = 0
        return motion.basic.TimedMoveDirection(*args, **kwargs)
    
    def testTiming(self):
        self.finished = False
        def handler(event):
            self.finished = True
        self.eventHub.subscribeToType(motion.basic.Motion.FINISHED,
                                      handler)
        
        # Start it up
        self.vehicle.orientation = math.Quaternion.IDENTITY
        m = self.makeClass(desiredHeading = -45, speed = 3, duration = 5.5)
        self.motionManager.setMotion(m)
        self.assertFalse(self.finished)
        
        # Ensure speeds our present
        expectedSpeed = pmath.sqrt(2)/2.0 * 3
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(expectedSpeed, self.controller.sidewaysSpeed, 6)
        
        # Check Timer
        mockTimer = \
            support.MockTimer.LOG[motion.basic.TimedMoveDirection.COMPLETE]
        self.assertFalse(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 5.5)
        
        # Finish
        mockTimer.finish()
        self.qeventHub.publishEvents()
        
        self.assertEqual(0, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed)
        self.assert_(self.finished)

class TestMoveDistance(support.MotionTest):
    def makeClass(self, *args, **kwargs):
        return motion.basic.MoveDistance(*args, **kwargs)
    
    def testType(self):
        m = self.makeClass(desiredHeading = 35, speed = 8, distance = 5)
        expType = motion.basic.Motion.IN_PLANE
        self.assertEquals(expType, m.type)
    
    def testStraight(self):
        self.vehicle.orientation = math.Quaternion.IDENTITY;
        
        m = self.makeClass(desiredHeading = 0, speed = 5, distance = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(5, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        
    def testRight(self):
        # Vehicle pointed straight ahead
        self.vehicle.orientation = math.Quaternion.IDENTITY
        
        m = self.makeClass(desiredHeading = 90, speed = 5, distance = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 4)
        self.assertAlmostEqual(-5, self.controller.sidewaysSpeed, 4)
        
    def testOffset(self):
        # Vehicle pointed 30 degrees left
        self.vehicle.orientation = math.Quaternion(math.Degree(30),
                                                   math.Vector3.UNIT_Z)
        self.vehicle.position = math.Vector2(0, 0)
        
        # Move in a direction 45 degrees left
        m = self.makeClass(desiredHeading = 75, speed = 5, distance = 5)
        self.motionManager.setMotion(m)
        
        expectedSpeed = pmath.sqrt(2)/2.0 * 5
        self.assertAlmostEqual(expectedSpeed, self.controller.speed, 4)
        self.assertAlmostEqual(-expectedSpeed, self.controller.sidewaysSpeed,4)

    def testPositionUpdate(self):
        """ Make sure the motion only finishes on an update at the target """

        self.vehicle.position = math.Vector2(0, 0)
        self.vehicle.orientation = math.Quaternion.IDENTITY
        
        m = self.makeClass(desiredHeading = -90, speed = 6, distance = 5)
        self.motionManager.setMotion(m)
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(6, self.controller.sidewaysSpeed, 5)

        # Change vehicle position
        position = math.Vector2(2.5, 2.5)
        self.vehicle.publishPositionUpdate(position)
        self.qeventHub.publishEvents()
        
        # Make sure the speeds result from the updated position not the
        # starting one
        expectedSpeed = pmath.sqrt(2)/2.0 * 6
        self.assertAlmostEqual(-expectedSpeed, self.controller.speed, 6)
        self.assertAlmostEqual(expectedSpeed, self.controller.sidewaysSpeed, 6)
        
        self.motionManager.stopCurrentMotion()

    def testUnitVector(self):
        # Create an instance of MoveDistance so we can test the function
        # Parameters don't matter
        m = self.makeClass(desiredHeading = 0, speed = 6, distance = 5)

        # constants
        sq3_2 = pmath.sqrt(3)/2.0
        sq2_2 = pmath.sqrt(2)/2.0
        # Test all major degrees, degrees based on right hand rule

        # 0 degrees = north
        vector = m._unitvector(0)
        expected = math.Vector2(0.0, 1.0)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 30 degrees = NNW
        vector = m._unitvector(30)
        expected = math.Vector2(-0.5, sq3_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 45 degrees = NW
        vector = m._unitvector(45)
        expected = math.Vector2(-sq2_2, sq2_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 60 degrees = WNW
        vector = m._unitvector(60)
        expected = math.Vector2(-sq3_2, 0.5)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 90 degrees = west
        vector = m._unitvector(90)
        expected = math.Vector2(-1.0, 0.0)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 120 degrees = WSW
        vector = m._unitvector(120)
        expected = math.Vector2(-sq3_2, -0.5)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 135 degrees = SW
        vector = m._unitvector(135)
        expected = math.Vector2(-sq2_2, -sq2_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 150 degrees = SSW
        vector = m._unitvector(150)
        expected = math.Vector2(-0.5, -sq3_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # 180 degrees = south
        vector = m._unitvector(180)
        expected = math.Vector2(0.0, -1.0)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -150 degrees = SSE
        vector = m._unitvector(-150)
        expected = math.Vector2(0.5, -sq3_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -135 degrees = SE
        vector = m._unitvector(-135)
        expected = math.Vector2(sq2_2, -sq2_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -120 degrees = ESE
        vector = m._unitvector(-120)
        expected = math.Vector2(sq3_2, -0.5)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -90 degrees = east
        vector = m._unitvector(-90)
        expected = math.Vector2(1.0, 0.0)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -60 degrees = ENE
        vector = m._unitvector(-60)
        expected = math.Vector2(sq3_2, 0.5)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -45 degrees = NE
        vector = m._unitvector(-45)
        expected = math.Vector2(sq2_2, sq2_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

        # -30 degrees = NNE
        vector = m._unitvector(-30)
        expected = math.Vector2(0.5, sq3_2)
        self.assertAlmostEqual(vector.x, expected.x, 5)
        self.assertAlmostEqual(vector.y, expected.y, 5)

    def testDirection(self):
        """
        Check that the vehicle chooses the correct desired position
        """

        self.vehicle.position = math.Vector2(0, 0)
        self.vehicle.orientation = math.Quaternion(math.Degree(-45),
                                                   math.Vector3.UNIT_Z)

        # Moving forward
        m = self.makeClass(desiredHeading = -45, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        root = pmath.sqrt(2)/2.0
        expectedPosition = math.Vector2(root * 5.0, root * 5.0)
        self.assertAlmostEqual(expectedPosition.x, m._desiredPosition.x, 5)
        self.assertAlmostEqual(expectedPosition.y, m._desiredPosition.y, 5)
        
        self.assertAlmostEqual(6, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        m = self.makeClass(desiredHeading = -45, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        root = pmath.sqrt(2)/2.0
        expectedPosition = math.Vector2(root * 5.0, root * 5.0)
        self.assertAlmostEqual(expectedPosition.x, m._desiredPosition.x, 5)
        self.assertAlmostEqual(expectedPosition.y, m._desiredPosition.y, 5)
        
        self.assertAlmostEqual(6, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        # Moving backward
        m = self.makeClass(desiredHeading = 135, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        expectedPosition = math.Vector2(root * -5.0, root * -5.0)
        self.assertAlmostEqual(expectedPosition.x, m._desiredPosition.x, 5)
        self.assertAlmostEqual(expectedPosition.y, m._desiredPosition.y, 5)
        
        self.assertAlmostEqual(-6, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        # Moving left
        m = self.makeClass(desiredHeading = 45, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        expectedPosition = math.Vector2(root * -5.0, root * 5.0)
        self.assertAlmostEqual(expectedPosition.x, m._desiredPosition.x, 5)
        self.assertAlmostEqual(expectedPosition.y, m._desiredPosition.y, 5)
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(-6, self.controller.sidewaysSpeed, 5)

        # Moving right
        m = self.makeClass(desiredHeading = -135, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        expectedPosition = math.Vector2(root * 5.0, root * -5.0)
        self.assertAlmostEqual(expectedPosition.x, m._desiredPosition.x, 5)
        self.assertAlmostEqual(expectedPosition.y, m._desiredPosition.y, 5)
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(6, self.controller.sidewaysSpeed, 5)

    def testFinish(self):
        """ Make sure that reaching the destination finishes the motion """

        self._finished = False
        # Create a function to be called to ensure the motion was finished
        def _handler(event):
            self._finished = True

        # Subscribe to the Motion.FINISHED event
        self.eventHub.subscribeToType(
            motion.basic.Motion.FINISHED, _handler)

        self.vehicle.position = math.Vector2(0, 0)
        self.vehicle.orientation = math.Quaternion.IDENTITY

        m = self.makeClass(desiredHeading = 0, speed = 6, distance = 5)
        self.motionManager.setMotion(m)

        # Make sure the motion hasn't already finished
        self.assert_(not self._finished)

        self.assertAlmostEqual(6, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        # Change the position and publish an update
        position = math.Vector2(0, 5)
        self.vehicle.publishPositionUpdate(position)
        self.qeventHub.publishEvents()

        # Check that the vehicle is not moving and the motion is finished
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assert_(self._finished)

    def testRelativeDirection(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(60),
                                          math.Vector3.UNIT_Z)
        
        m = self.makeClass(desiredHeading = 0, speed = 5,
                           distance = 5, absolute = False)
        self.motionManager.setMotion(m)
        
        self.assertEqual(5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        m = self.makeClass(desiredHeading = -180, speed = 5,
                           distance = 5, absolute = False)
        self.motionManager.setMotion(m)

        self.assertEqual(-5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        
    def testDirectionAfterTurn(self):
        self.vehicle.orientation = math.Quaternion(math.Degree(60),
                                                   math.Vector3.UNIT_Z)
        
        m = self.makeClass(desiredHeading = 0, speed = 5,
                           distance = 5, absolute = False)
        self.motionManager.setMotion(m)
        
        self.assertEqual(5, self.controller.speed)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)

        # Now turn the vehicle 90 degrees and make sure it's still heading
        # the same direction
        self.vehicle.orientation = math.Quaternion(math.Degree(150),
                                                   math.Vector3.UNIT_Z)
        self.vehicle.publishOrientationUpdate(self.vehicle.orientation)
        self.vehicle.publishPositionUpdate(self.vehicle.position)
        self.qeventHub.publishEvents()
        
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertEqual(5, self.controller.sidewaysSpeed)
            
if __name__ == '__main__':
    unittest.main()
        
