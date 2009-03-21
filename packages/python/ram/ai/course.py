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

import ram.ai.task as task
import ram.ai.state as state
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.bin as bin
import ram.ai.safe as safe
import ram.ai.sonarSafe as sonarSafe
import ram.ai.sonar as sonar

import ram.motion as motion
import ram.motion.basic

class Gate(task.Task):
    """
    This State overseas the completion of the gate objective
    """
    @staticmethod
    def _transitions():
        return { gate.COMPLETE : task.Next,
                 vision.EventType.PIPE_FOUND : task.Next,
                 'GO' : state.Branch(gate.Dive) }

    def PIPE_FOUND(self, event):
        self.ai.data['foundPipeEarly'] = True

    def enter(self):
        task.Task.enter(self)
        
        self.ai.data['foundPipeEarly'] = False
        self.exited = False
        
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Dive))
        
        # Start up pipe detector
        self.visionSystem.pipeLineDetectorOn()
        
    def exit(self):
        task.Task.exit(self)
        
        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Dive)):
            self.stateMachine.stopBranch(gate.Dive)
    
class PipeGate(task.Task):
    """
    Find and hover over the first pipe in the course
    """
    
    @staticmethod
    def _transitions():
        return { pipe.Centering.SETTLED : task.Next,
                 'GATE' : state.Branch(pipe.Searching),
                 'PIPE' : state.Branch(pipe.Seeking) }
    
    def enter(self):
        task.Task.enter(self)
        
        # Branch off state machine for finding the pipe
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.start(state.Branch(pipe.Seeking))
        else:
            self.stateMachine.start(state.Branch(pipe.Searching))
        
    def exit(self):
        task.Task.enter(self)
        
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.stopBranch(pipe.Seeking)
        else:
            self.stateMachine.stopBranch(pipe.Searching)

        self.visionSystem.pipeLineDetectorOff()
        
class Light(task.Task):
    """
    Task for completion of the light objective within a certain timelimit.
    """
    @staticmethod
    def _transitions():
        return { light.LIGHT_HIT : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(light.Start) }
    
    def enter(self, defaultTimeout = 60):
        task.Task.enter(self, defaultTimeout = defaultTimeout)
        self.stateMachine.start(state.Branch(light.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(light.Start)
        self.visionSystem.redLightDetectorOff()
    
class LightStaged(Light):
    """
    Does the light task in such with intermediate timers, that attempt to quit
    early if the light is lost.
    """
    DO_TIMEOUT = core.declareEventType('DO_TIMEOUT_')
    
    @staticmethod
    def _transitions():
        trans = Light._transitions()
        trans.update({
            vision.EventType.LIGHT_FOUND : LightStaged,
            LightStaged.DO_TIMEOUT : task.Next })
        return trans
    
    def LIGHT_FOUND(self, event):
        # Stop old
        self._timer.stop()
        
        if self.doTimer is None:
            timeout = self._config.get('doTimeout', 25)
            self.doTimer = self.timerManager.newTimer(LightStaged.DO_TIMEOUT, 
                                                      timeout)
            self.doTimer.start()
            
    def enter(self):
        Light.enter(self, defaultTimeout = 40)
        
        # Set time to none
        self.doTimer = None
    
class Pipe(task.Task):
    """
    Find and hover a pipe in the course
    """
    
    @staticmethod
    def _transitions():
        return { pipe.Centering.SETTLED : task.Next,
                'GO' : state.Branch(pipe.Dive) }
    
    def enter(self):
        task.Task.enter(self)
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Dive))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(pipe.Dive)
        self.visionSystem.pipeLineDetectorOff()
    
class Bin(task.Task):
    @staticmethod
    def _transitions():
        return { bin.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(bin.Dive) }

    def enter(self):
        task.Task.enter(self)
        self.stateMachine.start(state.Branch(bin.Dive))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(bin.Dive)
        self.visionSystem.binDetectorOff()
    
class PingerDive(task.Task):
    """
    Changes to the depth for proper sonar operation
    """
    @staticmethod
    def _transitions():
        return { motion.basic.Motion.FINISHED : task.Next }

    def enter(self):
        task.Task.enter(self)
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 2),
            speed = self._config.get('diveSpeed', 0.4))
        
        self.motionManager.setMotion(diveMotion)
        
class Pinger(task.Task):
    """
    Move until we are over the pinger
    """
    @staticmethod
    def _transitions():
        return {  sonar.COMPLETE : task.Next,
                 'GO' : state.Branch(sonar.Searching) }
    
    def enter(self):
        task.Task.enter(self)
        # Branch off state machine for finding the sonar
        self.stateMachine.start(state.Branch(sonar.Searching))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(sonar.Searching)

class SafeDive(task.Task):
    """
    Dive to a depth at which we can activate the safe vision system.  The sonar
    system is active during this time.
    """
    @staticmethod
    def _transitions():
        return { motion.basic.Motion.FINISHED : task.Next,
                 'GO' : state.Branch(sonar.Hovering) } 

    def enter(self):
        task.Task.enter(self)
        # Activate the sonar system
        self.stateMachine.start(state.Branch(sonar.Hovering))
        
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 10),
            speed = self._config.get('diveSpeed', 0.3))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()
        self.stateMachine.stopBranch(sonar.Hovering)

class SafeVision(task.Task):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
        
    @staticmethod
    def _transitions():
        return { safe.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(safe.Searching) }
    
    def enter(self):
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        task.Task.enter(self, defaultTimeout = 60)
        self.stateMachine.start(state.Branch(safe.Searching))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(safe.Searching)
        self.visionSystem.downwardSafeDetectorOff()

class SafeSonar(task.Task):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
    
    @staticmethod
    def _transitions():
        return { safe.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(sonarSafe.Settling) }
    
    def enter(self):
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        task.Task.enter(self, defaultTimeout = 500)
        
        self.stateMachine.start(state.Branch(sonarSafe.Settling))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(sonarSafe.Settling)
        self.visionSystem.downwardSafeDetectorOff()

class Octagaon(task.Task):
    """
    Surface in the octagon with or without the treasure, but with the sonar on
    """
    @staticmethod
    def _transitions():
        return { motion.basic.Motion.FINISHED : task.Next }

    def enter(self):
        task.Task.enter(self)
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 0),
            speed = self._config.get('diveSpeed', 0.3))
        
        self.motionManager.setMotion(diveMotion)

    def stop(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()

class RecoverFromSafe(task.Task):
    """
    Gets us back in the octagon if the safe fails for some reason.
    
    Dives to good pinger depth, and activates the sonar
    """
    pass