# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/control.py

# Project Imports
import ext.core as core
import ext.control as control

import ram.sim.input as input

import event

event.add_event_types(['THRUST_FORE', 'THRUST_BACK', 'TURN_LEFT', 'TURN_RIGHT',
                       'DIVE', 'SURFACE'])

class KeyboardController(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'KeyboardController'))
        self._controller = control.IController.castTo(deps[0])
        self._desiredSpeed = self._controller.getSpeed()
        
        # Hook into input system
        watched_buttons = {'_left' : ['TURN_LEFT'],
                           '_right' : ['TURN_RIGHT'],
                           '_forward' : ['THRUST_FORE'],
                           '_backward' : ['THRUST_BACK'],
                           '_dive' : ['DIVE'],
                           '_surface' : ['SURFACE']}
        self.key_observer = input.ButtonStateObserver(self, watched_buttons)
        
    def update(self, time_since_last_frame):
        if self._left:
            self._controller.yawVehicle(30 * time_since_last_frame)
        elif self._right:
            self._controller.yawVehicle(-30 * time_since_last_frame)
        
        if self._forward:
            self._desiredSpeed += 2 * time_since_last_frame
            self._controller.setSpeed(int(self._desiredSpeed))
        elif self._backward:
            self._desiredSpeed -= 2 * time_since_last_frame
            self._controller.setSpeed(int(self._desiredSpeed))
            
        currentDepth = self._controller.getDepth()
        print 'current_depth',currentDepth
        if self._dive:
            currentDepth += 2 * time_since_last_frame
            self._controller.setDepth(currentDepth)
        elif self._surface:
            currentDepth -= 2 * time_since_last_frame
            self._controller.setDepth(currentDepth)
            
core.SubsystemMaker.registerSubsystem('KeyboardController', KeyboardController)