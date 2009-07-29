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

# Standard imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek
import ram.timer

LIGHT_HIT = core.declareEventType('LIGHT_HIT')
COMPLETE = core.declareEventType('COMPLETE')

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

    @staticmethod
    def getattr():
        return set(['speed'])
    
    def enter(self):
        # Store the initial orientation
        orientation = self.vehicle.getOrientation()
        self.ai.data['lightStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self.ai.data['config'].get('lightDepth', 5),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)

        self.ai.data['firstSearching'] = True
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State, StoreLightEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Align }

    @staticmethod
    def getattr():
        return set(['legTime', 'sweepAngle', 'speed'])

    def enter(self):
        # Turn on the vision system
        self.visionSystem.redLightDetectorOn()
        
        # Set the start orientation if it isn't already set
        orientation = self.vehicle.getOrientation()
        direction = self.ai.data.setdefault('lightStartOrientation',
                                orientation.getYaw().valueDegrees())

        # Create the forward motion
        self._duration = self.ai.data['config'].get('Light', {}).get(
            'forwardDuration', 2)
        self._forwardSpeed = self.ai.data['config'].get('Light', {}).get(
            'forwardSpeed', 3)
        self._forwardMotion = motion.basic.TimedMoveDirection(
            desiredHeading = 0,
            speed = self._forwardSpeed,
            duration = self._duration,
            absolute = False)
        
        # Create zig zag search to 
        self._legTime = self._config.get('legTime', 15)
        self._sweepAngle = self._config.get('sweepAngle', 60)
        self._speed = self._config.get('speed', 2.5)
        self._zigZag = motion.search.ForwardZigZag(
            legTime = self._legTime,
            sweepAngle = self._sweepAngle,
            speed = self._speed,
            direction = direction)

        if self.ai.data.get('firstSearching', True) and self._duration > 0:
            self.motionManager.setQueuedMotions(self._forwardMotion,
                                                self._zigZag)
        else:
            self.motionManager.setMotion(self._zigZag)
        self.ai.data['firstSearching'] = False

    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, StoreLightEvent):
    @staticmethod
    def transitions():
        return state.FindAttempt.transitions(vision.EventType.LIGHT_FOUND,
                                             Align, Recover)
        
    def enter(self):
        state.FindAttempt.enter(self, timeout = 2)

