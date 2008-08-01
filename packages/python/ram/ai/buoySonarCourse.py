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
    @staticmethod
    def transitions():
        return { gate.COMPLETE : Light,
                 'GO' : state.Branch(gate.Dive) }
        
    def enter(self):
        course.Gate.enter(self)
        
        self.visionSystem.pipeLineDetectorOff()
        
class Light(state.State):
    TIMEOUT = core.declareEventType('TIMEOUT')
    
    @staticmethod
    def transitions():
        return { light.LIGHT_HIT : PingerDive,
                 Light.TIMEOUT : PingerDive,
                 'GO' : state.Branch(light.Searching) }
    
    def enter(self):
        self.stateMachine.start(state.Branch(light.Searching))
        
        # Create out timeout
        timeout = self._config.get('timeout', 40)
        self.timer = self.timerManager.newTimer(Light.TIMEOUT, timeout)
        self.timer.start()
    
    def exit(self):
        self.stateMachine.stopBranch(light.Searching)
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
