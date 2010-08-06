# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/filter.py

# STD Import
import unittest

# Project Imports
import ram.filter as filter

class TestMovingAverageFilter(unittest.TestCase):
    def testInit(self):
        # Test that setting a default works properly
        f = filter.MovingAverageFilter(5, default = 0.0)
        self.assertEqual([0,0,0,0,0], f.get())
        
        # Test that setting no default value works properly
        f = filter.MovingAverageFilter(5)
        self.assertEqual([], f.get())
        
    def testGetAverage(self):
        f = filter.MovingAverageFilter(5, default = 0.0)
        self.assertEqual(0, f.getAverage())
        
        inVals = [5,5,5,5,5]
        avgVals = [1, 2, 3, 4, 5]
        
        for i, a in zip(inVals, avgVals):
            f.append(i)
            self.assertAlmostEqual(a, f.getAverage(), 5)
            
        f = filter.MovingAverageFilter(5)
        self.assertEqual(0, f.getAverage())
        
        inVals = [5,5,5,5,5]
        avgVals = [5,5,5,5,5]
        
        for i, a in zip(inVals, avgVals):
            f.append(i)
            self.assertAlmostEqual(a, f.getAverage(), 5)
            
        f = filter.MovingAverageFilter(5)
        self.assertEqual(0, f.getAverage())
        
        inVals = [1,2,3,4,5]
        avgVals = [1, 1.5, 2, 2.5, 3]
        for i, a in zip(inVals, avgVals):
            f.append(i)
            self.assertAlmostEqual(a, f.getAverage(), 5)
            
    def testSingleLength(self):
        f = filter.MovingAverageFilter(1)
        
        inVals = [3,1,8,10,2]
        avgVals = inVals
        
        for i, a in zip(inVals, avgVals):
            f.append(i)
            self.assertAlmostEqual(a, f.getAverage(), 5)