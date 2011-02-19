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
        self._motionManager = core.Subsystem.getSubsystemOfType(
            motion.MotionManager, deps)

        self._time_since_last_motion = 0
        self._MOTION_RATE = .1 # seconds in between motions

        self._depthAdjust = 0
        self._lastDepth = self._controller.getDesiredDepth()
        self._velocityAdjust = math.Vector2.ZERO


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
        self._time_since_last_motion += time_since_last_frame
        
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
        estPosition = self._stateEstimator.getEstimatedPosition()
        estVelocity = self._stateEstimator.getEstimatedVelocity()
        desVelocity_n = self._controller.getDesiredVelocity()
        yaw = self._stateEstimator.getEstimatedOrientation().getYaw().valueRadians()

        desVelocity_b = self.nRb(-yaw) * desVelocity_n
        vIncForeward_b = math.Vector2(2 * time_since_last_frame, 0)
        vIncForeward_n = self.nRb(yaw) * vIncForeward_b
        
        if self._forward:
            newVelocity_b = desVelocity_b + vIncForeward_b
            if newVelocity_b[0] < 5 and newVelocity_b[1] < 5:
                desVelocity_b = self.nRb(yaw) * newVelocity_b

        elif self._backward:
            newVelocity_b = desVelocity_b - vIncForeward_b
            if newVelocity_b[0] > -5 and newVelocity_b[1] > -5:
                desVelocity_b = self.nRb(yaw) * newVelocity_b

        # SidewaysSpeed Control
        vIncSideways_b = math.Vector2(0, 2 * time_since_last_frame)
        vIncSideways_n = self.nRb(yaw) * vIncSideways_b

        if self._strafe_right:
            newVelocity_b = desVelocity_b + vIncSideways_b
            if newVelocity_b[0] < 5 and newVelocity_b[1] < 5:
                desVelocity_b = self.nRb(yaw) * newVelocity_b

        elif self._strafe_left:
            newVelocity_b = desVelocity_b - vIncSideways_b
            if newVelocity_b[0] > -5 and newVelocity_b[1] > -5:
                desVelocity_b = self.nRb(yaw) * newVelocity_b

        desVelocity_n = self.nRb(yaw) * desVelocity_b
        
        # Depth Control
        if self._dive:
            self._depthAdjust += 2 * time_since_last_frame

        elif self._surface:
            self._depthAdjust -= 2 * time_since_last_frame


        if self._time_since_last_motion > self._MOTION_RATE:
            # do the translate motion
            self._motionManager.setMotion(
                motion.Translate(trajectories.Vector2CubicTrajectory(
                        initialValue = estPosition,
                        finalValue = estPosition,
                        initialTime = timer.time(),
                        initialRate = estVelocity,
                        finalRate = desVelocity_n,
                        maxRate = 5)))

            # do the depth motion (only if there was a change from the keyboard controller)
            if pmath.fabs(self._depthAdjust) > 0:
                self._motionManager.setMotion(
                    motion.ChangeDepth(trajectories.ScalarCubicTrajectory(
                            initialValue = self._stateEstimator.getEstimatedDepth(),
                            finalValue = self._lastDepth + self._depthAdjust,
                            initialTime = timer.time(),
                            initialRate = self._stateEstimator.getEstimatedDepthRate(),
                            finalRate = 0,
                            maxRate = 2))) 
                self._lastDepth += self._depthAdjust
                self._depthAdjust = 0

            # reset
            self._time_since_last_motion = 0


        if self._stop:
            self._controller.holdCurrentPosition()
            self._controller.holdCurrentOrientation()
            self._controller.holdCurrentDepth()

            self._lastDepth = self._controller.getDesiredDepth()
            self._depthAdjust = 0
            
core.SubsystemMaker.registerSubsystem('KeyboardController', KeyboardController)
