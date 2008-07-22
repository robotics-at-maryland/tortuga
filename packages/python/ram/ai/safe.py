# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/safe.py


# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

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
        self._safe.setState(event.x, event.y)#, event.angle)

    def enter(self):
        self._safe = ram.motion.common.Target(0,0)

        speedGain = self._config.get('speedGain', 3)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',1)
        #yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('maxSpeed', 3)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 1)
        
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
        
class SafeCentering(SafeTrackingState):
    CENTERED = core.declareEventType('CENTERED')
    
    def SAFE_FOUND(self, event):
        SafeTrackingState.SAFE_FOUND(self, event)
        
        if math.Vector2(event.x, event.y).length() < self._centeredRange:
            self.publish(Seeking.CENTERED, core.Event())

            
    def enter(self):
        self._centeredRange = self._config.get('centertedRange', 0.2)
        SafeTrackingState.enter(self)
        
        
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
    
class Seeking(SafeCentering):
    """When the vehicle is moving to get over the safe"""
    
    @staticmethod
    def transitions():
        return SafeTrackingState.transitions(Seeking,
            { Seeking.CENTERED : Dive })
        
class Dive(SafeTrackingState):
    """Diving to the pre-grab depth"""

    @staticmethod
    def transitions():
        return SafeTrackingState.transitions(Dive,
            { ram.motion.basic.Motion.FINISHED : Offseting })
        
    def enter(self):
        safeDepth = self._config.get('safeDepth', 22)
        offset = self._config.get('depthOffset', 2)
        diveRate = self._config.get('diveRate', 0.4)
        
        targetDepth = safeDepth - offset
        diveMotion = motion.basic.RateChangeDepth(targetDepth, diveRate)
        self.motionManager.setMotion(diveMotion)
        
        SafeTrackingState.enter(self)
        
class Offseting(SafeCentering):
    """Moves the treasure under the grabber of the vehicle"""
    @staticmethod
    def transitions():
        return SafeCentering.transitions(Offseting,
            { SafeCentering.CENTERED : Settling })

    def SAFE_FOUND(self, event):
        event.y = event.y - self._offset
        SafeCentering.SAFE_FOUND(self, event)
        
    def enter(self):
        self._offset = self._config.get('offset', -0.7)
        SafeCentering.enter(self)
        
class Settling(SafeTrackingState):
    """Settles over the offseted safe in preperation for the grab"""
    
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return SafeTrackingState.transitions(Settling,
            { Settling.SETTLED : Grabbing })
        
    def SAFE_FOUND(self, event):
        event.y = event.y - self._offset
        SafeTrackingState.SAFE_FOUND(self, event)

    def enter(self):
        self.timer = self.timerManager.newTimer(Settling.SETTLED, 
                                                self._config.get('duration', 5))
        self.timer.start()
        
        self._offset = self._config.get('offset', -0.7)
        SafeTrackingState.enter(self)

    def exit(self):
        SafeTrackingState.exit(self)
        self.timer.stop()
        
class Grabbing(state.State):
    """Does the diving grab of the treasure"""
    
    GRABBED = core.declareEventType('GRABBED')
    
    @staticmethod
    def transitions():
        return {Grabbing.GRABBED : Surface,
                ram.motion.basic.Motion.FINISHED : Surface }
    
    def enter(self):
        # Timer to expire motion
        self.timer = self.timerManager.newTimer(Grabbing.GRABBED, 
                                                self._config.get('duration', 10))
        
        # Setup dive
        safeDepth = self._config.get('safeDepth', 22)
        offset = self._config.get('depthOffset', 2)
        diveRate = self._config.get('diveRate', 0.5)
        
        targetDepth = safeDepth + offset
        diveMotion = motion.basic.RateChangeDepth(targetDepth, diveRate)
        self.motionManager.setMotion(diveMotion)
        
        self.timer.start()

    def exit(self):
        self.timer.stop()
        
class Surface(state.State):
    """Diving to the pre-grab depth"""

    @staticmethod
    def transitions():
        return { ram.motion.basic.Motion.FINISHED : End }
        
    def enter(self):
        depth = self._config.get('depth', 0)
        diveRate = self._config.get('diveRate', 0.4)
        
        diveMotion = motion.basic.RateChangeDepth(depth, diveRate)
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
    
class End(state.State):
    pass
