# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Jonathan Speiser <jspeiser@umd.edu>

# All rights reserved.
#
# Author: Jonathan Speiser <jspeiser@umd.edu>
# File:  packages/python/ram/ai/square.py

"""
A state machine to go in a square pattern:
 - Waits for Switch
 - Dives to depth
 - Goes Forward/Turns in a square pattern
 - Surfaces
 
Reguires the following subsystems:
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
import ext.control as control

class Wait(state.State):
    @staticmethod
    def transitions():
        # TODO: Replace with a start event from the vehicle or sensor board
        return {"Start" : Dive}
    
class Dive(state.State):
    @staticmethod
    def transitions():
        return {motion.basic.Motion.FINISHED : Forward}
    
    def enter(self):
        Forward.NUMTURNS = 0
        # Go to 5 feet in 5 increments
        diveMotion = motion.basic.ChangeDepth(5, 5)
        self.motionManager.setMotion(diveMotion)
    
class Forward(state.State):
    FORWARD_DONE = core.declareEventType('FORWARD_DONE')
    FORWARD_CONT = core.declareEventType('FORWARD_CONT')
    NUMTURNS = 0
	
    @staticmethod
    def transitions():
        return {Forward.FORWARD_DONE : Surface , Forward.FORWARD_CONT : Turn}

    def enter(self):
        #print "PONG"
        # Full speed ahead!!
        Forward.NUMTURNS += 1
        self.controller.setSpeed(3)
        
        # Timer goes off in 10 seconds then sends off FORWARD_DONE
        if Forward.NUMTURNS > 3:
            timer = self.timerManager.newTimer(Forward.FORWARD_DONE, 10)
        else:
            timer = self.timerManager.newTimer(Forward.FORWARD_CONT, 10)
        timer.start()
    
    def exit(self):
        self.controller.setSpeed(0)
		
class Turn(state.State):
    TURN_DONE = core.declareEventType('TURN_DONE') #use 
    @staticmethod
    def transitions():
	    return { Turn.TURN_DONE : Forward }

    def enter(self):
        self._conn = self.motionManager._eventHub.subscribe(
            control.IController.AT_ORIENTATION,
            self.controller, self._turnDone)
        
        self.controller.yawVehicle(90)
        #print "ping"

    def _turnDone(self,event):
        event = ext.core.Event()
        self.publish(Turn.TURN_DONE, event)

class Surface(state.State):
    @staticmethod
    def transitions():
        return {motion.basic.Motion.FINISHED : End}     
    
    def enter(self):  
        # Go to 0 feet in 5 increments
        surfaceMotion = motion.basic.ChangeDepth(0, 5)
        self.motionManager.setMotion(surfaceMotion)
        
class End(state.State):
    def enter(self):
        print 'Mission Complete'
