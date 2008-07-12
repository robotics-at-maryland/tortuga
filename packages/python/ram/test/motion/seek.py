# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/test/motion/seek.py

"""
Tests for the ram.motion.seek module
"""

# STD Imports
import unittest

# Project Imports
import ext.math as math

import ram.motion as motion
import ram.motion.seek
import ram.test.motion.support as support

class TestPointTarget(unittest.TestCase):
    def testRelativeDepth(self):
        # Below
        p = motion.seek.PointTarget(azimuth = 0, elevation = -30, range = 10,
                                    x = 0, y = 0)
        self.assertAlmostEqual(5, p.relativeDepth, 5)

        # Below with some azimuth
        p = motion.seek.PointTarget(azimuth = 15, elevation = -15, range = 3,
                                    x = 0, y = 0)
        self.assertAlmostEqual(0.776, p.relativeDepth, 3)
        
        # Above
        p = motion.seek.PointTarget(azimuth = 15, elevation = 30, range = 5,
                                    x = 0, y = 0)
        self.assertAlmostEqual(-2.5, p.relativeDepth, 5)
        
        # Below with some azimuth
        p = motion.seek.PointTarget(azimuth = 15, elevation = 45, range = 12,
                                    x = 0, y = 0)
        self.assertAlmostEqual(-8.485, p.relativeDepth, 3)

class SeekPointTest(support.MotionTest):
    def makeClass(self, *args, **kwargs):
        return motion.seek.SeekPoint(*args, **kwargs)
            
    def checkCommand(self, azimuth, elevation, range = 0, x = 0, y = 0,
                     yawChange = None, newDepth = None, newSpeed = None):
        """
        Checks the commands given to the controller with a certain buoy state
        
        @type  yawChange: float
        @param yawChange: The expected IController.yawVehicle() value
        
        @type  newDepth: float
        @param newDepth: The expected IController.setDepth() value
        """
        # Creat bouy with the given characteristics
        bouy = motion.seek.PointTarget(azimuth = azimuth, 
                                       elevation = elevation, 
                                       range = range, x = x, y = y)
        
        # Creat the motion to seek the target
        maxSpeed = 0
        if newSpeed is not None:
            maxSpeed = 1
        m = self.makeClass(target = bouy, maxSpeed = maxSpeed)
        
        # Start it and check the first results
        self.motionManager.setMotion(m)
        if yawChange is not None:
            self.assertAlmostEqual(yawChange, self.controller.yawChange, 3)
        if newDepth is not None:
            self.assertAlmostEqual(newDepth, self.controller.depth, 3)
        if newSpeed is not None:
            self.assertAlmostEqual(newSpeed, self.controller.speed, 3)

