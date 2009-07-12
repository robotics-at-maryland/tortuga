# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/light.py
  
"""
A state machine for finding and hitting the red light
 - Search for light
 - Seeks light when found (goes back to search if lost)
 - Forward ram when close
 - Halts vehicle
 
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek

LIGHT_HIT = core.declareEventType('LIGHT_HIT')

class StoreLightEvent(object):
    """
    Common subclass for states that have a LIGHT_FOUND transition, it stores 
    the event in ai.data.
    """
    def LIGHT_FOUND(self, event):
        self.ai.data['lastLightEvent'] = event

class Start(state.State):
    """
    Does all the setup work for the light task.  Currently this involves just
    going to the proper depth to begin the search for the light.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }
    
    def enter(self):
        # Store the initial orientation
        orientation = self.vehicle.getOrientation()
        self.ai.data['lightStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 9.5),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State, StoreLightEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Align }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.redLightDetectorOn()
        
        # Set the start orientation if it isn't already set
        orientation = self.vehicle.getOrientation()
        self.ai.data.setdefault('lightStartOrientation',
                                orientation.getYaw().valueDegrees())
        
        # Create zig zag search to 
        self._legTime = self._config.get('legTime', 15)
        self._sweepAngle = self._config.get('sweepAngle', 60)
        self._speed = self._config.get('speed', 2.5)
        zigZag = motion.search.ForwardZigZag(
            legTime = self._legTime,
            sweepAngle = self._sweepAngle,
            speed = self._speed)
        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, StoreLightEvent):
    REFOUND_LIGHT = core.declareEventType('REFOUND_LIGHT')
    
    @staticmethod
    def transitions():
        trans = state.FindAttempt.transitions(FindAttempt.REFOUND_LIGHT,
                                             Align, Searching)
        trans.update({ motion.basic.Motion.FINISHED : FindAttempt,
                       vision.EventType.LIGHT_FOUND : FindAttempt })
        
        return trans
    
    def FINISHED(self, event):
        self._finished = True
        
    def LIGHT_FOUND(self, event):
        StoreLightEvent.LIGHT_FOUND(self, event)
        
        # Turn off the timer
        if self.timer is not None:
            self.timer.stop()
            self.timer = None
            
        # Check if the motion is finished
        if self._finished:
            self.publish(FindAttempt.REFOUND_LIGHT, core.Event())
            
        # Check if we should finish it early
        if ((0.0 - self._yThreshold) < event.y < self._yThreshold):
            self.motionManager.stopCurrentMotion()
            self.controller.holdCurrentDepth()
            self.publish(FindAttempt.REFOUND_LIGHT, core.Event())
            
        # Stop the backwards motion
        if self._recoverMotion is not None:
            self.motionManager._stopMotion(self._recoverMotion)
            self._recoverMotion = None
        
    def enter(self):
        state.FindAttempt.enter(self, timeout = 4)
        
        event = self.ai.data['lastLightEvent']
        
        vectorLength = math.Vector2(event.x, event.y).length()
        vehicleOrientation = self.vehicle.getOrientation().getYaw().valueDegrees()
        
        # Load the thresholds for searching
        self._yThreshold = self._config.get('yThreshold', 0.05)
        self._reverseSpeed = self._config.get('reverseSpeed', 4)
        self._advanceSpeed = self._config.get('advancedSpeed', 1)
        self._closeDepthChange = self._config.get('closeDepthChange', 1)
        self._depthChange = self._config.get('depthChange', 1)
        self._diveSpeed = self._config.get('diveSpeed', 0.3)
        self._yawChange = self._config.get('yawChange', 15)
        self._radius = self._config.get('radius', .7)
        self._closeRangeThreshold = self._config.get('closeRangeThreshold', 5)
        self._farRangeThreshold = self._config.get('farRangeThreshold', 8)
        
        # Create so the other FindAttempt motions won't crash
        self._recoverMotion = None
        
        if event.range < self._closeRangeThreshold:
            # If the range is very close, backup and change depth

            # Find the backwards direction and create the motion
            desiredDirection = math.Degree(vehicleOrientation + 180)
            self._recoverMotion = motion.basic.MoveDirection(desiredDirection,
                                                       self._reverseSpeed)
            
            # Find the current depth and create the motion
            newDepth = self.controller.getDepth()
            changeDepth = False
            if event.y > self._yThreshold:
                newDepth = newDepth - self._closeDepthChange
                changeDepth = True
            elif event.y < (0.0 - self._yThreshold):
                newDepth = newDepth + self._closeDepthChange
                changeDepth = True
            diveMotion = motion.basic.RateChangeDepth(desiredDepth = newDepth,
                                                      speed = self._diveSpeed)
            # Start necessary motions
            self.motionManager.setMotion(self._recoverMotion)
            if changeDepth:
                self.motionManager.setMotion(diveMotion)
                self._finished = False
            else:
                self._finished = True
        elif vectorLength > self._radius:
            # If the light is lost outside of the radius, turn towards it
            #currentDepth = self.controller.getDepth()
            #epthChange = motion.basic.RateChangeDepth(currentDepth + event.y,
            #                                           self._depthChangeSpeed)
            #self.motionManager.setMotion(depthChange)
            
            # Yaw the vehicle if it's outside on an x-axis
            yawAngle = 0.0
            if event.x > self._radius:
                yawAngle = (0.0 - self._yawChange)
            elif event.x < (0.0 - self._radius):
                yawAngle = self._yawChange
            self.controller.yawVehicle(yawAngle)
            
            # Change the depth if it's outside on the y-axis
            newDepth = self.controller.getDepth()
            if event.y > self._radius:
                newDepth = newDepth - self._depthChange
            elif event.y < (0.0 - self._radius):
                newDepth = newDepth + self._depthChange
            dive = motion.basic.RateChangeDepth(desiredDepth = newDepth,
                                                  speed = self._diveSpeed)
            self.motionManager.setMotion(dive)
            self._finished = False
        elif event.range > self._farRangeThreshold and \
                vectorLength < self._radius:
            # If the range is far and inside radius, move forwards slowly
            desiredDirection = math.Degree(vehicleOrientation)
            recoverMotion = motion.basic.MoveDirection(desiredDirection,
                                                       self._advanceSpeed)
            self.motionManager.setMotion(recoverMotion)
            self._finished = True
        else:
            # Otherwise, wait for a symbol before continuing
            self._finished = True
            self.motionManager.stopCurrentMotion()

