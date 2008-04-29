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
            speed = 5)

        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seek(state.State):
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_LOST : Searching,
                 vision.EventType.PIPE_FOUND : Seek,
                 vision.EventType.PIPE_CENTERED : FollowPath }

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
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()

class FollowPath(state.State):
    LOST_PATH = core.declareEventType('LOST_PATH')
    
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_LOST : FollowPath,
                 vision.EventType.PIPE_FOUND : FollowPath,
                 FollowPath.LOST_PATH : End }


    def PIPE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        angle = 0
        if event.x < 0.5 and event.y < 0.5:
            angle = event.angle
        self._pipe.setState(event.x, event.y, angle)
        
    def PIPE_LOST(self, event):
        """We have driving off the 'end' of the pipe"""
        self._timer = self.timerManager.newTimer(FollowPath.LOST_PATH, 15)
        self._timer.start()

    def enter(self):
        """Makes the vehicle follow along line outlined by the pipe"""
        self._pipe = ram.motion.pipe.Pipe(0,0,0)
        motion = ram.motion.pipe.Follow(pipe = self._pipe,
                                       maxSpeed = 2,
                                       maxSidewaysSpeed = 3)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        #print '"Pipe Follow"'
