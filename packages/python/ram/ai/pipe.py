# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/pipe.py


"""
A state machine for following the pipeline
 - Searches for a pipe
 - Centers over the first pipe, and waits to settle
 - Drives forward until it doesn't see the pipe
 - Goes back to the first step
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search
import ram.motion.pipe

class PipeFollowingState(state.State):
    @staticmethod
    def transitions(myState, trans = None):
        if trans is None:
            trans = {}
        trans.update({vision.EventType.PIPE_LOST : Searching,
                      vision.EventType.PIPE_FOUND : myState})
        return trans
    
    def PIPE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        self._pipe.setState(event.x, event.y, event.angle)

    def enter(self):
        self._pipe = ram.motion.pipe.Pipe(0,0,0)

        speedGain = self._config.get('speedGain', 5)
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain',3)
        yawGain = self._config.get('yawGain', 1)
        maxSpeed = self._config.get('forwardSpeed', 5)
        
        motion = ram.motion.pipe.Hover(pipe = self._pipe,
                                       maxSpeed = maxSpeed,
                                       maxSidewaysSpeed = 3,
                                       sidewaysSpeedGain = sidewaysSpeedGain,
                                       speedGain = speedGain,
                                       yawGain = yawGain)
        self.motionManager.setMotion(motion)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        self.motionManager.stopCurrentMotion()

class Dive(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }
    
    def enter(self):
        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 7),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """When the vehicle is looking for a pipe"""
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_FOUND : Seeking }

    def enter(self):
        # Turn on the vision system
        self.visionSystem.pipeLineDetectorOn()

        # Create zig zag search to 
        zigZag = motion.search.ForwardZigZag(
            legTime = 5,
            sweepAngle = 20,
            speed = self._config.get('forwardSpeed', 3))

        self.motionManager.setMotion(zigZag)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Seeking(PipeFollowingState):
    """When the vehicle is moving over the found pipe"""
    
    @staticmethod
    def transitions():
        return PipeFollowingState.transitions(Seeking,
            { vision.EventType.PIPE_CENTERED : Centering })

class Centering(PipeFollowingState):
    """
    When the vehicle is settling over the pipe
    
    @cvar SETTLED: Event fired when vehile has settled over the pipe
    """
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return PipeFollowingState.transitions(Centering,
            { Centering.SETTLED : AlongPipe })

    def enter(self):
        self.timer = self.timerManager.newTimer(Centering.SETTLED, 5)
        self.timer.start()
        
        PipeFollowingState.enter(self)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        PipeFollowingState.exit(self)
        self.timer.stop()

class AlongPipe(PipeFollowingState):
    """
    When the vehicle is following along a visible pipe
    """
    FOUND_NEW_PIPE = core.declareEventType('FOUND_NEW_PIPE')
    
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_LOST : BetweenPipes,
                 vision.EventType.PIPE_FOUND : AlongPipe,
                 AlongPipe.FOUND_NEW_PIPE : Seeking }


    def PIPE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        
        # Determine if a new pipe has appeared in the field of view
        newPipeLoc = ext.math.Vector3(event.x, event.y, 0)
        if self._lastPipeLoc is not None:
            if self._lastPipeLoc.distance(newPipeLoc) > 0.5:
                self.publish(AlongPipe.FOUND_NEW_PIPE, core.Event())
        self._lastPipeLoc = newPipeLoc
        
        # Update the targets state
        angle = ext.math.Degree(0)
        if event.x < 0.5 and event.y < 0.5:
            angle = event.angle
        event.angle = angle
        PipeFollowingState.PIPE_FOUND(self, event)

    def enter(self):
        """Makes the vehicle follow along line outlined by the pipe"""
        self._lastPipeLoc = None
        
        self._pipe = ram.motion.pipe.Pipe(0,0,0)
        maxSpeed = self._config.get('forwardSpeed', 5)
        motion = ram.motion.pipe.Follow(pipe = self._pipe,
                                        speedGain = 5,
                                        maxSpeed = maxSpeed,
                                        maxSidewaysSpeed = 3)
        self.motionManager.setMotion(motion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
       
class BetweenPipes(state.State):
    """
    When the vehicle is between two pipes, and can't see either.
    """
    LOST_PATH = core.declareEventType('LOST_PATH')
    
    @staticmethod
    def transitions():
        return {vision.EventType.PIPE_FOUND : Seeking,
                BetweenPipes.LOST_PATH : End }
    
    def enter(self):
        """We have driving off the 'end' of the pipe set a timeout"""
        self.timer = self.timerManager.newTimer(BetweenPipes.LOST_PATH, 15)
        self.timer.start()
        
        self.controller.setSpeed(self._config.get('forwardSpeed', 5))
        
    def exit(self):
        self.controller.setSpeed(0)
        self.timer.stop()
        
class End(state.State):
    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        #print '"Pipe Follow"'
