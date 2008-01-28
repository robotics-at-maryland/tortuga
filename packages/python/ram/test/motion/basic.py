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
  
  
class TestChangeDepth(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testDive(self):
        self.vehicle.depth = 5
        m = motion.ChangeDepth(10, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
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
        m = motion.ChangeDepth(5, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
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
        
class TestChangeHeading(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
        
    def testLeft(self):
        m = motion.ChangeHeading(30, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
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
        m = motion.ChangeHeading(-30, 5) 
        self.qeventHub.subscribeToType(motion.Motion.FINISHED, 
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
        
