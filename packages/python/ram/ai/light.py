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

import ram.filter as filter
import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek

LIGHT_HIT = core.declareEventType('LIGHT_HIT')


class Start(state.State):
    """
    Does all the setup work for the light task.  Currently this involves just
    going to the proper depth to begin the search for the light.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }
    
    def enter(self):
        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 9.5),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()



class Searching(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Align }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.redLightDetectorOn()

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
        
class Align(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : Searching,
                 vision.EventType.LIGHT_FOUND : Align,
                 ram.motion.seek.SeekPoint.POINT_ALIGNED : Seek }

    def POINT_ALIGNED(self, event):
        """Holds the current depth when we find we are aligned"""
        self.controller.holdCurrentDepth()

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._light.setState(event.azimuth, event.elevation, event.range,
                             event.x, event.y)

    def enter(self):
        self._light = ram.motion.seek.PointTarget(0, 0, 0, 0, 0,
                                                  vehicle = self.vehicle)
        depthGain = self._config.get('depthGain', 3)
        iDepthGain = self._config.get('iDepthGain', 0.5)
        dDepthGain = self._config.get('dDepthGain', 0.5)
        maxDepthDt = self._config.get('maxDepthDt', 0.3)
        desiredRange = self._config.get('desiredRange', 5)
        speed = self._config.get('speed', 3)
        alignmentThreshold = self._config.get('alignmentThreshold', 0.1)
        motion = ram.motion.seek.SeekPointToRange(target = self._light,
                                                  alignmentThreshold = alignmentThreshold,
                                                  desiredRange = desiredRange,
                                                  maxRangeDiff = 5,
                                                  maxSpeed = speed,
                                                  depthGain = depthGain,
                                                  iDepthGain = iDepthGain,
                                                  dDepthGain = dDepthGain,
                                                  maxDepthDt = maxDepthDt)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : Searching,
                 vision.EventType.LIGHT_FOUND : Seek,
                 vision.EventType.LIGHT_ALMOST_HIT : Hit }

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
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
        return {Hit.FORWARD_DONE : End}

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
        
class End(state.State):
    def enter(self):
        self.publish(LIGHT_HIT, core.Event())
