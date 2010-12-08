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
import math as pmath

# Project Imports
import ext.core as core
import ext.math as math
import ext.control as control
import ext.estimation as estimation


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

        self._controller = core.Subsystem.getSubsystemOfType(
            control.IController, deps)
        self._stateEstimator = core.Subsystem.getSubsystemOfType(
            estimation.IStateEstimator, deps)

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
    
    def nRb(self, yaw):
        r_cos = pmath.cos(yaw)
        r_sin = pmath.sin(yaw)
        return math.Matrix2(r_cos, r_sin, -r_sin, r_cos)

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
        velocity = self._controller.getDesiredVelocity()
        position = self._stateEstimator.getEstimatedPosition()
        yaw = self._stateEstimator.getEstimatedOrientation().getYaw().valueRadians()

        vIncForeward_b = math.Vector2(2 * time_since_last_frame, 0)
        vIncForeward_n = self.nRb(yaw) * vIncForeward_b
        
        if self._forward:
            newVelocity = velocity + vIncForeward_n
            if newVelocity[0] < 5 and newVelocity[1] < 5:
                self._controller.translate(position, newVelocity)
        elif self._backward:
            newVelocity = velocity - vIncForeward_n
            if newVelocity[0] > -5 and newVelocity[1] > -5:
                self._controller.translate(position, newVelocity)

        # SidewaysSpeed Control
        vIncSideways_b = math.Vector2(0, 2 * time_since_last_frame)
        vIncSideways_n = self.nRb(yaw) * vIncSideways_b

        if self._strafe_right:
            newVelocity = velocity + vIncSideways_n
            if newVelocity[0] < 5 and newVelocity[1] < 5:
                self._controller.translate(position, newVelocity)
        elif self._strafe_left:
            newVelocity = velocity - vIncSideways_n
            if newVelocity[0] > -5 and newVelocity[1] > -5:
                self._controller.translate(position, newVelocity)
            
        # Depth Control
        currentDepth = self._controller.getDesiredDepth()
        
        if self._dive:
            newDepth = currentDepth + 2 * time_since_last_frame
            self._controller.changeDepth(newDepth, 0)
        elif self._surface:
            newDepth = currentDepth - 2 * time_since_last_frame
            self._controller.changeDepth(newDepth, 0)

        if self._stop:
            self._controller.holdCurrentPosition()
            self._controller.holdCurrentOrientation()
            self._controller.holdCurrentDepth()
            
core.SubsystemMaker.registerSubsystem('KeyboardController', KeyboardController)
