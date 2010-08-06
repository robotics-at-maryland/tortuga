# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/ai/gen2/course.py

"""
A state machine which controls the vehicle through the course. Currently it
assumes the vehicle can find all the pipes, but allows missing objectives other then the pipes.
"""

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.control as control

import ram.ai.task as task
import ram.ai.state as state
#import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.gen2.light as light
#import ram.ai.barbedwire as barbedwire
#import ram.ai.target as target
#import ram.ai.bin as bin
#import ram.ai.randombin as randombin
#import ram.ai.safe as safe
#import ram.ai.sonarSafe as sonarSafe
#import ram.ai.sonar as sonar

import ram.motion as motion
import ram.motion.basic

class Light(task.Task):
    """
    Task for completion of the light objective within a certain timelimit. It
    also remembers its initial orientation and returns to that orientation 
    before moving onto the next state.
    """

    @staticmethod
    def _transitions(thisState = None):
        if thisState is None:
            thisState = Light
        return { light.COMPLETE : task.Next,
                 light.FAILURE : task.Failure,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(light.Start) }

    def enter(self, defaultTimeout = 90):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(light.Start))
    
    def exit(self):
        task.Task.exit(self)
        
        self.stateMachine.stopBranch(light.Start)
        self.visionSystem.redLightDetectorOff()
        self.motionManager.stopCurrentMotion()

class Pipe(task.Task):
    """
    Find and hover a pipe in the course
    """
    
    COMPLETE = core.declareEventType('COMPLETE')
    
    @staticmethod
    def _transitions(currentState = None):
        if currentState is None:
            currentState = Pipe
        return { pipe.Centering.SETTLED : currentState,
                 Pipe.COMPLETE : task.Next,
                'GO' : state.Branch(pipe.Start) }

    @staticmethod
    def getattr():
        return set(['pipesToFind']).union(task.Task.getattr())
        
    def SETTLED(self, event):
        """
        Sends a complete event after enough pipes have been found
        """
        
        # We found a pipe increment the count
        self._pipeCount += 1
        
        if self._pipeCount >= self._pipesToFind:
            # We found enough pipes move on
            self.publish(Pipe.COMPLETE, core.Event())
    
    def enter(self, defaultTimeout = 10):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self._pipesToFind = self._config.get('pipesToFind', 1)
        self._pipeCount = 0
        self.ai.data['pipeBiasDirection'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'biasDirection', None)
        self.ai.data['pipeThreshold'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'threshold', None)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Start))
        
    def exit(self):
        del self.ai.data['pipeBiasDirection']
        del self.ai.data['pipeThreshold']
        
        task.Task.exit(self)
        self.stateMachine.stopBranch(pipe.Start)
        self.visionSystem.pipeLineDetectorOff()
        
    @property
    def pipesToFind(self):
        return self._pipesToFind

class Pipe1(Pipe):
    @staticmethod
    def _transitions():
        return Pipe._transitions(Pipe1)
