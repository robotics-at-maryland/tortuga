# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/motion/support.py

"""
Provides support classes for the testing of the AI subsystem
"""

# Python Imports
import unittest

# Project Imports
import ext.core as core
import ram.ai.state as state
import ram.timer as timer

from ram.test.timer import TimerTester
from ram.test.motion.support import MockController


class MockMotionManager(core.Subsystem):
    def __init__(self):
        core.Subsystem.__init__(self, 'MotionManager')
        self.currentMotion = None
    
    def setMotion(self, motion):
        self.currentMotion = motion
        
        
class AITestCase(TimerTester):
    def setUp(self):
        TimerTester.setUp(self)
        
        self.eventHub = core.EventHub()
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        self.timerManager = timer.TimerManager(deps = [self.eventHub])
        self.motionManager = MockMotionManager()
        self.controller = MockController(None)
        
        deps = [self.motionManager, self.controller, self.timerManager,
                self.eventHub, self.qeventHub]
        self.machine = state.Machine(deps = deps)
    
    def tearDown(self):
        TimerTester.tearDown(self)
        self.machine.stop()
        
    def injectEvent(self, etype, eclass = core.Event, **kwargs):
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
        event = eclass()
        
        # Set desired class attributes
        for attr, val in kwargs.iteritems():
            setattr(event, attr, val)
        
        self.machine.injectEvent(event)
        
    def releaseTimer(self, timer):
        """
        Stops a spinning background timer, joins it, then releases its event
        """
        assert self.block, "Must set self.block to be true"
        
        # Stops the background loop
        self.block = False
        timer.join()
        # Releases the event the timer published when the loop stopped
        self.qeventHub.publishEvents()