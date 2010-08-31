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
import ram.timer

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
        
    def testSetState(self):
        # Test basic set state
        x = 1
        y = 0.5
        azimuth = 10
        elevation = 10
        range = 5
        timeStamp = 1
        
        p = motion.seek.PointTarget(azimuth = 5, elevation = 5, range = 3, 
                                    x = 2, y = 1, timeStamp = None)

        p.setState(azimuth = azimuth, elevation = elevation, 
                   range = range, x = x, y = y, timeStamp = timeStamp)

        # Test new values
        self.assertEqual(x, p.x)
        self.assertEqual(y, p.y)
        self.assertEqual(azimuth, p.azimuth)
        self.assertEqual(elevation, p.elevation)
        self.assertEqual(range, p.range)
        self.assertEqual(timeStamp, p.timeStamp)

        # Test old values
        self.assertEqual(2, p.prevX)
        self.assertEqual(1, p.prevY)
        self.assertEqual(5, p.prevAzimuth)
        self.assertEqual(5, p.prevElevation)
        self.assertEqual(3, p.prevRange)
        self.assertEqual(None, p.prevTimeStamp)
        
        # Test set state with a level vehicle present
        mockVehicle = support.MockVehicle()
        
        p = motion.seek.PointTarget(azimuth = 5, elevation = 5, range = 3, 
                                    x = 2, y = 1, timeStamp = None,
                                    vehicle = mockVehicle)

        p.setState(azimuth = azimuth, elevation = elevation, 
                   range = range, x = x, y = y, timeStamp = timeStamp)
        self.assertEqual(x, p.x)
        self.assertEqual(y, p.y)
        self.assertEqual(azimuth, p.azimuth)
        self.assertEqual(elevation, p.elevation)
        self.assertEqual(range, p.range)
        self.assertEqual(timeStamp, p.timeStamp)

        # Test old values
        self.assertEqual(2, p.prevX)
        self.assertEqual(1, p.prevY)
        self.assertEqual(5, p.prevAzimuth)
        self.assertEqual(5, p.prevElevation)
        self.assertEqual(3, p.prevRange)
        self.assertEqual(None, p.prevTimeStamp)
        
        # Test with un-level vehicle pitched downward 11.25 degrees 
        # or 1/8 the FOV
        mockEstimator.orientation = math.Quaternion(
            math.Degree(11.25), math.Vector3.UNIT_Y) 
        motion.seek.PointTarget.VERTICAL_FOV = 90
        
        p.setState(azimuth = azimuth, elevation = elevation, 
                   range = range, x = x, y = y, timeStamp = timeStamp)
        self.assertEqual(x, p.x)
        self.assertAlmostEqual(0.25, p.y, 5)
        self.assertEqual(azimuth, p.azimuth)
        self.assertEqual(elevation, p.elevation)
        self.assertEqual(range, p.range)

        self.assertEqual(x, p.prevX)
        self.assertEqual(y, p.prevY)
        self.assertEqual(azimuth, p.prevAzimuth)
        self.assertEqual(elevation, p.prevElevation)
        self.assertEqual(range, p.prevRange)
        self.assertEqual(timeStamp, p.prevTimeStamp)
        

