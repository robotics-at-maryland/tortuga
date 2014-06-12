# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/safe.py

# STD Imports
import unittest
import math as pmath

# Project Imports
import ext.math as math

import ram.motion as motion
import ram.motion.safe
import ram.test.motion.support as support

class TestOffsettingHover(support.MotionTest):
    def setUp(self):
        support.MotionTest.setUp(self)
        self.motionFinished = False

    def handleFinished(self, event):
        self.motionFinished = True
        
    def testType(self):
        target = motion.common.Target(0,0)
        dir = motion.safe.OffsettingHover.Y
        m = motion.safe.OffsettingHover(target, dir, 0.2, 0.01)
        self.assertEqual(motion.basic.Motion.IN_PLANE, m.type)
        
    def testUpY(self):
        # Go to 0.5 units, at 0.1 unit a second, with a 10Hz update rate
        target = motion.common.Target(0,0)
        dir = motion.safe.OffsettingHover.Y
        m = motion.safe.OffsettingHover(target = target, direction = dir, 
            offset = 0.5, speed = 0.1)
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.safe.OffsettingHover.NEXT_OFFSET]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check five steps
        expectedDesired = 0
        for i in xrange(0, 50):
            expectedDesired += 0.01
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDesired, m._yDesired)
        self.assertAlmostEqual(0.5, m._yDesired, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(0.5, m._yDesired, 3)
        self.assertEqual(True, self.motionFinished)

    def testDownY(self):
        # Go to 0.5 units, at 0.1 unit a second, with a 10Hz update rate
        target = motion.common.Target(0,0)
        dir = motion.safe.OffsettingHover.Y
        m = motion.safe.OffsettingHover(target = target, direction = dir, 
            offset = -0.5, speed = 0.1)
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.safe.OffsettingHover.NEXT_OFFSET]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check five steps
        expectedDesired = 0
        for i in xrange(0, 50):
            expectedDesired -= 0.01
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDesired, m._yDesired)
        self.assertAlmostEqual(-0.5, m._yDesired, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(-0.5, m._yDesired, 3)
        self.assertEqual(True, self.motionFinished)
        
    def testUpX(self):
        # Go to 0.5 units, at 0.1 unit a second, with a 10Hz update rate
        target = motion.common.Target(0,0)
        dir = motion.safe.OffsettingHover.X
        m = motion.safe.OffsettingHover(target = target, direction = dir, 
            offset = 0.5, speed = 0.1)
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.safe.OffsettingHover.NEXT_OFFSET]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check five steps
        expectedDesired = 0
        for i in xrange(0, 50):
            expectedDesired += 0.01
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDesired, m._xDesired)
        self.assertAlmostEqual(0.5, m._xDesired, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(0.5, m._xDesired, 3)
        self.assertEqual(True, self.motionFinished)

    def testDownX(self):
        # Go to 0.5 units, at 0.1 unit a second, with a 10Hz update rate
        target = motion.common.Target(0,0)
        dir = motion.safe.OffsettingHover.X
        m = motion.safe.OffsettingHover(target = target, direction = dir, 
            offset = -0.5, speed = 0.1)
        self.qeventHub.subscribeToType(motion.basic.Motion.FINISHED, 
                                       self.handleFinished)
        
        # Start
        self.motionManager.setMotion(m)

        mockTimer = \
            support.MockTimer.LOG[motion.safe.OffsettingHover.NEXT_OFFSET]
        self.assert_(mockTimer.repeat)
        self.assertEqual(mockTimer.sleepTime, 0.1)

        # Check five steps
        expectedDesired = 0
        for i in xrange(0, 50):
            expectedDesired -= 0.01
            mockTimer.finish()
            self.qeventHub.publishEvents()
            self.assertEqual(expectedDesired, m._xDesired)
        self.assertAlmostEqual(-0.5, m._xDesired, 3)

        # Make sure more events don't let it keep going        
        mockTimer.finish()
        self.qeventHub.publishEvents()

        self.assertAlmostEqual(-0.5, m._xDesired, 3)
        self.assertEqual(True, self.motionFinished)