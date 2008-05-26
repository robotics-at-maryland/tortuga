# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/gate.py

"""
A state machine which controls the vehicle through the course. Currently it
assumes the vehicle can find all the pipes, but allows missing objectives other
then the pipes.
 
 
Requires the following subsystems:
 - 
"""

# Project Imports
import ext.core as core
import ram.ai.state as state
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light

class Gate(state.State):
    """
    This State overseas the completion of the gate objective
    """
    @staticmethod
    def transitions():
        return { gate.COMPLETE : Pipe1 }
    
    def enter(self):
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Dive))
        
    def exit(self):
        if (self.stateMachine.branches.has_key(gate.Dive)):
            self.stateMachine.stopBranch(gate.Dive)
    
class Pipe1(state.State):
    """
    Find and hover over the first pipe in the course
    """
    
    @staticmethod
    def transitions():
        return { pipe.Centering.SETTLED : Light }
    
    def enter(self):
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Searching))
        
    def exit(self):
        self.stateMachine.stopBranch(pipe.Searching)
        self.visionSystem.pipeLineDetectorOff()
        
class Light(state.State):
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { light.LIGHT_HIT : Pipe2,
                 Light.TIMEOUT : Pipe2 }
    
    def enter(self):
        self.stateMachine.start(state.Branch(light.Searching))
        
        # Create out timeout
        self.timer = self.timerManager.newTimer(Light.TIMEOUT, 60)
        self.timer.start()
    
    def exit(self):
        self.stateMachine.stopBranch(light.Searching)
        self.visionSystem.redLightDetectorOff()
    
class Pipe2(state.State):
    """
    Find and hover over the second pipe in the course
    """
    
    @staticmethod
    def transitions():
        return { pipe.Centering.SETTLED : End }
    
    def enter(self):
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Searching))
        
    def exit(self):
        self.stateMachine.stopBranch(pipe.Searching)
        self.visionSystem.pipeLineDetectorOff()
    
    
class End(state.End):
    pass