class SeekPointTest(support.MotionTest):
    def makeClass(self, *args, **kwargs):
        return motion.seek.SeekPoint(*args, **kwargs)
            
    def makeTarget(self, *args, **kwargs):
        return motion.seek.PointTarget(*args, **kwargs)
            
    def checkCommand(self, azimuth, elevation, range = 0, x = 0, y = 0,
                     yawChange = None, newDepth = None, newSpeed = None,
                     newSidewaysSpeed = None, translate = False,
                     maxDepthDt = 0):
        """
        Checks the commands given to the controller with a certain buoy state
        
        @type  yawChange: float
        @param yawChange: The expected IController.yawVehicle() value
        
        @type  newDepth: float
        @param newDepth: The expected IController.setDepth() value
        """
        # Creat Buoy with the given characteristics
        Buoy = self.makeTarget(azimuth = azimuth, elevation = elevation, 
                               range = range, x = x, y = y)
        
        # Creat the motion to seek the target
        maxSpeed = 0
        if newSpeed is not None:
            maxSpeed = 1
        if newSidewaysSpeed is None or translate:
            if not translate:
                m = self.makeClass(target = Buoy, maxSpeed = maxSpeed, 
                                   maxDepthDt = maxDepthDt)
            else:
                m = self.makeClass(target = Buoy, maxSpeed = maxSpeed,
                                   translate = True, maxDepthDt = maxDepthDt)
        else:
            maxSidewaysSpeed = 0
            if newSidewaysSpeed is not None:
                maxSidewaysSpeed = 1
            m = self.makeClass(target = Buoy, maxSpeed = maxSpeed,
                               maxSidewaysSpeed = maxSidewaysSpeed,
                               maxDepthDt = maxDepthDt)
        
        # Start it and check the first results
        self.motionManager.setMotion(m)
        if yawChange is not None:
            self.assertAlmostEqual(yawChange, self.controller.yawChange, 3)
        if newDepth is not None:
            self.assertAlmostEqual(newDepth, self.controller.depth, 3)
        if newSpeed is not None:
            self.assertAlmostEqual(newSpeed, self.controller.speed, 3)
        if newSidewaysSpeed is not None:
            self.assertAlmostEqual(newSidewaysSpeed, 
                                   self.controller.sidewaysSpeed, 3)

