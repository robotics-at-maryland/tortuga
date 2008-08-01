# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/gate.py

"""
A state machine to go through the gate:
 - Waits for Switch
 - Dives to depth
 - Goes forward for a time
 
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# TODO: Add some way for required subsystems to be checked more throughly

# Project Imports
import ext.core as core

import ram.ai.state as state
import ram.motion as motion
import ram.motion.search

# Denotes when this state machine finishes
COMPLETE = core.declareEventType('COMPLETE')

class Wait(state.State):
    @staticmethod
    def transitions():
        # TODO: Replace with a start event from the vehicle or sensor board
        return {"Start" : Dive}
    
class Dive(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Forward }
    
    def enter(self):
        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 5),
            speed = self._config.get('speed', 1.0/3.0))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()
    
class Forward(state.State):
    DONE = core.declareEventType('DONE')

    @staticmethod
    def transitions():
        return {Forward.DONE : End}

    def enter(self):
        # Full speed ahead!!
        self.controller.setSpeed(self._config.get('speed',3))
        
        # Timer goes off in X seconds then sends off DONE
        self.timer = self.timerManager.newTimer(
            eventType = Forward.DONE, 
            duration = self._config.get('time', 10))
        self.timer.start()
    
    def exit(self):
        self.timer.stop()
        self.controller.setSpeed(0)
        
class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())