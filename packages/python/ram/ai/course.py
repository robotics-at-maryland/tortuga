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
import ext.vision as vision

import ram.ai.state as state
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.bin as bin
import ram.ai.safe as safe
import ram.ai.sonar as sonar

import ram.motion as motion
import ram.motion.basic

class Gate(state.State):
    """
    This State overseas the completion of the gate objective
    """
    @staticmethod
    def transitions():
        return { gate.COMPLETE : Pipe1,
                 vision.EventType.PIPE_FOUND : Pipe1,
                 'GO' : state.Branch(gate.Dive) }

    def PIPE_FOUND(self, event):
        self.ai.data['foundPipeEarly'] = True

    def enter(self):
        self.ai.data['foundPipeEarly'] = False
        self.exited = False
        
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Dive))
        
        # Start up pipe detector
        self.visionSystem.pipeLineDetectorOn()
        
    def exit(self):
        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Dive)):
            self.stateMachine.stopBranch(gate.Dive)
    
class Pipe1(state.State):
    """
    Find and hover over the first pipe in the course
    """
    
    @staticmethod
    def transitions():
        return { pipe.Centering.SETTLED : Light,
                 'GATE' : state.Branch(pipe.Searching),
                 'PIPE' : state.Branch(pipe.Seeking) }
    
    def enter(self):
        # Branch off state machine for finding the pipe
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.start(state.Branch(pipe.Seeking))
        else:
            self.stateMachine.start(state.Branch(pipe.Searching))
        
    def exit(self):
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.stopBranch(pipe.Seeking)
        else:
            self.stateMachine.stopBranch(pipe.Searching)

        self.visionSystem.pipeLineDetectorOff()
        
class Light(state.State):
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { light.LIGHT_HIT : Pipe2,
                 Light.TIMEOUT : Pipe2,
                 'GO' : state.Branch(light.Searching) }
    
    def enter(self):
        self.stateMachine.start(state.Branch(light.Searching))
        
        # Create out timeout
        self.timer = self.timerManager.newTimer(Light.TIMEOUT,
                                                self._config.get('timeout',60))
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
        return { pipe.Centering.SETTLED : Bin,
                'GO' : state.Branch(pipe.Dive) }
    
    def enter(self):
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Dive))
        
    def exit(self):
        self.stateMachine.stopBranch(pipe.Dive)
        self.visionSystem.pipeLineDetectorOff()
    
class Bin(state.State):
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { bin.COMPLETE : Pipe3,
                 Bin.TIMEOUT : Pipe3,
                 'GO' : state.Branch(bin.Dive) }

    def enter(self):
        self.stateMachine.start(state.Branch(bin.Dive))
        
        # Create out timeout
        timeout = self._config.get('timeout',240)
        self.timer = self.timerManager.newTimer(Bin.TIMEOUT, timeout)
                  
        self.timer.start()
    
    def exit(self):
        self.stateMachine.stopBranch(bin.Dive)
        self.visionSystem.binDetectorOff()

class Pipe3(state.State):
    """
    Find and hover over the second pipe in the course
    """
    
    @staticmethod
    def transitions():
        return { pipe.Centering.SETTLED : PingerDive,
                 'GO' : state.Branch(pipe.Dive) }
    
    def enter(self):
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Dive))
        
    def exit(self):
        self.stateMachine.stopBranch(pipe.Dive)
        self.visionSystem.pipeLineDetectorOff()
    
class PingerDive(state.State):
    """
    Changes to the depth for proper sonar operation
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Pinger }

    def enter(self):
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 2),
            speed = self._config.get('diveSpeed', 0.4))
        
        self.motionManager.setMotion(diveMotion)
        
class Pinger(state.State):
    """
    Move until we are over the pinger
    """
    @staticmethod
    def transitions():
        return {  sonar.COMPLETE : SafeDive,
                 'GO' : state.Branch(sonar.Searching) }
    
    def enter(self):
        # Branch off state machine for finding the sonar
        self.stateMachine.start(state.Branch(sonar.Searching))
        
    def exit(self):
        self.stateMachine.stopBranch(sonar.Searching)

class SafeDive(state.State):
    """
    Dive to a depth at which we can activate the safe vision system.  The sonar
    system is active during this time.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Safe,
                 'GO' : state.Branch(sonar.Hovering) } 

    def enter(self):
        # Activate the sonar system
        self.stateMachine.start(state.Branch(sonar.Hovering))
        
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 10),
            speed = self._config.get('diveSpeed', 0.3))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
        self.stateMachine.stopBranch(sonar.Hovering)

class Safe(state.State):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
    
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { safe.COMPLETE : Octagaon,
                 Safe.TIMEOUT : Octagaon,
                 'GO' : state.Branch(safe.Searching) }
    
    def enter(self):
        self.stateMachine.start(state.Branch(safe.Searching))
        
        # Create out timeout
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        timeout = self._config.get('timeout', 60)
        self.timer = self.timerManager.newTimer(Safe.TIMEOUT, timeout)
               
        self.timer.start()
    
    def exit(self):
        self.stateMachine.stopBranch(safe.Searching)
        self.visionSystem.downwardSafeDetectorOff()

class RecoverFromSafe(state.State):
    """
    Gets us back in the octagon if the safe fails for some reason.
    
    Dives to good pinger depth, and activates the sonar
    """
    pass

class Octagaon(state.State):
    """
    Surface in the octagon with or without the treasure, but with the sonar on
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : End }

    def enter(self):
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 0),
            speed = self._config.get('diveSpeed', 0.3))
        
        self.motionManager.setMotion(diveMotion)

    def stop(self):
        self.motionManager.stopCurrentMotion()

class End(state.End):
    pass
