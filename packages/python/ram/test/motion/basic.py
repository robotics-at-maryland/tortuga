# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest

# Project Imports
import ext.math

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
        
        
    def testStopCurrentMotion(self):
        m = support.MockMotion()
        self.motionManager.setMotion(m)

        self.motionManager.stopCurrentMotion()
        self.assert_(m.stoped)
  
        self.assertEqual(None, self.motionManager.currentMotion)
  
class TestChangeDepth(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
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
                ext.math.Quaternion(ext.math.Radian(ext.math.Degree(i)),
                                    ext.math.Vector3.UNIT_Z))
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
                ext.math.Quaternion(ext.math.Radian(ext.math.Degree(-i)),
                                    ext.math.Vector3.UNIT_Z))
            self.qeventHub.publishEvents()
            
        self.assert_(self.motionFinished)
            
if __name__ == '__main__':
    unittest.main()
        
