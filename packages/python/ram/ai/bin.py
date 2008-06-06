# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/bin.py

"""
Currently hovers over the bin
"""

# Project Imports
import ext.core as core
import ext.vision as vision

import ram.ai.state as state
import ram.motion as motion
import ram.motion.basic
import ram.motion.search
import ram.motion.common

class HoveringState(state.State):
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._bin.setState(event.x, event.y)

    def enter(self):
        self._bin = ram.motion.common.Target(0,0)
        motion = ram.motion.common.Hover(target = self._bin,
                                         maxSpeed = 5,
                                         maxSidewaysSpeed = 3)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()

class SettlingState(HoveringState):
    def enter(self, eventType, eventTime):
        self.timer = self.timerManager.newTimer(eventType, eventTime)
        self.timer.start()
        
        HoveringState.enter(self)

    def exit(self):
        HoveringState.exit(self)
        self.timer.stop()

class Searching(state.State):
    """When the vehicle is looking for a bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Seeking }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.binDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 5)

        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seeking(HoveringState):
    """When the vehicle is moving over the found bin"""
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : Seeking,
                 vision.EventType.BIN_CENTERED : Centering }

class Centering(SettlingState):
    """
    When the vehicle is settling over the bin
    
    @cvar SETTLED: Event fired when vehile has settled over the bin
    """
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : Centering,
                 Centering.SETTLED : Dive }
    
    def enter(self):
        SettlingState.enter(self, Centering.SETTLED, 5)
    
class Dive(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : Dive,
                 motion.basic.Motion.FINISHED : DropMarker }
        
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        #self._bin.setState(event.x, event.y)
        pass
        
    def enter(self):
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 8),
            speed = self._config.get('diveSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(diveMotion)
        
#        self._bin = ram.motion.common.Target(0,0)
#        motion = ram.motion.common.Hover(target = self._bin,
#                                         maxSpeed = 5,
#                                         maxSidewaysSpeed = 3)
#        self.motionManager.setMotion(motion)
        
class DropMarker(SettlingState):
    DROPPED = core.declareEventType('DROPPPED')
    
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : DropMarker,
                 DropMarker.DROPPED : Surface }

    def enter(self):
        SettlingState.enter(self, DropMarker.DROPPED, 5)
        # TODO: drop marker here
        
class Surface(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_LOST : Searching,
                 vision.EventType.BIN_FOUND : Surface,
                 motion.basic.Motion.FINISHED : End }
        
    def BIN_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        #self._bin.setState(event.x, event.y)
        pass
        
    def enter(self):
        surfaceMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 3),
            speed = self._config.get('surfaceSpeed', 1.0/3.0))
        
        self.motionManager.setMotion(surfaceMotion)
        
#        self._bin = ram.motion.common.Target(0,0)
#        motion = ram.motion.common.Hover(target = self._bin,
#                                         maxSpeed = 5,
#                                         maxSidewaysSpeed = 3)
#        self.motionManager.setMotion(motion)
        
class End(state.State):
    def enter(self):
        self.visionSystem.binDetectorOff()
