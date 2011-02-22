# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion/support.py

"""
Provides support classes for the testing of the Motion subsystem
"""

# STD Imports
import unittest

# Project Imports
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle
import ext.estimation as estimation
import ext.math as math

import ram.timer as timer
import ram.motion as motion
import ram.motion.basic

# Mock subsystems
class MockController(control.IController):
    def __init__(self, eventHub, estimator):
        control.IController.__init__(self, "Controller", eventHub)
        self._depth = 0
        self._depthRate = 0
        self._depthAccel = 0

        self._position = math.Vector2.ZERO
        self._velocity = math.Vector2.ZERO
        self._accel = math.Vector2.ZERO

        self._orientation = math.Quaternion.IDENTITY
        self._angularRate = math.Vector3.ZERO
        self._angularAccel = math.Vector3.ZERO

        self._yawChange = 0
        self._headingHolds = 0
        self._depthHolds = 0
        self._positionHolds = 0

        self._estimator = estimator
        


    def changeDepth(self, depth, depthRate = 0, depthAccel = 0):
        self._depth = depth
        self._depthRate = depthRate
        self._depthAccel = depthAccel

    def translate(self, position, 
                  velocity = math.Vector2.ZERO,
                  accel = math.Vector2.ZERO):
        self._position = position
        self._velocity = velocity
        self._accel = accel
    
    def rotate(self, orientation,
               angularRate = math.Vector3.ZERO,
               angularAccel = math.Vector3.ZERO):
        self._orientation = orientation
        self._angularRate = angularRate
        self._angularAccel = angularAccel
        


    def getDesiredDepth(self):
        return self._depth
        
    def getDesiredDepthRate(self):
        return self._depthRate

    def getDesiredDepthAccel(self):
        return self._depthAccel




    def getDesiredPosition(self):
        return self._position

    def getDesiredVelocity(self):
        return self._velocity

    def getDesiredAccel(self):
        return self._accel
    


    def getDesiredOrientation(self):
        return self._orientation
    
    def getDesiredAngularRate(self):
        return self._angularRate

    def getDesiredAngularAccel(self):
        return self._angularAccel



    def holdCurrentDepth(self):
        self._depthHolds += 1

    def holdCurrentHeading(self):
        self._headingHolds += 1

    def holdCurrentPosition(self):
        self._positionHolds += 1
        

    def yawVehicle(self, yawChange):
        self._yawChange = yawChange
        
    def setDesiredOrientation(self, orien):
        self._desiredOrientation = orien
        
    def getDesiredOrientation(self):
        return self._desiredOrientation



    def atDepth(self):
        if self._depth == self._estimator.getEstimatedDepth():
            return True
        else:
            return False
        
    def atPosition(self):
        print self._position, ' ', self._estimator.getEstimatedPosition()
        if self._position == self._estimator.getEstimatedPosition():
            return True
        else:
            return False

    def atVelocity(self):
        if self._velocity == self._estimator.getEstimatedVelocity():
            return True
        else:
            return False

    def atOrientation(self):
        if self._orientation == self._estimator.getEstimatedOrientation():
            return True
        else:
            return False
        
class MockVehicle(vehicle.IVehicle):
    def __init__(self, eventHub = core.EventHub(), cfg = None):
        vehicle.IVehicle.__init__(self, "Vehicle", eventHub)
        if cfg is None:
            cfg = {}

        # Take the name of the device and store it 
        # don't care about the contents
        self._devices = []
        for device in cfg.get('Devices', {}).iterkeys():
            self._devices.append(device)
        
        self.markersDropped = 0
        self.torpedosFired = 0
        self.grabberRelease = False
        self.unsafed = True
        self.force = math.Vector3.ZERO
        self.torque = math.Vector3.ZERO
    
    def applyForcesAndTorques(self, force, torque):
        self.force = force
        self.torque = torque

    def dropMarker(self):
        self.markersDropped += 1

    def fireTorpedo(self):
        self.torpedosFired += 1

    def releaseGrabber(self):
        self.grabberReleased = True

    def unsafeThrusters(self):
        self.unsafed = True

    def safeThrusters(self):
        self.unsafed = False

    def getDeviceNames(self):
        return self._devices    


