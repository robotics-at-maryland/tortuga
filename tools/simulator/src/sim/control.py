# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/control.py

"""
This module uses the ram.sim.input package to turn key events into commands to
an for an IController.
"""

# Project Imports
import ext.core as core
import ext.control as control

import ram.sim.input as input
import ram.motion.basic as motion
import ram.motion.trajectories as trajectories
import ram.timer as timer

import ram.event as event

event.add_event_types(['THRUST_FORE', 'THRUST_BACK', 'TURN_LEFT', 'TURN_RIGHT',
                       'DIVE', 'SURFACE', 'PITCH_UP', 'PITCH_DOWN', 
                       'ROLL_PORT', 'ROLL_STARBOARD', 'STRAFE_RIGHT',
                       'STRAFE_LEFT', 'STOP'])

class KeyboardController(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'KeyboardController'))
#        self._controller = control.IController.castTo(deps[0])
        self._controller = deps[0]
        self._desiredSpeed = self._controller.getSpeed()
        self._desiredSidewaysSpeed = self._controller.getSidewaysSpeed()
        
        # Hook into input system
        watched_buttons = {'_left' : ['TURN_LEFT'],
                           '_right' : ['TURN_RIGHT'],
                           '_forward' : ['THRUST_FORE'],
                           '_backward' : ['THRUST_BACK'],
                           '_dive' : ['DIVE'],
                           '_surface' : ['SURFACE'],
                           '_pitch_up' : ['PITCH_UP'],
                           '_pitch_down' : ['PITCH_DOWN'],
                           '_roll_port' : ['ROLL_PORT'],
                           '_roll_starboard' : ['ROLL_STARBOARD'],
                           '_strafe_right' : ['STRAFE_RIGHT'],
                           '_strafe_left' : ['STRAFE_LEFT'],
                           '_stop' : ['STOP']}
        self.key_observer = input.ButtonStateObserver(self, watched_buttons)
        
    def backgrounded(self):
        return False

    def unbackground(self, join = False):
        pass

    def background(self):
        pass
    
    def update(self, time_since_last_frame):
        # Turn (Yaw) Control
        if self._left:
            self._controller.yawVehicle(30 * time_since_last_frame, 0)
        elif self._right:
            self._controller.yawVehicle(-30 * time_since_last_frame, 0)
        
        # Pitch Control
        if self._pitch_up:
            self._controller.pitchVehicle(30 * time_since_last_frame, 0)
        elif self._pitch_down:
            self._controller.pitchVehicle(-30 * time_since_last_frame, 0)
        
        # Roll Control
        if self._roll_port:
            self._controller.rollVehicle(30 * time_since_last_frame, 0)
        elif self._roll_starboard:
            self._controller.rollVehicle(-30 * time_since_last_frame, 0)
        
        # Speed Control
        if self._desiredSpeed > 5:
            self._desiredSpeed = 5
        elif self._desiredSpeed < -5:
            self._desiredSpeed = -5
            
        if self._forward:
            self._desiredSpeed += 2 * time_since_last_frame
            self._controller.setSpeed(self._desiredSpeed)
        elif self._backward:
            self._desiredSpeed -= 2 * time_since_last_frame
            self._controller.setSpeed(self._desiredSpeed)

        # SidewaysSpeed Control
        if self._desiredSidewaysSpeed > 5:
            self._desiredSidewaysSpeed = 5
        elif self._desiredSidewaysSpeed < -5:
            self._desiredSidewaysSpeed = -5
            
        if self._strafe_right:
            self._desiredSidewaysSpeed += 2 * time_since_last_frame
            self._controller.setSidewaysSpeed(self._desiredSidewaysSpeed)
        elif self._strafe_left:
            self._desiredSidewaysSpeed -= 2 * time_since_last_frame
            self._controller.setSidewaysSpeed(self._desiredSidewaysSpeed)
            
        # Depth Control
        currentDepth = self._controller.getDepth()
        depthRate = self._estimator.getEstimatedDepthRate()
        
        if self._dive:
            newDepth = currentDepth + 2 * time_since_last_frame
        elif self._surface:
            newDepth = currentDepth - 2 * time_since_last_frame

        trajectory = trajectories.ScalarCubicTrajectory(
            initialValue = currentDepth, finalValue = newDepth,
            initialRate = depthRate, finalRate = 0,
            maxRate = 3, initialTime = timer.time())
            
        if self._stop:
            self._controller.holdCurrentPosition()
            
core.SubsystemMaker.registerSubsystem('KeyboardController', KeyboardController)
