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

class FarSeeking(state.State):
    CLOSE = core.declareEventType('CLOSE')
    
    @staticmethod
    def transitions():
        return {vehicle.device.ISonar.UPDATE : FarSeeking,
                FarSeeking.CLOSE : End } 
    
    def UPDATE(self, event):
        """Update the state of the light, this moves the vehicle"""
        pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
            event.direction)
        
        if self._lastTime != event.pingTimeSec:
            self._lastTime = event.pingTimeSec
            # x = 0, no sideways motion
            # y = 1, full forward speed
            self._pipe.setState(0, 1, pingerOrientation.getYaw(True))
        
            if math.fabs(event.direction.z) > math.fabs(self._closeZ):
                 self.publish(FarSeeking.CLOSE, core.Event())

    def enter(self):
        self._pipe = ram.motion.pipe.Pipe(0, 0, 0)
        self._lastTime = 0

        self._closeZ = self._config.get('closeZ', 0.8)
        speedGain = self._config.get('speedGain', 5)
        yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('forwardSpeed', 1)
        
        motion = ram.motion.pipe.Hover(pipe = self._pipe, 
                                       maxSpeed = maxSpeed, 
                                       maxSidewaysSpeed = 0, 
                                       sidewaysSpeedGain = 0, 
                                       speedGain = speedGain, 
                                       yawGain = yawGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        print "End"