class TestSeekPoint(SeekPointTest):
    #def setUp(self):
    

    
    def testDeadAhead(self):
        # Buoy dead ahead of the vehicle
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
        # actually in a 0.5 foot oscillation around the target depth
        self.estimator.depth = 5
        self.controller.depth = 4.5
        
        # Buoy dead ahead and 5 feet below current vehicle position
        # Make sure we only change a depth
        self.checkCommand(azimuth = 0, elevation = 0, y = -0.75, yawChange = 0, 
                          newDepth = 5.75)

        # Now do the same test with a limited depth delta
        self.estimator.depth = 5
        self.controller.depth = 4.5
        
        self.checkCommand(azimuth = 0, elevation = 0, y = -0.75, yawChange = 0, 
                          newDepth = 5.3, maxDepthDt = 0.3)
        
    def testAbove(self):
        # Same oscillation issue as above
        self.estimator.depth = 10
        self.controller.depth = 10.5
        
        # Buoy dead ahead and 2.588 feet above the current vehicle
        # Make sure we only change depth
        self.checkCommand(azimuth = 0, elevation = 0, y = 0.5, yawChange = 0, 
                          newDepth = 9.5)

        # Now do the same test with a limited depth delta
        self.estimator.depth = 10
        self.controller.depth = 10.5
        
        self.checkCommand(azimuth = 0, elevation = 0, y = 0.5, yawChange = 0, 
                          newDepth = 9.7, maxDepthDt = 0.3)
        
    def testLeft(self):
        # Turning toward light at 30 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
        self.estimator.orientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
        
        # Buoy at same depth and 45 degrees left of vehicle's heading
        # Make sure we only rotate relative to the controllers desired 
        # orientation
        self.checkCommand(azimuth = 45, elevation = 0, yawChange = 20, 
                          newDepth = 0)
        
        # Turning away from light at 55 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
        self.estimator.orientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
            
        # Buoy dead ahead and 45 degrees left of vehicle's heading
        self.checkCommand(azimuth = 45, elevation = 0, yawChange = 70, 
                          newDepth = 0)
        
    def testTranslateLeft(self):
        # Buoy in the left part of the frame
        self.checkCommand(azimuth = 45, elevation = 0, x = -0.5, 
                          translate = True, newSidewaysSpeed = -0.5)
        

    def testRight(self):
        # Turning away from light at -30 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
        self.estimator.orientation = \
            math.Quaternion(math.Degree(-15), math.Vector3.UNIT_Z)
       
        # Buoy at same depth and 15 degrees right of vehicle's heading
        # Make sure we only rotate relative to the controllers desired 
        # orientation
        self.checkCommand(azimuth = -15, elevation = 0, yawChange = -40, 
                          newDepth = 0)
        
        # Turning toward light at 55 Degrees off North
        self.controller.desiredOrientation = \
            math.Quaternion(math.Degree(-5), math.Vector3.UNIT_Z)
        self.estimator.orientation = \
            math.Quaternion(math.Degree(10), math.Vector3.UNIT_Z)
            
        # Buoy dead ahead and 20 degrees right of vehicle's heading
        self.checkCommand(azimuth = -20, elevation = 0, yawChange = -5, 
                          newDepth = 0)

    def testTranslateRight(self):
        # Buoy in the right part of the frame
        self.checkCommand(azimuth = -45, elevation = 0, x = 0.5, 
                          translate = True, newSidewaysSpeed = 0.5)
        
    def testStop(self):
        # Buoy in the right part of the frame
        self.checkCommand(azimuth = -22.5, elevation = 0, x = 0.25,
                          range = 10, newSpeed = 0.6464,
                          translate = True, newSidewaysSpeed = 0.25)
        
        # Stop the motion and make sure there is zero sideways speed
        self.motionManager.stopCurrentMotion()
        self.assertEqual(0, self.controller.sidewaysSpeed)
        self.assertEqual(0, self.controller.speed)

    def testTrack(self):
        self.estimator.depth = 5
        
        # Create Buoy ahead of the vehicle
        Buoy = motion.seek.PointTarget(azimuth = 0, elevation = 0, range = 0,
                                       x = 0, y = 0)
        m = motion.seek.SeekPoint(target = Buoy)
        self.motionManager.setMotion(m)
        
        # Make sure we aren't going anywhere
        self.assertAlmostEqual(0, self.controller.yawChange, 3)
        self.assertAlmostEqual(5, self.controller.depth, 3)

        # Update the buoy and make sure the motion updates the controller
        Buoy.setState(azimuth = 15, elevation = 30, range = 0, y = 1, x = 0,
                      timeStamp = None)
        self.assertAlmostEqual(15, self.controller.yawChange, 4)
        self.assertAlmostEqual(4, self.controller.depth, 4)

        # Make sure a second update works
        Buoy.setState(azimuth = -25, elevation = -30, range = 0, y = -1, x = 0,
                      timeStamp = None)
        self.assertAlmostEqual(-25, self.controller.yawChange, 4)
        self.assertAlmostEqual(6, self.controller.depth, 4)

        # Stop motion and make sure updates there is not a change in the
        # controller from further state changes
        m.stop()
        Buoy.setState(azimuth = 15, elevation = 9, range = 0, x = 0, y = -2,
                      timeStamp = None)
        self.assertAlmostEqual(-25, self.controller.yawChange, 4)
        self.assertAlmostEqual(6, self.controller.depth, 4)

    def _handlePointAligned(self, event):
        self._aligned = True
        
    def testAlignEvent(self):
        # Subscribe to align event
        self._aligned = False
        self.qeventHub.subscribeToType(
            ram.motion.seek.SeekPoint.POINT_ALIGNED,
            self._handlePointAligned)
        
        # Create our point to seek
        Buoy = motion.seek.PointTarget(azimuth = 0, elevation = 0, range = 0,
                                       x = 0, y = 0)
        
        # Start up the motion
        Buoy.setState(azimuth = 50, elevation = 30, range = 0, x = 1, y = 1,
                      timeStamp = None)
        m = motion.seek.SeekPoint(target = Buoy)
        self.motionManager.setMotion(m)
        
        # Feed data which doesn't trigger the event
        self.qeventHub.publishEvents()
        self.assertFalse(self._aligned)
        
        # Feed data which does trigger the event
        Buoy.setState(azimuth = 0, elevation = 0, range = 0, y = 0, x = 0,
                      timeStamp = None)
        self.qeventHub.publishEvents()
        self.assertTrue(self._aligned)

class TestSeekPointToRange(SeekPointTest):
    def addClassArgs(self, kwargs):
        # These value by default make the vehicle go full forward speed
        if not kwargs.has_key('desiredRange'):
            kwargs['desiredRange'] = self.desiredRange
        if not kwargs.has_key('maxRangeDiff'):
            kwargs['maxRangeDiff'] = self.maxRangeDiff
                
    def makeClass(self, *args, **kwargs):
        self.addClassArgs(kwargs)
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
