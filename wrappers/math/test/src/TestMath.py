# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/math/test/src/TestMath.py

# STD Imports
import unittest
import math as pmath

# Project Imports
import ext.math as math

class TestConversions(unittest.TestCase):
    def testRadianDegree(self):
        self.assertEquals(20, math.Degree(20).valueDegrees())

        rad = math.Radian(math.Degree(20))
        self.assertEquals(20, rad.valueDegrees())

        deg = math.Degree(math.Radian(pmath.pi))
        self.assertAlmostEqual(180, deg.valueDegrees(), 3)

        
        # Ensure no automatic double to Radian/Degree conversions occur
        try:
            deg - 1
            self.fail("Implicit conversions from float/int not allowed")
        except TypeError:
            pass

        try:
            deg - 2.0
            self.fail("Implicit conversions from float/int not allowed")
        except TypeError:
            pass
        
        try:
            rad - 3
            self.fail("Implicit conversions from float/int not allowed")
        except TypeError:
            pass

        try:
            rad - 4.0
            self.fail("Implicit conversions from float/int not allowed")
        except TypeError:
            pass

class TestVectorMatrix2(unittest.TestCase):
    def test(self):
        start = math.Vector2(1,0);
        rotationMatrix = math.Matrix2()
        rotationMatrix.fromAngle(math.Degree(45));
        
        expected = math.Vector2(math.Vector2(1,1).normalisedCopy());
        # TODO: Expose the free operator functions
        #result = (start * rotationMatrix)

        #self.assertAlmostEqual(expected.x, result.x, 4)
        #self.assertAlmostEqual(expected.y, result.y, 4)

if __name__ == '__main__':
    unittest.main()
