# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/pipe.py

"""
Currently hovers over the pipe
"""

# Project Imports
import ext.core as core
import ext.vision as vision

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.pipe

class Searching(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_FOUND : Seek }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.pipeLineDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 15,
            sweepAngle = 60,
            speed = 2.5)
        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_LOST : Searching,
                 vision.EventType.PIPE_FOUND : Seek}
                 #vision.EventType.LIGHT_ALMOST_HIT : Hit }

    def PIPE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._pipe.setState(event.x, event.y, event.angle)

    def enter(self):
        self._pipe = ram.motion.pipe.Pipe(0,0,0)
        motion = ram.motion.pipe.Hover(pipe = self._pipe,
                                       maxSpeed = 3,
                                       maxSidewaysSpeed = 3)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

#class Hit(state.State):
#    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
#    
#    @staticmethod
#    def transitions():
#        return {Hit.FORWARD_DONE : End}
#
#    def enter(self):
#        self.visionSystem.redLightDetectorOff()
#
#        print '"Attempting to hit light, Charge!!!!"'
#        # Timer goes off in 3 seconds then sends off FORWARD_DONE
#        timer = self.timerManager.newTimer(Hit.FORWARD_DONE, 3)
#        timer.start()
#    
#    def exit(self):
#        self.controller.setSpeed(0)
        
#class End(state.State):
#    def enter(self):
#        print '"Mission Complete"'