# Mock Estimatior
class MockEstimator(estimation.IStateEstimator):
    def __init__(self, eventHub = core.EventHub(), cfg = None):
        estimation.IStateEstimator.__init__(self, "StateEstimator", eventHub)

        self._position = math.Vector2(0, 0)
        self._velocity = math.Vector2(0, 0)
        self._orientation = math.Quaternion.IDENTITY
        self._depth = 0
        self.linAccel = math.Vector3.ZERO
        self.angRate = math.Vector3.ZERO

        self._obstacles = {}
        
    def getEstimatedPosition(self):
        return self._position
    def getEstimatedVelocity(self):
        return self._velocity
    def getEstimatedLinearAcceleration(self):
        return self.linAccel
    def getEstimatedAngularRate(self):
        return self.angRate
    def getEstimatedOrientation(self):
        return self._orientation
    def getEstimatedDepth(self):
        return self._depth
    def getEstimatedDepthDot(self):
        return 0
    def addObstacle(self, name, obstacle):
        self.name = name
        self.obstacle = obstacle
    def getObstaclePosition(self):
        return math.Vector2(0,0)
    def getObstacleDepth(self):
        return 0

    def publishOrientationUpdate(self, vehicleOrientation):
        event = math.OrientationEvent()
        event.orientation = vehicleOrientation
        self.publish(estimation.IStateEstimator.ESTIMATED_ORIENTATION_UPDATE,
                     event)

    def publishVelocityUpdate(self, vehicleVelocity):
        event = math.Vector2Event()
        event.vector2 = vehicleVelocity
        self.publish(estimation.IStateEstimator.ESTIMATED_VELOCITY_UPDATE, event)

    def publishPositionUpdate(self, vehiclePosition):
        event = math.Vector2Event()
        event.vector2 = vehiclePosition
        self.publish(estimation.IStateEstimator.ESTIMATED_POSITION_UPDATE, event)

    def publishDepthUpdate(self, vehicleDepth):
        event = math.NumericEvent()
        event.number = vehicleDepth
        self.publish(estimation.IStateEstimator.ESTIMATED_DEPTH_UPDATE, event)

    def depth(self, val):
        self._depth = val
    def orientation(self, val):
        self._orientation = val
    def velocity(self, val):
        self._velocity = val
    def position(self, val):
        self._position = val
    depth = property(lambda self: self._depth, depth)
    orientation = property(lambda self: self._orientation,
                           orientation)
    velocity = property(lambda self: self._velocity, velocity)
    position = property(lambda self: self._position, position)


# For testing purposes
class MockTimer(timer.Timer):
    ORIG = timer.Timer
    LOG = {}
    
    def __init__(self, eventPublisher, eventType, sleepTime, repeat = False):
        MockTimer.ORIG.__init__(self, eventPublisher, eventType, sleepTime)
        
        self.sleepTime = sleepTime
        self.started = False
        self.stopped = False
        self.repeat = repeat
        
        # Log the timer so we can reference it in our tests
        MockTimer.LOG[eventType] = self
        
    def run(self):
        pass
        
    def start(self):
        self.started = True
        
    def stop(self):
        MockTimer.ORIG.stop(self)
        self.stopped = True
        
    def finish(self):
        """
        Fires off the finish event
        """
        self._complete()

# Mock Motion
class MockMotion(motion.basic.Motion):
    def __init__(self, _type = motion.basic.Motion.NORMAL):
        motion.basic.Motion.__init__(self, _type = _type)
        self.started = False

    @property
    def controller(self):
        return self._controller
    @property
    def vehicle(self):
        return self._vehicle
    @property
    def estimator(self):
        return self._estimator
    
    def _start(self):
        assert not self.started
        self.started = True
        
    def stop(self):
        motion.basic.Motion.stop(self)
        self.stopped = True

    @staticmethod
    def willComplete():
        return True
        
# Provides basic test support
class MotionTest(unittest.TestCase):
    def mockSleep(self, seconds):
        self.seconds = seconds
    
    def setUp(self):
        # Create the event hub to collect all published events
        self.eventHub = core.EventHub()
        self.vehicle = MockVehicle(self.eventHub)
        self.estimator = MockEstimator(self.eventHub)
        self.controller = MockController(self.eventHub, self.estimator)
        
        # The QueuedEventHub lets us queue the events to be released when ready
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        
        deps = [self.vehicle, self.controller, self.estimator,
                self.qeventHub, self.eventHub]
        self.motionManager = motion.basic.MotionManager({}, deps)

        # Replace Timer with out Mock Timer Class
        MockTimer.LOG = {}
        timer._origTimer = timer.Timer
        timer.Timer = MockTimer

    def tearDown(self):
        # Put the original timer class back
        timer.Timer = timer._origTimer
        del timer._origTimer