class TestSeekPoint(SeekPointTest):
    #def setUp(self):
    

    
    def testDeadAhead(self):
        # Bouy dead ahead of the vehicle
        # Make sure no vehicle heading or depth changes are ordered
        self.checkCommand(azimuth = 0, elevation = 0, range = 10, 
                          yawChange = 0, newDepth = 0, newSpeed = 1)

    def testSpeed(self):
        # This is right at the range, we shouldn't be moving
        self.checkCommand(azimuth = 45, elevation = 45, newSpeed = 0)

        # Half way there
        self.checkCommand(azimuth = 45/2.0, elevation = 45/2.0, newSpeed = 0.5)

        # A little closer
        self.checkCommand(azimuth = 45/2.0, elevation = 0, newSpeed = 0.6464)
        self.checkCommand(azimuth = 0, elevation = 45/2.0, newSpeed = 0.6464)

        # Dead ahead
        self.checkCommand(azimuth = 0, elevation = 0, newSpeed = 1)
        
    def testBelow(self):
        # Setup the vehicle and the controller, such that the vehicle is 
        # actually in a 0.5 foot oscilation around the target depth
        self.vehicle.depth = 5
        self.controller.depth = 4.5
        
        # Bouy dead ahead and 5 feet below current vehicle position
        # Make sure we only change a depth
        self.checkCommand(azimuth = 0, elevation = 0, y = -0.75, yawChange = 0, 
                          newDepth = 5.75)
        
    def testAbove(self):
        # Same oscilation issue as above
        self.vehicle.depth = 10
        self.controller.depth = 10.5
        
        # Bouy dead ahead and 2.588 feet above the current vehicle
        # Make sure we only change depth
        self.checkCommand(azimuth = 0, elevation = 0, y = 0.5, yawChange = 0, 
                          newDepth = 9.5)
        
    def testLeft(self):
        # Turning toward light at 30 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
        
        # Bouy at same depth and 45 degress left of vehicle's heading
        # Make sure we only rotate relative to the controllers desired 
        # orientation
        self.checkCommand(azimuth = 45, elevation = 0, yawChange = 20, 
                          newDepth = 0)
        
        # Turning away from light at 55 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
            
        # Bouy dead ahead and 45 degress left of vehicle's heading
        self.checkCommand(azimuth = 45, elevation = 0, yawChange = 70, 
                          newDepth = 0)

    def testRight(self):
        # Turning away from light at -30 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
       
        # Bouy at same depth and 15 degress right of vehicle's heading
        # Make sure we only rotate relative to the controllers desired 
        # orientation
        self.checkCommand(azimuth = -15, elevation = 0, yawChange = -40, 
                          newDepth = 0)
        
        # Turning toward light at 55 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(-5), math.Vector3.UNIT_Z)
        self.vehicle.orientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
            
        # Bouy dead ahead and 20 degress right of vehicle's heading
        self.checkCommand(azimuth = -20, elevation = 0, yawChange = -5, 
                          newDepth = 0)

    def testTrack(self):
        self.vehicle.depth = 5
        
        # Creat bouy ahead of the vehicle
        bouy = motion.seek.PointTarget(azimuth = 0, elevation = 0, range = 0,
                                       x = 0, y = 0)
        m = motion.seek.SeekPoint(target = bouy)
        self.motionManager.setMotion(m)
        
        # Make sure we aren't going anywhere
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(5, self.controller.depth, 3)

        # Update the buoy and make sure the motion updates the controller
        bouy.setState(azimuth = 15, elevation = 30, range = 0, y = 1, x = 0)
        self.assertAlmostEqual(15, self.controller.yawChange, 4)
        self.assertAlmostEqual(4, self.controller.depth, 4)

        # Make sure a second update works
        bouy.setState(azimuth = -25, elevation = -30, range = 0, y = -1, x = 0)
        self.assertAlmostEqual(-25, self.controller.yawChange, 4)
        self.assertAlmostEqual(6, self.controller.depth, 4)

        # Stop motion and make sure updates there is not a change in the
        # controller from further state changes
        m.stop()
        bouy.setState(azimuth = 15, elevation = 9, range = 0, x = 0, y = -2)
        self.assertAlmostEqual(-25, self.controller.yawChange, 4)
        self.assertAlmostEqual(6, self.controller.depth, 4)


class TestSeekPointToRange(SeekPointTest):
    def makeClass(self, *args, **kwargs):
        
        # These value by default make the vehicle go full forward speed
        if not kwargs.has_key('desiredRange'):
            kwargs['desiredRange'] = self.desiredRange
        if not kwargs.has_key('maxRangeDiff'):
            kwargs['maxRangeDiff'] = self.maxRangeDiff
        
        return motion.seek.SeekPointToRange(*args, **kwargs)
    
    def testOffHeadingSpeed(self):
        # These value by default make the vehicle go full forward speed
        self.desiredRange = -1
        self.maxRangeDiff = 1
        
        # This is right at the range, we shouldn't be moving
        self.checkCommand(azimuth = 45, elevation = 45, newSpeed = 0)

        # Half way there
        self.checkCommand(azimuth = 45/2.0, elevation = 45/2.0, newSpeed = 0.5)

        # A little closer
        self.checkCommand(azimuth = 45/2.0, elevation = 0, newSpeed = 0.6464)
        self.checkCommand(azimuth = 0, elevation = 45/2.0, newSpeed = 0.6464)

        # Dead ahead
        self.checkCommand(azimuth = 0, elevation = 0, newSpeed = 1)
        
    def testRangeSpeed(self):
        # Values are passed as the 'desiredRange' and 'maxRangeDiff' args to 
        # the SeekTargeToRange constructor
        self.desiredRange = 10.0
        self.maxRangeDiff = 5.0
        
        # Proper range, no motion (max speed set to 1)
        self.checkCommand(azimuth = 0, elevation = 0, range = 10, newSpeed = 0)
        
        # Too close, and inside maxRangeDiff
        self.checkCommand(azimuth = 0, elevation = 0, range = 7.5, 
                          newSpeed = -0.5)
        
        # Too close outside maxRangeDiff, value should be capped
        self.checkCommand(azimuth = 0, elevation = 0, range = 2.5,
                          newSpeed = -1)
        
        # Too far, and inside maxRangeDiff
        self.checkCommand(azimuth = 0, elevation = 0, range = 12.5, 
                          newSpeed = 0.5)
        
        # Too far outside maxRangeDiff, value should be capped
        self.checkCommand(azimuth = 0, elevation = 0, range = 17,
                          newSpeed = 1)
        
    def testBothSpeeds(self):
        self.desiredRange = 10.0
        self.maxRangeDiff = 5.0
        
        # Too close, and inside maxRangeDiff
        self.checkCommand(azimuth = 45/2.0, elevation = 45/2.0, range = 7.5, 
                          newSpeed = -0.25)