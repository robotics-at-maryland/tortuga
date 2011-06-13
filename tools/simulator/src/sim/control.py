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
import ext.network as network
import ext.vehicle as vehicle
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
    TARGET_VELOCITY_UPDATE = core.declareEventType('TARGET_VELOCITY_UPDATE')
    TARGET_DEPTH_UPDATE = core.declareEventType('TARGET_DEPTH_UPDATE')

    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'KeyboardController'))

        self._controller = core.Subsystem.getSubsystemOfType(
            control.IController, deps)
        self._stateEstimator = core.Subsystem.getSubsystemOfType(
            estimation.IStateEstimator, deps)
        self._motionManager = core.Subsystem.getSubsystemOfType(
            motion.MotionManager, deps)

        self._time_since_last_motion = 0
        self._MOTION_RATE = 0.1 # seconds in between motions

        self._targetVelocity_b = self._stateEstimator.getEstimatedVelocity()
        self._targetDepth = self._stateEstimator.getEstimatedDepth()

        self._newDepth = False
        self._newVelocity = False

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
        self._time_since_last_motion += time_since_last_frame
        
        # Try to stop wherever we currently are.
        if self._stop:
            self._controller.holdCurrentPosition()
            self._controller.holdCurrentOrientation()
            self._controller.holdCurrentDepth()
            self._targetDepth = self._controller.getDesiredDepth()
            self._targetVelocity_b = math.Vector2.ZERO
            self._newVelocity = True
            self._newDepth = True
            return 
        
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
        

        # increment or decrement the foreward velocity in body coordinates
        vIncForeward_b = math.Vector2(0.2 * time_since_last_frame,
                                      0)
        
        if self._forward:
            self._targetVelocity_b += vIncForeward_b
            self._newVelocity = True

        elif self._backward:
            self._targetVelocity_b -= vIncForeward_b
            self._newVelocity = True

        # increment or decrement the sideways velocity in body coordinates
        vIncStarboard_b = math.Vector2(0,
                                       0.2 * time_since_last_frame)

        if self._strafe_right:
            self._targetVelocity_b += vIncStarboard_b
            self._newVelocity = True

        elif self._strafe_left:
            self._targetVelocity_b -= vIncStarboard_b
            self._newVelocity = True

        
        # Depth Control
        if self._dive:
            self._targetDepth += 2 * time_since_last_frame
            self._newDepth = True

        elif self._surface:
            self._targetDepth -= 2 * time_since_last_frame
            self._newDepth = True

        # only update every once in a while so we are not computing 
        # new trajectories at absurd rates.  
        if self._time_since_last_motion > self._MOTION_RATE:
            
            if self._newVelocity:
                # calculate the current velocity in body coordinates
                orientation = self._stateEstimator.getEstimatedOrientation()
                yaw = orientation.getYaw().valueRadians()
                estVelocity_n = self._stateEstimator.getEstimatedVelocity()
                estVelocity_b = math.bRn(yaw) * estVelocity_n
                desVelocity_n = self._controller.getDesiredVelocity()
                desVelocity_b = math.bRn(yaw) * desVelocity_n
                # do the translate motion
                self._motionManager.setMotion(
                    motion.Translate(
                        trajectories.Vector2ConstAccelTrajectory(
                            initialValue = math.Vector2.ZERO,
                            initialRate = estVelocity_b,
                            finalRate = self._targetVelocity_b,
                            accel = 0.2),
                        frame = motion.Frame.LOCAL))
                vEvent = core.Event()
                vEvent.velocity = math.nRb(yaw) * self._targetVelocity_b
                self.publish(KeyboardController.TARGET_VELOCITY_UPDATE, vEvent)
                
            if self._newDepth:
                estDepth = self._stateEstimator.getEstimatedDepth()
                estDepthRate = self._stateEstimator.getEstimatedDepthRate()
                desDepth = self._controller.getDesiredDepth()
                desDepthRate = self._controller.getDesiredDepthRate()
                # do the depth motion
                self._motionManager.setMotion(
                    motion.ChangeDepth(trajectories.ScalarCubicTrajectory(
                            initialValue = estDepth,
                            finalValue = self._targetDepth,
                            initialRate = estDepthRate,
                            avgRate = 0.2))) 
                dEvent = core.Event()
                dEvent.depth = self._targetDepth
                self.publish(KeyboardController.TARGET_DEPTH_UPDATE, dEvent)

            self._newDepth = False
            self._newVelocity = False
            # reset update timer
            self._time_since_last_motion = 0

            
core.SubsystemMaker.registerSubsystem('KeyboardController', KeyboardController)
