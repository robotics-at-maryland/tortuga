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
import ram.ai.sonarSafe as sonarSafe
import ram.ai.sonar as sonar
import ram.ai.safe as safe

import ram.motion as motion
import ram.motion.basic
import ram.ai.course as course

class Gate(course.Gate):
    """
    This State overseas the completion of the gate objective
    """
    PIPE_ON = core.declareEventType('PIPE_ON_')
    
    @staticmethod
    def transitions():
        return { gate.COMPLETE : Light,
                 vision.EventType.PIPE_FOUND : Pipe,
                 Gate.PIPE_ON : Gate,
                 'GO' : state.Branch(gate.Dive) }
        
    def PIPE_ON_(self, event):
        """Turn pipe detector on after a delay"""
        self.visionSystem.pipeLineDetectorOn()
        
    def enter(self):
        course.Gate.enter(self)
        
        # Setup timer to trigger pipe detector after a certain delay
        delay = self._config.get('pipeDelay', 30)
        self.timer = self.timerManager.newTimer(Gate.PIPE_ON, delay)
        self.timer.start()
        
        # Save current heading
        self.ai.data['gateOrientation'] = \
            self.controller.getDesiredOrientation()
        
        self.visionSystem.pipeLineDetectorOff()
        
    def exit(self):
        course.Gate.exit(self)
        self.timer.stop()
       
class Pipe(state.State):
    """
    Find and hover over the second pipe in the course
    """
    TIMEOUT = core.declareEventType('TIMEOUT_')
    DO_TIMEOUT = core.declareEventType('DO_TIMEOUT_')
    MOVE_ON = core.declareEventType('MOVE_ON')
    
    @staticmethod
    def transitions():
        return { vision.EventType.PIPE_LOST : Pipe,
                 vision.EventType.PIPE_FOUND : Pipe,
                 Pipe.TIMEOUT : Pipe,
                 Pipe.DO_TIMEOUT : Pipe,
                 pipe.Centering.SETTLED : Light,
                 Pipe.MOVE_ON : Light,
                'GO' : state.Branch(pipe.Dive) }
    def PIPE_LOST(self, event):
        self._moveOn()
        
    def PIPE_FOUND(self, event):
        # Stop old
        self.timer.stop()
        
        if self.doTimer is None:
            timeout = self._config.get('doTimeout', 20)
            self.doTimer = self.timerManager.newTimer(Pipe.DO_TIMEOUT, 
                                                      timeout)
            self.doTimer.start()
        
        
    def TIMEOUT_(self, event):
        self._moveOn()
        
    def DO_TIMEOUT_(self, event):
        self._moveOn()
        
    def _moveOn(self):
        # Go back to the gates orientation
        self.controller.setDesiredOrientation(self.ai.data['gateOrientation'])
        
        # Move on to the Light hitting task
        self.publish(Pipe.MOVE_ON, core.Event())
    
    def enter(self):
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Dive))
        
        timeout = self._config.get('aquireTimeout', 20)
        self.timer = self.timerManager.newTimer(Pipe.TIMEOUT, timeout)
        self.timer.start()
        
        # Set default value do timer, so we only create it once
        self.doTimer = None
        
    def exit(self):
        self.stateMachine.stopBranch(pipe.Dive)
        self.visionSystem.pipeLineDetectorOff()
        
class Light(state.State):
    TIMEOUT = core.declareEventType('TIMEOUT')
    DO_TIMEOUT = core.declareEventType('DO_TIMEOUT_')
    
    @staticmethod
    def transitions():
        return { vision.EventType.LIGHT_FOUND : Light,
                 light.LIGHT_HIT : PingerDive,
                 Light.TIMEOUT : PingerDive,
                 Light.DO_TIMEOUT : PingerDive,
                 'GO' : state.Branch(light.Dive) }
    
    def LIGHT_FOUND(self, event):
        # Stop old
        self.timer.stop()
        
        if self.doTimer is None:
            timeout = self._config.get('doTimeout', 25)
            self.doTimer = self.timerManager.newTimer(Light.DO_TIMEOUT, 
                                                      timeout)
            self.doTimer.start()
    
    def enter(self):
        self.stateMachine.start(state.Branch(light.Dive))
        
        # Create out timeout
        timeout = self._config.get('timeout', 40)
        self.timer = self.timerManager.newTimer(Light.TIMEOUT, timeout)
        self.timer.start()
        
        # Set time to none
        self.doTimer = None
    
    def exit(self):
        self.stateMachine.stopBranch(light.Dive)
        self.visionSystem.redLightDetectorOff()

class PingerDive(course.PingerDive):
    """
    Changes to the depth for proper sonar operation
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Pinger }

class Pinger(course.Pinger):
    """
    Move until we are over the pinger
    """
    @staticmethod
    def transitions():
        return {  sonar.COMPLETE : Safe,
                 'GO' : state.Branch(sonar.Searching) }

class Safe(course.Safe):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
    
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { safe.COMPLETE : Octagaon,
                 Safe.TIMEOUT : Octagaon,
                 'GO' : state.Branch(sonarSafe.Settling) }
    
    def enter(self):
        self.stateMachine.start(state.Branch(sonarSafe.Settling))
        
        # Create out timeout
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        timeout = self._config.get('timeout', 500)
        self.timer = self.timerManager.newTimer(Safe.TIMEOUT, timeout)
               
        self.timer.start()
    
    def exit(self):
        self.stateMachine.stopBranch(sonarSafe.Settling)
        self.visionSystem.downwardSafeDetectorOff()

class Octagaon(course.Octagaon):
    """
    Surface in the octagon with or without the treasure, but with the sonar on
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : End,
                 'GO' : state.Branch(sonar.Searching) } 

class End(state.End):
    pass
