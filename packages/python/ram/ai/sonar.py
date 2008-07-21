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

class PingerState(state.State):
    @staticmethod
    def transitions(myState, trans = None):
        if trans is None:
            trans = {}
        trans.update({vehicle.device.ISonar.UPDATE : myState})
        return trans
    
    def _isNewPing(self, event):
        if self._lastTime != event.pingTimeSec:
            self._lastTime = event.pingTimeSec
            return True
        return False
    
    def _pingerAngle(self, event):
        pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
            event.direction)
        return pingerOrientation.getYaw(True)
    
    def _loadSettings(self):
        self._closeZ = self._config.get('closeZ', 0.8)
        self._speedGain = self._config.get('speedGain', 5)
        self._yawGain = self._config.get('yawGain', 1)
        self._maxSpeed = self._config.get('forwardSpeed', 1)
        self._maxSidewaysSpeed = 0
        self._sidewaysSpeedGain = 0
    
    def enter(self):
        self._pipe = ram.motion.pipe.Pipe(0, 0, 0)
        self._lastTime = 0

        self._loadSettings()
        
        motion = ram.motion.pipe.Hover(pipe = self._pipe, 
                                       maxSpeed = self._maxSpeed, 
                                       maxSidewaysSpeed = self._maxSidewaysSpeed, 
                                       sidewaysSpeedGain = self._sidewaysSpeedGain, 
                                       speedGain = self._speedGain, 
                                       yawGain = self._yawGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()
        

class Searching(state.State):
    @staticmethod
    def transitions():
        return { vehicle.device.ISonar.UPDATE : CloseSeeking }
        
    def UPDATE(self, event):
        pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
            event.direction)
        self.controller.yawVehicle(pingerOrientation.getYaw(True).valueDegrees())

class FarSeeking(PingerState):
    CLOSE = core.declareEventType('CLOSE')
    
    @staticmethod
    def transitions():
        return PingerState.transitions(FarSeeking, 
            { FarSeeking.CLOSE : End } ) 
    
    def UPDATE(self, event):
        if self._isNewPing(event):
            # x = 0, no sideways motion
            # y = 1, full forward speed
            self._pipe.setState(0, 1, self._pingerAngle(event))
            
            if math.fabs(event.direction.z) > math.fabs(self._closeZ):
                 self.publish(FarSeeking.CLOSE, core.Event())
                 
class CloseSeeking(PingerState):
    
    @staticmethod
    def transitions():
        return PingerState.transitions(CloseSeeking)
    
    def _loadSettings(self):
        PingerState._loadSettings(self)
        self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 2)
    
    def UPDATE(self, event):
        if self._isNewPing(event):
            # Converting from the vehicle reference frame, to the image space
            # reference frame used by the pipe motion
            self._pipe.setState(-event.direction.y, event.direction.x, 
                                ext.math.Degree(0))
    

class End(state.State):
    def enter(self):
        print "End"