class Align(state.State, StoreLightEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : FindAttempt,
                 vision.EventType.LIGHT_FOUND : Align,
                 ram.motion.seek.SeekPoint.POINT_ALIGNED : Seek }

    def POINT_ALIGNED(self, event):
        """Holds the current depth when we find we are aligned"""
        if self._depthGain != 0:
            self.controller.holdCurrentDepth()

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        StoreLightEvent.LIGHT_FOUND(self, event)
        self._light.setState(event.azimuth, event.elevation, event.range,
                             event.x, event.y)

    def enter(self):
        self._light = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  vehicle = self.vehicle)
        self._depthGain = self._config.get('depthGain', 3)
        iDepthGain = self._config.get('iDepthGain', 0.5)
        dDepthGain = self._config.get('dDepthGain', 0.5)
        maxDepthDt = self._config.get('maxDepthDt', 1)
        desiredRange = self._config.get('desiredRange', 5)
        speed = self._config.get('speed', 3)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        motion = ram.motion.seek.SeekPointToRange(target = self._light,
                                                  alignmentThreshold = alignmentThreshold,
                                                  desiredRange = desiredRange,
                                                  maxRangeDiff = 5,
                                                  maxSpeed = speed,
                                                  depthGain = self._depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain,
                                                  maxDepthDt = maxDepthDt)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(state.State, StoreLightEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : FindAttempt,
                 vision.EventType.LIGHT_FOUND : Seek,
                 vision.EventType.LIGHT_ALMOST_HIT : Hit }

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        StoreLightEvent.LIGHT_FOUND(self, event)
        self._light.setState(event.azimuth, event.elevation, event.range,
                             event.x, event.y)

    def enter(self):
        self._light = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  vehicle = self.vehicle)
        depthGain = self._config.get('depthGain', 0)
        iDepthGain = self._config.get('iDepthGain', 0)
        dDepthGain = self._config.get('dDepthGain', 0)
        speed = self._config.get('speed', 3)
        motion = ram.motion.seek.SeekPoint(target = self._light,
                                                  maxSpeed = speed,
                                                  depthGain = depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Hit(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Hit.FORWARD_DONE : Continue}

    def enter(self):
        self.visionSystem.redLightDetectorOff()

        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        duration = self._config.get('duration', 3)
        speed = self._config.get('speed', 3)
        self.timer = self.timerManager.newTimer(Hit.FORWARD_DONE, duration)
        self.timer.start()
        self.controller.setSpeed(speed)
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        self.publish(LIGHT_HIT, core.Event())
        
class Continue(state.MultiMotion):
    @staticmethod
    def transitions():
        return state.MultiMotion.transitions(Continue, End)
    
    def enter(self):
        # Load config settings
        self._backwardSpeed = self._config.get('backwardSpeed', 3)
        self._backwardDuration = self._config.get('backwardDuration', 4)
        self._upwardDepth = self._config.get('upwardDepth', 2.5)
        self._upwardSpeed = self._config.get('upwardSpeed', 0.3)
        self._forwardSpeed = self._config.get('forwardSpeed', 3)
        self._forwardDuration = self._config.get('forwardDuration', 8)
        self._turnSteps = self._config.get('turnSteps', 10)

        # Load the original orientation
        original = self.ai.data.get('lightStartOrientation', None)
        
        # Create the motions
        if original is not None:
            self._rotate = motion.basic.ChangeHeading(desiredHeading = original,
                                                      steps = self._turnSteps)
        self._backward = motion.basic.TimedMoveDirection(desiredHeading = 180,
            speed = self._backwardSpeed, duration = self._backwardDuration,
            absolute = False)
        currentDepth = self.controller.getDepth()
        self._upward = motion.basic.RateChangeDepth(
            desiredDepth = currentDepth - self._upwardDepth,
            speed = self._upwardSpeed)
        self._forward = motion.basic.TimedMoveDirection(desiredHeading = 0,
            speed = self._forwardSpeed, duration = self._forwardDuration,
            absolute = False)
        
        # Create the MultiMotion
        if original is None:
            state.MultiMotion.enter(self, self._backward, self._upward,
                                    self._forward)
        else:
            state.MultiMotion.enter(self, self._rotate, self._backward,
                                    self._upward, self._forward)

class End(state.State):
    def enter(self):
        self.motionManager.stopCurrentMotion()