class Recover(state.FindAttempt, StoreLightEvent):
    REFOUND_LIGHT = core.declareEventType('REFOUND_LIGHT')
    
    @staticmethod
    def transitions():
        trans = state.FindAttempt.transitions(Recover.REFOUND_LIGHT,
                                             Align, Searching)
        trans.update({ motion.basic.Motion.FINISHED : Recover,
                       vision.EventType.LIGHT_FOUND : Recover })
        
        return trans

    @staticmethod
    def getattr():
        return set(['yThreshold', 'reverseSpeed', 'advanceSpeed',
                    'closeDepthChange', 'depthChange', 'diveSpeed',
                    'yawChange', 'radius', 'closeRangeThreshold',
                    'farRangeThreshold']).union(state.FindAttempt.getattr())
    
    def FINISHED(self, event):
        self._finished = True
        
    def LIGHT_FOUND(self, event):
        StoreLightEvent.LIGHT_FOUND(self, event)

        if self._recoverMethod == "Close Range":
            # Turn off the timer and backwards motion
            if self.timer is not None:
                self.timer.stop()
                self.timer = None

                self.motionManager._stopMotion(self._recoverMotion)
                
                # Create the depth motion if needed
                newDepth = self.vehicle.getDepth()
                changeDepth = False
                if event.y > self._yThreshold:
                    newDepth = newDepth - self._closeDepthChange
                    changeDepth = True
                elif event.y < (0.0 - self._yThreshold):
                    newDepth = newDepth + self._closeDepthChange
                    changeDepth = True
                
                # Start the depth motion if necessary
                if changeDepth:
                    self._diveMotion = motion.basic.RateChangeDepth(
                        desiredDepth = newDepth, speed = self._diveSpeed)
                    self.motionManager.setMotion(self._diveMotion)
                    self._finished = False
                else:
                    self._finished = True
            
            # Check if the motion is finished
            if self._finished:
                self.publish(Recover.REFOUND_LIGHT, core.Event())
            
            # Check if we should finish it early
            if ((0.0 - self._yThreshold) < event.y < self._yThreshold):
                self.motionManager.stopCurrentMotion()
                self.controller.holdCurrentDepth()
                self.publish(Recover.REFOUND_LIGHT, core.Event())
        else:
            self.publish(Recover.REFOUND_LIGHT, core.Event())
        
    def enter(self):
        state.FindAttempt.enter(self, timeout = 4)
        
        event = self.ai.data.get('lastLightEvent', None)
        
        self._recoverMethod = "Default"
        
        # Load the thresholds for searching
        self._yThreshold = self._config.get('yThreshold', 0.05)
        self._reverseSpeed = self._config.get('reverseSpeed', 4)
        self._advanceSpeed = self._config.get('advanceSpeed', 1)
        self._closeDepthChange = self._config.get('closeDepthChange', 1)
        self._depthChange = self._config.get('depthChange', 1)
        self._diveSpeed = self._config.get('diveSpeed', 0.3)
        self._yawChange = self._config.get('yawChange', 15)
        self._radius = self._config.get('radius', .7)
        self._closeRangeThreshold = self._config.get('closeRangeThreshold', 5)
        self._farRangeThreshold = self._config.get('farRangeThreshold', 8)
        
        if event is None:
            self._recoverMethod = "None"
            self.motionManager.stopCurrentMotion()
        else:
            vectorLength = math.Vector2(event.x, event.y).length()
            vehicleOrientation = \
                self.vehicle.getOrientation().getYaw().valueDegrees()

            if event.range < self._closeRangeThreshold:
                # If the range is very close, backup and change depth
                # Find the backwards direction and create the motion
                self._recoverMethod = "Close Range"
                self._recoverMotion = \
                    motion.basic.MoveDirection(-180, self._reverseSpeed,
                                                absolute = False)
            
                # Set the backwards motion
                self.motionManager.setMotion(self._recoverMotion)
                self._finished = False
            elif vectorLength > self._radius and event.range < \
                    self._farRangeThreshold:
                self._recoverMethod = "Mid Range"
                yawAngle = 0.0
                if event.x > self._radius:
                    yawAngle = (0.0 - self._yawChange)
                elif event.x < (0.0 - self._radius):
                    yawAngle = self._yawChange
                self.controller.yawVehicle(yawAngle)
            
                # Change the depth if it's outside on the y-axis
                newDepth = self.vehicle.getDepth()
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
                self._recoverMethod = "Far Range"
                # If the range is far and inside radius, move forwards slowly
                desiredDirection = math.Degree(vehicleOrientation)
                recoverMotion = motion.basic.MoveDirection(
                    0, self._advanceSpeed, absolute = False)
                self.motionManager.setMotion(recoverMotion)
                self._finished = True
            else:
                # Otherwise, wait for a symbol before continuing
                self._finished = True
                self.motionManager.stopCurrentMotion()

    def exit(self):
        self.motionManager.stopCurrentMotion()
	self.controller.setSpeed(0)
	self.controller.setSidewaysSpeed(0)

