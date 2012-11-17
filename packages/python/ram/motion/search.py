# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/motion.py

# Project Imports
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle
import ext.estimation as estimation
import ext.math as math

import ram.timer
from ram.motion.basic import Motion
import ram.motion.basic as basic

class ForwardZigZag(Motion):
    LEG_COMPLETE = core.declareEventType('LEG_COMPLETE')
    
    def __init__(self, legTime, sweepAngle, speed, direction = None):
        """
        Drives a forward zig-zag pattern
        """
        Motion.__init__(self, _type = Motion.IN_PLANE | Motion.ORIENTATION)
        
        self._first = True
        self._legTime = legTime
        self._sweepAngle = sweepAngle
        self._speed = speed
        self._connections = []
        self._timer = None
        self._inLeg = False
        self._direction = direction
        
    def _start(self):
        # Register to recieve AT_ORIENTATION events
        conn = self._eventHub.subscribe(control.IController.AT_ORIENTATION,
                                        self._controller, self._atOrientation)
        self._connections.append(conn)
        
        conn = self._eventHub.subscribeToType(ForwardZigZag.LEG_COMPLETE,
                                              self._legFinished)
        self._connections.append(conn)

        if self._direction is None:
            currentAngle = 0
            desiredAngle = 0
        else:
            vehicle = self._estimator.getEstimatedOrientation()
            currentAngle = vehicle.getYaw().valueDegrees()
            desiredAngle = self._direction - currentAngle

        if currentAngle - desiredAngle > 0:
            self._sweepAngle *= -1

        self._controller.yawVehicle(desiredAngle + (self._sweepAngle / 2.0))
        self._sweepAngle *= -1
        
    def _atOrientation(self, event):
        """
        AT_ORIENTATION event handler
        
        Called when the vehicle reaches the command orientation
        """
        if self._inLeg:
            return
        self._inLeg = True

        legTime = self._legTime
        if self._first:
            legTime = legTime / 2.0
            self._first = False
        
        # Start the vehicle forward and create a timer to change the motion
        self._controller.setSpeed(self._speed)
        self._timer = ram.timer.Timer(self._eventPublisher, 
                                     ForwardZigZag.LEG_COMPLETE, legTime)
        self._timer.start()

            
    def _legFinished(self, event):
        """
        Called when the leg timer expires
        """
        self._controller.setSpeed(0)
        self._controller.yawVehicle(self._sweepAngle)
        self._sweepAngle *= -1
        self._timer = None
        self._inLeg = False
    
    def stop(self):
        """
        Called by the MotionManager when another motion takes over, stops movement
        """
        self._controller.setSpeed(0)
        for conn in self._connections:
            conn.disconnect()

        if self._timer is not None:
            self._timer.stop()

    @staticmethod
    def isComplete():
        return False
            
class ZigZag(Motion):
    LEG_COMPLETE = core.declareEventType('LEG_COMPLETE')
    
    def __init__(self, legTime, sweepAngle, speed):
        """
        Drives a zig-zag which is indepdent of orientation
        """
        Motion.__init__(self, _type = Motion.IN_PLANE)
        
        self._first = True
        self._legTime = legTime
        self._sweepAngle = sweepAngle
        self._speed = speed
        self._connections = []
        
        self._timer = None
        self._direction = None
        self._forwardDirection = None
        
    def _start(self):
        # Register to recieve ORIENTATION_UPDATE events
        conn = self._eventHub.subscribe(
            estimation.IStateEstimator.ESTIMATED_ORIENTATION_UPDATE,
            self._estimator, self._onOrientation)
        self._connections.append(conn)
        
        conn = self._eventHub.subscribeToType(ZigZag.LEG_COMPLETE,
                                              self._onLegFinished)
        self._connections.append(conn)

        # Grab our general direction of motion
        self._forwardDirection = self._estimator.getEstimatedOrientation()
        
        # Start the pattern
        self._update()

        
    def _update(self):
        legTime = self._legTime
        if self._first:
            legTime = legTime / 2.0
            self._first = False
        
        # Start the vehicle forward and create a timer to change the motion
        self._setDirection()
        self._setSpeeds(self._estimator.getEstimatedOrientation())
        
        self._timer = ram.timer.Timer(self._eventPublisher, 
                                      ZigZag.LEG_COMPLETE, legTime)
        self._timer.start()

        
    def _setSpeeds(self, orientation):
        """
        Steps the speeds to vehicle based on the given orientation
        """
        # Direction of desired vehicle motion
        desiredDirection = self._direction.getYaw(True).valueDegrees()
        
        # Vehicle heading in degrees
        vehicleHeading =  orientation.getYaw(True).valueDegrees()
        
        yawTransform = vehicleHeading - desiredDirection

        # Find speed in vehicle cordinates
        baseSpeed = math.Vector3(self._speed, 0, 0)
        toVehicleFrame = math.Quaternion(math.Degree(yawTransform),
                                         math.Vector3.UNIT_Z)
        vehicleSpeed = toVehicleFrame * baseSpeed
        
        # Finally set the speeds
        self._controller.setSpeed(vehicleSpeed.x)
        self._controller.setSidewaysSpeed(vehicleSpeed.y)
        
    def _setDirection(self):
        """
        Sets the direction of the search path based on the current sweep angle
        """
        self._sweepAngle *= -1
        dir = math.Quaternion(math.Degree(self._sweepAngle / 2.0), 
                              math.Vector3.UNIT_Z)
        self._direction = dir * self._forwardDirection
        

    def _onOrientation(self, event):
        """
        Corrects commanded speed based on vehicle alignment
        """
        self._setSpeeds(event.orientation)
        
    def _onLegFinished(self, event):
        """
        Called when the leg timer expires
        """
        self._timer = None
        self._update()
    
    def stop(self):
        """
        Called by the MotionManager when another motion takes over, stops movement
        """
        self._controller.setSpeed(0)
        self._controller.setSidewaysSpeed(0)
        
        for conn in self._connections:
            conn.disconnect()

        if self._timer is not None:
            self._timer.stop()
            
    @staticmethod
    def isComplete():
        return False
