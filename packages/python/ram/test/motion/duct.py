# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/test/motion/duct.py

# STD Imports
import unittest

# Project Imports
import ram.motion.duct as duct
from ram.test.motion.seek import TestSeekPointToRange

class TestDuctSeekAlign(TestSeekPointToRange):
    def setUp(self):
        TestSeekPointToRange.setUp(self)
        self.maxAlignDiff = 1
        self.maxSidewaysSpeed = 1
        self.targetAlignment = 0
        self.desiredRange = 0
        self.maxRangeDiff = 1
    
    def addClassArgs(self, kwargs):
        # These value by default make the vehicle go full forward speed
        if not kwargs.has_key('maxAlignDiff'):
            kwargs['maxAlignDiff'] = self.maxAlignDiff
        if not kwargs.has_key('maxSidewaysSpeed'):
            kwargs['maxSidewaysSpeed'] = self.maxSidewaysSpeed
        TestSeekPointToRange.addClassArgs(self, kwargs)

    def makeTarget(self, *args, **kwargs):
        if not kwargs.has_key('alignment'):
            kwargs['alignment'] = self.targetAlignment
        return duct.Duct(*args, **kwargs)

    def makeClass(self, *args, **kwargs):
        self.addClassArgs(kwargs)
        return duct.DuctSeekAlign(*args, **kwargs)
    
    def testToTheRight(self):
        # Test within max align
        self.maxAlignDiff = 40
        self.targetAlignment = 20
        
        # Straigh aheard and to the right
        self.checkCommand(azimuth = 0, elevation = 0, newSidewaysSpeed = -0.5)

        # Now outside the max
        self.targetAlignment = 60
        self.checkCommand(azimuth = 0, elevation = 0, newSidewaysSpeed = -1)
        
    def testToTheLeft(self):
        self.maxAlignDiff = 40
        self.targetAlignment = -20
        
        # Straigh aheard and to the left
        self.checkCommand(azimuth = 0, elevation = 0, newSidewaysSpeed = 0.5)
        
        self.targetAlignment = -60
        self.checkCommand(azimuth = 0, elevation = 0, newSidewaysSpeed = 1)