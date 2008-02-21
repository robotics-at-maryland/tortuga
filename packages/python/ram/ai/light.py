# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/light.py

"""

"""

# Project Imports
import ext.core as core
import ext.vision as vision

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.seek

class Searching(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Seek }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.redLightDetectorOn()
        
        # Create zig zag search to 
        # 15 seconds long, 120 Degree sweep, speed 1
        zigZag = motion.search.ForwardZigZag(15, 120, 1)
        self.motionManager.setMotion(zigZag)
        

class Seek(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_LOST : Searching,
                 vision.EventType.LIGHT_FOUND : Seek
                 vision.EventType.LIGHT_ALMOST_HIT : Hit }

    def LIGHT_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._light.setState(event.azimuth, event.elevation, event.range)

    def enter(self):
        self._light = ram.motion.seek.PointTarget(0,0,0)
        motion = ram.motion.seek.SeekPoint(self._light)
        self.motionManager.setMotion(motion)


class Hit(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    
    @staticmethod
    def transitions():
        return {Hit.FORWARD_DONE : End}

    def enter(self):
        
        self.visionSystem.redLightDetectorOff()
        # Timer goes off in 3 seconds then sends off FORWARD_DONE
        timer = self.timerManager.newTimer(Hit.FORWARD_DONE, 3)
        timer.start()
    
    def exit(self):
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        print 'Mission Complete'
