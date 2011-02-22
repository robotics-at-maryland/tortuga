# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
# All rights reserved.
#
# Author: Jonathan Wonders <jwonders@umd.edu>
# File:  packages/python/ram/test/motion.py

# STD Imports
import unittest

# Project Imports
import ext.math as math
import ram.motion.trajectories as trajectories


class TestScalarCubicTrajectory(unittest.TestCase):
    def testScalarCubicTrajectory(self):
        # first make sure that the trajectory boundary conditions are right
        iVal = 2.0
        fVal = 5.0
        iRate = 0.0
        fRate = 0.0

        t = trajectories.ScalarCubicTrajectory(initialValue = iVal,
                                               finalValue = fVal,
                                               initialRate = iRate,
                                               finalRate = fRate)
        tTimeI = t.getInitialTime()
        tTimeF = t.getFinalTime()

        self.assertAlmostEqual(tTimeI, 0.0, 4)
        self.assertAlmostEqual(t.computeValue(tTimeI), iVal, 4)
        self.assertAlmostEqual(t.computeValue(tTimeF), fVal, 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeI, 1), iRate, 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeF, 1), fRate, 4)

        
class TestVector2CubicTrajectory(unittest.TestCase):
    def testVector2CubicTrajectory(self):
        # first make sure that the trajectory boundary conditions are right
        iVal = math.Vector2(1, 2)
        fVal = math.Vector2(3, 4)
        iRate = math.Vector2(0, 0)
        fRate = math.Vector2(0, 0)

        t = trajectories.Vector2CubicTrajectory(initialValue = iVal,
                                                finalValue = fVal,
                                                initialRate = iRate,
                                                finalRate = fRate)
        tTimeI = t.getInitialTime()
        tTimeF = t.getFinalTime()

        self.assertAlmostEqual(tTimeI, 0.0, 4)

        self.assertAlmostEqual(t.computeValue(tTimeI)[0], iVal[0], 4)
        self.assertAlmostEqual(t.computeValue(tTimeI)[1], iVal[1], 4)

        self.assertAlmostEqual(t.computeValue(tTimeF)[0], fVal[0], 4)
        self.assertAlmostEqual(t.computeValue(tTimeF)[1], fVal[1], 4)

        self.assertAlmostEqual(t.computeDerivative(tTimeI, 1)[0], iRate[0], 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeI, 1)[1], iRate[1], 4)

        self.assertAlmostEqual(t.computeDerivative(tTimeF, 1)[0], fRate[0], 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeF, 1)[1], fRate[1], 4)

class TestStepTrajectory(unittest.TestCase):
    def testStepTrajectory(self):
        # first make sure that the trajectory boundary conditions are right
        iVal = 0.0
        fVal = 5.0
        iRate = 1.0
        fRate = 3.0

        t = trajectories.StepTrajectory(initialValue = iVal,
                                        finalValue = fVal,
                                        initialRate = iRate,
                                        finalRate = fRate)

        # we need a time slightly before and after the step for this one
        tTimeI = t.getInitialTime() - 0.0001
        tTimeF = t.getFinalTime() + 0.0001

        self.assertAlmostEqual(tTimeI, -0.0001, 4)
        self.assertAlmostEqual(t.computeValue(tTimeI), iVal, 4)
        self.assertAlmostEqual(t.computeValue(tTimeF), fVal, 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeI, 1), iRate, 4)
        self.assertAlmostEqual(t.computeDerivative(tTimeF, 1), fRate, 4)


class TestSlerpTrajectory(unittest.TestCase):
    pass

class TestAngularRateTrajectory(unittest.TestCase):
    pass
