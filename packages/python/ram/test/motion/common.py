# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/test/motion/pipe.py

"""
Tests for the ram.motion.pipe module
"""

# STD Imports
import unittest

# Project Imports
import ext.math as math

import ram.motion as motion
import ram.motion.common
import ram.test.motion.support as support

#test PIDLoop
class TestPIDLoop(unittest.TestCase):
    def testPIDcase1(self):
        x = 1
        xd = 2
        dt = 1
        dtMin = 0.1
        dtMax = 4
        kp = 10
        kd = 3
        ki = 1
        sum = 3
        xOld = 5
        expectedControl = 20
        actualControl, sum, xOld = motion.common.PIDLoop(x,xd,dt,dtMin,dtMax,kp,kd,ki,sum,xOld)
        self.assertAlmostEquals(actualControl, expectedControl, 3)
        #self.assertAlmostEquals(arg1, arg2, num_decimal_places)
    def testPIDcase2(self):
        x = -4
        xd = 12
        dt = 10
        dtMin = 0.1
        dtMax = 20
        kp = 10
        kd = 4
        ki = 1.1
        sum = 7
        xOld = -50
        expectedControl = 309.9000
        actualControl, sum, xOld = motion.common.PIDLoop(x,xd,dt,dtMin,dtMax,kp,kd,ki,sum,xOld)
        self.assertAlmostEquals(actualControl, expectedControl,3)

class TestHover(support.MotionTest):
    def makeClass(self, *args, **kwags):
        return motion.common.Hover(*args, **kwags)
    
    def makeTarget(self, *args, **kwargs):
        return motion.common.Target(*args, **kwargs)
    
    def testType(self):
        target = motion.common.Target(x = -0.5, y = 0.5)
        m = motion.common.Hover(target = target)
        self.assertEquals(motion.basic.Motion.IN_PLANE, m.type)
    
    def testUpperLeftHover(self):
        # All gains set to 1 (default)
        target = self.makeTarget(x = -0.5, y = 0.5)
        m = self.makeClass(target = target, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(0.5, self.controller.speed, 3)
        self.assertAlmostEqual(-0.5, self.controller.sidewaysSpeed, 3)
        
        # Different gains
        target = self.makeTarget(x = -0.5, y = 0.5)
        m = self.makeClass(target = target, speedGain = 0.5, maxSpeed = 3,
                           sidewaysSpeedGain = 2, maxSidewaysSpeed = 3)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(0.25, self.controller.speed, 3)
        self.assertAlmostEqual(-1, self.controller.sidewaysSpeed, 3)
    
    def testLowerRightHover(self):
        # All gains set to 1 (default)
        target = self.makeTarget(x = 0.25, y = -1)
        m = self.makeClass(target = target, speedGain = 1,
                           sidewaysSpeedGain = 1, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(-1, self.controller.speed, 3)
        self.assertAlmostEqual(0.25, self.controller.sidewaysSpeed, 3)
        
        # Different gains
        target = self.makeTarget(x = 0.25, y = -1)
        m = self.makeClass(target = target, speedGain = 3, 
                           sidewaysSpeedGain = 0.75, maxSpeed = 5,
                           maxSidewaysSpeed = 2)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(-3, self.controller.speed, 3)
        self.assertAlmostEqual(0.1875, self.controller.sidewaysSpeed, 3)
    
    def testStop(self):
        # Move the vehicle
        target = self.makeTarget(x = -0.5, y = 0.5)
        m = self.makeClass(target = target, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(0.5, self.controller.speed, 3)
        self.assertAlmostEqual(-0.5, self.controller.sidewaysSpeed, 3)
        
        # Make sure it stops
        self.motionManager.stopCurrentMotion()
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(0, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
