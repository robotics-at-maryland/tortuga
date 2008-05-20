# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/support.py

"""
Provides support classes for the testing of the AI subsystem
"""

# Python Imports
import unittest

# Project Imports
import ext.core as core
import ram.ai.state as state
import ram.timer as timer
import ram.motion as motion
import ram.motion.basic

from ram.test.timer import TimerTester
from ram.test.motion.support import MockController, MockVehicle


class MockMotionManager(core.Subsystem):
    def __init__(self):
        core.Subsystem.__init__(self, 'MotionManager')
        self.currentMotion = None
        self.stopped = False
    
    def setMotion(self, motion):
        self.stopped = False
        self.currentMotion = motion
        
    def stopCurrentMotion(self):
        self.stopped = True
        
        
class AITestCase(TimerTester):
    def setUp(self, extraDeps = None, cfg = None):
        TimerTester.setUp(self)
        
        if extraDeps is None:
            extraDeps = []
            
        if cfg is None:
            cfg = {}
        
        self.eventHub = core.EventHub()
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        self.timerManager = timer.TimerManager(deps = [self.eventHub])
        self.controller = MockController(None)
        self.vehicle = MockVehicle()
        
        deps = [self.controller, self.timerManager, self.eventHub, 
                self.qeventHub, self.vehicle]
        
        self.motionManager = motion.basic.MotionManager(cfg, deps)
        deps.append(self.motionManager)
        
        deps.extend(extraDeps)
        self.machine = state.Machine(deps = deps)
    
    def tearDown(self):
        TimerTester.tearDown(self)
        self.machine.stop()
        
    def injectEvent(self, etype, eclass = core.Event, *args, **kwargs):
        """
        Sends and event of the desired type and class into the state machine
        
        @type etype: str
        @param etype: The event type to give the event
        
        @type eclass: Subclass of ext.core.Event
        @param eclass: The class of the event object
        """
        
        if kwargs.has_key('type'):
            raise ValueError, "Keyword arguments cannot contain 'type'"
        
        kwargs['type'] = etype
        event = eclass(*args)
        
        # Set desired class attributes
        for attr, val in kwargs.iteritems():
            setattr(event, attr, val)
        
        self.machine.injectEvent(event)
        
    def releaseTimer(self, timer):
        """
        Stops a spinning background timer, joins it, then releases its event
        """
        assert self.timerBlock, "Must set self.timerBlock to be true"
        
        # Stops the background loop
        self.timerBlock = False
        timer.join()
        # Releases the event the timer published when the loop stopped
        self.qeventHub.publishEvents()
        
    def assertCurrentState(self, stateType):
        """
        Compares the type of the current state machine state to the given one
        """
        self.assertEquals(stateType, type(self.machine.currentState()))
        
    def assertCurrentMotion(self, motionType):
        """
        Compares the type of the current motion to the given one
        """
        self.assertEquals(motionType, type(self.motionManager.currentMotion))
        
    def assertLessThan(self, a, b):
        if not (a < b):
            self.fail(str(a) + " is not less than " + str(b))
            
    def assertGreaterThan(self, a, b):
        if not (a > b):
            self.fail(str(a) + " is not greater than " + str(b))
