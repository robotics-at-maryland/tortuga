# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/safe.py


# Project Imports
import ext.core as core
import ext.vision as vision

import ram.ai.state as state
import ram.motion as motion
#import ram.motion.search
import ram.motion.common

class SafeTrackingState(state.State):
    @staticmethod
    def transitions(myState, trans = None):
        if trans is None:
            trans = {}
        trans.update({vision.EventType.SAFE_LOST : Searching,
                      vision.EventType.SAFE_FOUND : myState})
        return trans
    
    def SAFE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._pipe.setState(event.x, event.y)#, event.angle)

    def enter(self):
        self._safe = ram.motion.common.Target(0,0)

        speedGain = self._config.get('speedGain', 5)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',3)
        #yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('maxSpeed', 5)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 3)
        
        motion = ram.motion.common.Hover(target = self._safe,
                                         maxSpeed = maxSpeed,
                                         maxSidewaysSpeed = maxSidewaysSpeed,
                                         sidewaysSpeedGain = sidewaysSpeedGain,
                                         speedGain = speedGain)
                                       #yawGain = yawGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()
        
class Searching(state.State):
    """When the vehicle is looking for the treasure"""
    @staticmethod
    def transitions():
        return { vision.EventType.SAFE_FOUND : Seeking }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.downwardSafeDetectorOn()

        # Create zig zag search to 
#        zigZag = motion.search.ForwardZigZag(
#            legTime = 5,
#            sweepAngle = 20,
#            speed = self._config.get('forwardSpeed', 3))
#
#        self.motionManager.setMotion(zigZag)

#    def exit(self):
#        self.motionManager.stopCurrentMotion()
    
class Seeking(SafeTrackingState):
    """When the vehicle is moving over the found pipe"""
    
    @staticmethod
    def transitions():
        return SafeTrackingState.transitions(Seeking)
            