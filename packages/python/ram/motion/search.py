# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/motion.py

# Project Imports
import ext.core as core
import ext.control as control
import ext.math as math

import ram.timer
from ram.motion.basic import Motion

class ForwardZigZag(Motion):
    LEG_COMPLETE = core.declareEventType('LEG_COMPLETE')
    
    def __init__(self, legTime, sweepAngle, speed):
        """
        Drives a forward zig-zag pattern
        """
        Motion.__init__(self)
        
        self._first = True
        self._legTime = legTime
        self._sweepAngle = sweepAngle
        self._speed = speed
        self._connections = []
        
    def _start(self):
        # Register to recieve AT_ORIENTATION events
        conn = self._eventHub.subscribe(control.IController.AT_ORIENTATION,
                                        self._controller, self._atOrientation)
        self._connections.append(conn)
        
        conn = self._eventHub.subscribeToType(ForwardZigZag.LEG_COMPLETE,
                                              self._legFinished)
        self._connections.append(conn)

        self._controller.yawVehicle(self._sweepAngle / 2.0)
        self._sweepAngle *= -1
        
    def _atOrientation(self, event):
        """
        AT_ORIENTATION event handler
        
        Called when the vehicle reaches the command orientation
        """
        legTime = self._legTime
        if self._first:
            legTime = legTime / 2.0
            self._first = False
        
        # Start the vehicle forward and create a timer to change the motion
        self._controller.setSpeed(self._speed)
        timer = ram.timer.Timer(self._eventPublisher, 
                                ForwardZigZag.LEG_COMPLETE, legTime)
        timer.start()
            
    def _legFinished(self, event):
        """
        Called when the leg timer expires
        """
        self._controller.setSpeed(0)
        self._controller.yawVehicle(self._sweepAngle)
        self._sweepAngle *= -1
    
    def stop(self):
        self._controller.setSpeed(0)
        for conn in self._connections:
            conn.disconnect()
