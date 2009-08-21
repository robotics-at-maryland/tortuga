# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/sonar.py
  
# STD Imports
import math
  
# Project Imports
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device
import ext.math

import ram.ai.state as state
import ram.motion as motion
#import ram.motion.search
import ram.motion.pipe

COMPLETE = core.declareEventType('COMPLETE')

class PingerState(state.State):
    """
    Base state for the using the Sonar data to hover toward and over the Pinger.
    
    Base classes are required to update the actual target with new Sonar 
    information.
    """
    TIMEOUT = core.declareEventType('PINGER_TIMEOUT')

    @staticmethod
    def transitions(myState, timeoutState = None, trans = None):
        if timeoutState is None:
            timeoutState = PingerLost
        if trans is None:
            trans = {}
        trans.update({vehicle.device.ISonar.UPDATE : myState,
                      PingerState.TIMEOUT : timeoutState})
        return trans

    @staticmethod
    def getattr():
        return set(['closeZ', 'speedGain', 'yawGain', 'maxSpeed', 'timeout'])

    def UPDATE(self, event):
        if self._timeout > 0:
            self._pingChecker.stop()
            self._pingChecker = self.timerManager.newTimer(PingerState.TIMEOUT,
                                                           self._timeout)
            self._pingChecker.start()
    
    def _isNewPing(self, event):
        if self._lastTime != event.pingTimeUSec:
            self._lastTime = event.pingTimeUSec
            return True
        return False
    
    def _pingerAngle(self, event):
        pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
            event.direction)
        return pingerOrientation.getYaw(True)
    
    def _loadSettings(self):
        self._closeZ = self._config.get('closeZ', 0.85)
        self._speedGain = self._config.get('speedGain', 5)
        self._yawGain = self._config.get('yawGain', 1)
        self._maxSpeed = self._config.get('maxSpeed', 1)
        self._maxSidewaysSpeed = 0
        self._sidewaysSpeedGain = 0
     
    def enter(self, timeout = 2.5):
        self._pipe = ram.motion.pipe.Pipe(0, 0, 0, timeStamp = None)
        self._lastTime = 0

        self._loadSettings()

        motion = ram.motion.pipe.Hover(pipe = self._pipe, 
                                       maxSpeed = self._maxSpeed, 
                                       maxSidewaysSpeed = self._maxSidewaysSpeed, 
                                       sidewaysSpeedGain = self._sidewaysSpeedGain, 
                                       speedGain = self._speedGain, 
                                       yawGain = self._yawGain)
        self.motionManager.setMotion(motion)

        # Set up the ping timer
        self._pingChecker = None
        self._timeout = self._config.get('timeout', timeout)
        if self._timeout > 0:
            self._pingChecker = self.timerManager.newTimer(
                PingerState.TIMEOUT, self._timeout)
            self._pingChecker.start()

    def exit(self):
        if self._pingChecker is not None:
            self._pingChecker.stop()
        self.motionManager.stopCurrentMotion()
        
class Start(state.State):
    """
    Changes to the depth for proper sonar operation, then starts the search.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['diveSpeed', 'offset', 'minimumDepth'])

    def enter(self):
        offset = self._config.get('offset', 0)
        desiredDepth = self.ai.data['config'].get('sonarDepth', 2)

        # Check that it's not lower than the minimum depth
        desiredDepth -= offset
        minimumDepth = self._config.get('minimumDepth', 0.5)
        if desiredDepth < minimumDepth:
            desiredDepth = minimumDepth

        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = desiredDepth,
            speed = self._config.get('diveSpeed', 0.4))
        
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """
    Waits for a ping to happen, and when it gets one, turns toward the source.
    Then it waits for 4 seconds, to allow the any disturbances in pinger data 
    from the vehicles rotation to die down, and then moves on to the next state. 
    """
    CHANGE = core.declareEventType("CHANGE")

    @staticmethod
    def transitions():
        return { vehicle.device.ISonar.UPDATE : Searching,
	             Searching.CHANGE : FarSeeking }
        
    def UPDATE(self, event):
        if self._first:
            pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
                event.direction)
            self.controller.yawVehicle(pingerOrientation.getYaw(True).valueDegrees())
	   
	    self.timer = self.timerManager.newTimer(Searching.CHANGE, 4)
	    self.timer.start()
	    self._first = False

    def enter(self):
        self._first = True
        self.timer = None
        
    def exit(self):
        if self.timer is not None:
            self.timer.stop()

class TranslationSeeking(PingerState):
    """
    Feeds the pinger data into the target and causes the vehicle to translate
    toward the pinger.
    """
    CLOSE = core.declareEventType('CLOSE')

    @staticmethod
    def getattr():
        return set(['maxSidewaysSpeed', 'sidewaysSpeedGain']).union(
            PingerState.getattr())
    
    def _loadSettings(self):
        PingerState._loadSettings(self)
        self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 2)

    def UPDATE(self, event):
        PingerState.UPDATE(self, event)
        if self._isNewPing(event):
            # Converting from the vehicle reference frame, to the image space
            # reference frame used by the pipe motion
            self._pipe.setState(-event.direction.y, event.direction.x, 
                                ext.math.Degree(0), event.timeStamp)
            
            if math.fabs(event.direction.z) > math.fabs(self._closeZ):
                 self.publish(TranslationSeeking.CLOSE, core.Event())

# 0.65
class FarSeeking(TranslationSeeking):
    """
    Approaches the pinger from a far away range, it moves quicker, and can 
    possibly lose pings.  If this is used close to a pinger, it can and *will*
    overshoot.
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(FarSeeking, PingerLost,
            { TranslationSeeking.CLOSE : CloseSeeking } ) 

    @staticmethod
    def getattr():
        return set(['midRangeZ']).union(TranslationSeeking.getattr())
                 
    def _loadSettings(self):
        TranslationSeeking._loadSettings(self)
        self._closeZ = self._config.get('midRangeZ', 0.65)
                 
class CloseSeeking(TranslationSeeking):
    """
    For when we are close to the pinger, moves slow enough that it should not 
    miss any pings or overshoot but is too slow to use from far away.
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(CloseSeeking, PingerLostClose,
            { TranslationSeeking.CLOSE : End } ) 

    @staticmethod
    def getattr():
        return set(['closeZ']).union(TranslationSeeking.getattr())

    def _loadSettings(self):
        PingerState._loadSettings(self)
        self._closeZ = self._config.get('closeZ', 0.8)
    
class Hovering(TranslationSeeking):
    """
    Just hovers over the pinger
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(Hovering, PingerLostHovering)

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())

class PingerLost(state.FindAttempt):
    @staticmethod
    def transitions(foundState = FarSeeking, timeoutState = Searching):
        return state.FindAttempt.transitions(
            vehicle.device.ISonar.UPDATE, foundState, timeoutState)

    def enter(self):
        state.FindAttempt.enter(self)

        self.motionManager.stopCurrentMotion()

class PingerLostClose(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(CloseSeeking)

class PingerLostHovering(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(Hovering)
