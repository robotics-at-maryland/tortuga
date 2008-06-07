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
import ram.motion.pipe

import ram.test.motion.support as support
import ram.test.motion.common

class TestHover(ram.test.motion.common.TestHover):
    def filterClassArgs(self, kwargs):
        # Do some translation on the args
        pipe = kwargs.get('target', None)
        if pipe is not None:
           del kwargs['target']
           kwargs['pipe'] = pipe
        return kwargs
    
    def makeClass(self, *args, **kwargs):
        kwargs = self.filterClassArgs(kwargs)
        return motion.pipe.Hover(*args, **kwargs)

    def filterTargetArgs(self, kwargs):
        if not kwargs.has_key('relativeAngle'):
            kwargs['relativeAngle'] = 0
        return kwargs
        
    def makeTarget(self, *args, **kwargs):
        kwargs = self.filterTargetArgs(kwargs)
        return motion.pipe.Pipe(*args, **kwargs)

    def testType(self):
        pipe = motion.pipe.Pipe(x = 0, y = 0, relativeAngle = 15)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        expType = motion.basic.Motion.IN_PLANE | motion.basic.Motion.ORIENTATION
        self.assertEquals(expType, m.type)    
    
    def testAngle(self):
        # Dead center, with yawGain = 1
        pipe = motion.pipe.Pipe(x = 0, y = 0, relativeAngle = 15)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(15, self.controller.yawChange, 3)
        
        # Make sure bad yaw gains are protected
        self.assertRaises(TypeError, motion.pipe.Hover, x = 0, y = 0, 
                          relativeAngle = 0, yawGain = 2)
        
        # Test a yaw gain
        pipe = motion.pipe.Pipe(x = 0, y = 0, relativeAngle = 30)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1, yawGain = 0.5)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(15, self.controller.yawChange, 3)
        
        # Turning toward pipe at 30 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
        
        # Pipe at 45 degress left of vehicle's heading
        # Make sure we only rotate relative to the controllers desired 
        # orientation
        pipe = motion.pipe.Pipe(x = 0, y = 0, relativeAngle = 45)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(20, self.controller.yawChange, 3)
    
    def testStop(self):
        # Test standard stop stuff
        ram.test.motion.common.TestHover.testStop(self)
        
        # Move the vehicle
        pipe = motion.pipe.Pipe(x = -0.5, y = 0.5, relativeAngle = 0)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        
        # Make sure it stops
        self.motionManager.stopCurrentMotion()
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
    
class TestFollow(TestHover):
    def makeClass(self, *args, **kwargs):
        kwargs = self.filterClassArgs(kwargs)
        return motion.pipe.Follow(*args, **kwargs)
    
    def testUpperLeftHover(self):
        """Not applicable"""
        pass
    
    def testLowerRightHover(self):
        """Not applicable"""
        pass
    
    def testStraight(self):
        # All gains set to 1 (default)
        pipe = motion.pipe.Pipe(x = 0, y = 0.5, relativeAngle = 0)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(1, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
        
        # Different gains
        pipe = motion.pipe.Pipe(x = 0, y = -1, relativeAngle = 0)
        m = self.makeClass(pipe = pipe, maxSpeed = 4,
                           maxSidewaysSpeed = 2)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(4, self.controller.speed, 3)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 3)
    
    
    def testLeftRight(self):
        # To the left, All gains set to 1 (default)
        pipe = motion.pipe.Pipe(x = -0.25, y = 0.5, relativeAngle = 0)
        m = self.makeClass(pipe = pipe, maxSpeed = 1,
                           maxSidewaysSpeed = 1)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(0.75, self.controller.speed, 3)
        self.assertAlmostEqual(-0.25, self.controller.sidewaysSpeed, 3)
        
        # To the right, Different gains
        pipe = motion.pipe.Pipe(x = 0.25, y = 0.5, relativeAngle = 0)
        m = self.makeClass(pipe = pipe, maxSpeed = 4,
                           maxSidewaysSpeed = 2)
        
        self.motionManager.setMotion(m)
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(3, self.controller.speed, 3)
        self.assertAlmostEqual(0.5, self.controller.sidewaysSpeed, 3)
    