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


if __name__ == '__main__':
    unittest.main()