class Align(state.State, StoreLightEvent):

    SEEK_LIGHT = core.declareEventType('SEEK_LIGHT')

    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : FindAttempt,
                 vision.EventType.LIGHT_FOUND : Align,
                 motion.seek.SeekPoint.POINT_ALIGNED : Align,
                 Align.SEEK_LIGHT : Seek }

    @staticmethod
    def getattr():
        return set(['depthGain', 'iDepthGain', 'dDepthGain', 'maxDepthDt',
                    'desiredRange', 'speed', 'alignmentThreshold',
                    'translate', 'translateGain', 'iTranslateGain',
                    'dTranslateGain', 'planeThreshold', 'kp', 'kd'])

    def POINT_ALIGNED(self, event):
        """Holds the current depth when we find we are aligned"""
        if self._depthGain != 0:
            self.controller.holdCurrentDepth()

    def _compareChange(self, pvalues, dvalues):
        x, y = pvalues
        dx, dy = dvalues

        if self._depthGain == 0:
            y = 0

        finalx = self._kp * abs(x) + self._kd * abs(dx)
        finaly = self._kp * abs(y) + self._kd * abs(dy)

        return finalx < self._planeThreshold and \
            finaly < self._planeThreshold

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        StoreLightEvent.LIGHT_FOUND(self, event)
        self._light.setState(event.azimuth, event.elevation, event.range,
                             event.x, event.y, event.timeStamp)

        change = self._light.changeOverTime()
        if self._compareChange((event.x, event.y),
                               (change[3], change[4])):
            self.publish(Align.SEEK_LIGHT, core.Event())

    def enter(self):
        self._kp = self._config.get('kp', 1.0)
        self._kd = self._config.get('kd', 1.0)
        self._light = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  timeStamp = None,
                                                  vehicle = self.vehicle,
                                                  kp = self._kp, kd = self._kd)
        self._planeThreshold = self._config.get('planeThreshold', 0.03)
        self._depthGain = self._config.get('depthGain', 3)
        iDepthGain = self._config.get('iDepthGain', 0.5)
        dDepthGain = self._config.get('dDepthGain', 0.5)
        maxDepthDt = self._config.get('maxDepthDt', 1)
        desiredRange = self._config.get('desiredRange', 5)
        speed = self._config.get('speed', 3)
        translate = self._config.get('translate', False)
        translateGain = self._config.get('translateGain', 1.0)
        iTranslateGain = self._config.get('iTranslateGain', 0)
        dTranslateGain = self._config.get('dTranslateGain', 0)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        motion = ram.motion.seek.SeekPointToRange(target = self._light,
                                                  alignmentThreshold = alignmentThreshold,
                                                  desiredRange = desiredRange,
                                                  maxRangeDiff = 5,
                                                  maxSpeed = speed,
                                                  depthGain = self._depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain,
                                                  maxDepthDt = maxDepthDt,
                                                  translate = translate,
                                                  translateGain = translateGain,
                                                iTranslateGain = iTranslateGain,
                                                dTranslateGain = dTranslateGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(state.State, StoreLightEvent):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : FindAttempt,
                 vision.EventType.LIGHT_FOUND : Seek,
                 vision.EventType.LIGHT_ALMOST_HIT : Hit }

    @staticmethod
    def getattr():
        return set(['depthGain', 'iDepthGain', 'dDepthGain', 'speed'])

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        StoreLightEvent.LIGHT_FOUND(self, event)
        self._light.setState(event.azimuth, event.elevation, event.range,
                             event.x, event.y, event.timeStamp)

    def enter(self):
        self._light = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  timeStamp = None,
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

    @staticmethod
    def getattr():
        return set(['duration', 'speed'])

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
        
class Continue(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.QUEUED_MOTIONS_FINISHED : End }

    @staticmethod
    def getattr():
        return set(['backwardSpeed', 'backwardDuration', 'upwardDepth',
                    'upwardSpeed', 'forwardSpeed', 'forwardDuration',
                    'turnSteps'])
    
    def enter(self):
        # Load config settings
        self._backwardSpeed = self._config.get('backwardSpeed', 3)
        self._backwardDuration = self._config.get('backwardDuration', 4)
        self._upwardDepth = self._config.get('upwardDepth', 2.5)
        self._upwardSpeed = self._config.get('upwardSpeed', 0.3)
        self._forwardSpeed = self._config.get('forwardSpeed', 3)
        self._forwardDuration = self._config.get('forwardDuration', 8)
        self._turnSteps = self._config.get('turnSteps', 1)

        # Load the original orientation
        original = self.ai.data.get('lightStartOrientation', None)
        
        # Create the motions
        if original is not None:
            self._rotate = motion.basic.ChangeHeading(desiredHeading = original,
                                                      steps = self._turnSteps)
        self._backward = motion.basic.TimedMoveDirection(desiredHeading = 180,
            speed = self._backwardSpeed, duration = self._backwardDuration,
            absolute = False)
        currentDepth = self.vehicle.getDepth()
        self._upward = motion.basic.RateChangeDepth(
            desiredDepth = currentDepth - self._upwardDepth,
            speed = self._upwardSpeed)
        self._forward = motion.basic.TimedMoveDirection(desiredHeading = 0,
            speed = self._forwardSpeed, duration = self._forwardDuration,
            absolute = False)
        
        # Queue the motions
        if original is None:
            self.motionManager.setQueuedMotions(self._backward, self._upward,
                                                self._forward)
        else:
            self.motionManager.setQueuedMotions(self._rotate, self._backward,
                                                self._upward, self._forward)

class End(state.State):
    def enter(self):
        self.motionManager.stopCurrentMotion()
        self.publish(COMPLETE, core.Event())
