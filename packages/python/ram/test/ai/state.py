# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/state.py


# STD Imports
import unittest

# Project Imports
import ext.core as core
import ram.ai.state as state

class Reciever(object):
    def __init__(self):
        self.event = None
        self.called = False
    
    def __call__(self, event):
        self.event = event
        self.called = True

# Test States (Consider Magic base class to take care of the init method)
class TrackedState(state.State):
    def __init__(self, machine):
        state.State.__init__(self, machine)
        self.entered = False
        self.exited = False

    def enter(self):
        self.entered = True

    def exit(self):
        self.exited = True

class Start(TrackedState):
    def __init__(self, machine):
        TrackedState.__init__(self, machine)
        self.event = None
        self.func = None
        
    @staticmethod
    def transitions():
        return { "Start" : End,
                 "Change" : Simple,
                 "LoopBack" : LoopBack }
 #                "Stuck" : state.PushState(Unstick) }

    def Start(self, event):
        self.event = event

#class Unstick(TrackedState):
#    

class Simple(state.State):
    pass

class LoopBack(TrackedState):
    def __init__(self, machine):
        TrackedState.__init__(self, machine)
        self.transCount = 0
        self.enterCount = 0

    def enter(self):
        TrackedState.enter(self)
        self.enterCount += 1

    @staticmethod
    def transitions():
        return { "Update" : LoopBack }

    def Update(self, event):
        self.transCount += 1

class End(TrackedState):
    pass

class TestStateMachine(unittest.TestCase):
    def setUp(self):
        self.machine = state.Machine()
        self.machine.start(Start)

    def _makeEvent(self, etype, **kwargs):
        event = core.Event()
        event.type = etype
        for key, value in kwargs.iteritems():
            setattr(event, key, value)
        return event

    def testbasic(self):
        # Check to make sure we get the default
        cstate = self.machine.currentState()
        self.assertEquals(Start, type(cstate))

    def testStart(self):
        cstate = self.machine.currentState()
        self.assert_(cstate.entered)
        self.assertFalse(cstate.exited)
        
    def testInjectEvent(self):
        startState = self.machine.currentState()

        self.machine.injectEvent(self._makeEvent("Start", value = 1))
        cstate = self.machine.currentState()

        # Check to me sure we left the start state
        self.assert_(startState.exited)

        # Check to make sure we reached the proper state
        self.assertEquals(End, type(cstate))
        self.assert_(cstate)

        # Make sure the transition function was called
        self.assertEquals(startState.event.sender, self.machine)
        self.assertEquals(startState.event.value, 1)

    def testSimple(self):
        self.machine.injectEvent(self._makeEvent("Change"))
        cstate = self.machine.currentState()
        self.assertEquals(Simple, type(cstate))

    def testLoop(self):
        self.machine.injectEvent(self._makeEvent("LoopBack"))
        cstate = self.machine.currentState()

        # Ensure we got into are looping state
        self.assertEquals(LoopBack, type(cstate))
        self.assert_(cstate.entered)

        # Make  A Loopback
        self.machine.injectEvent(self._makeEvent("Update"))
        newstate = self.machine.currentState()

        self.assertEquals(LoopBack, type(newstate))
        self.assertFalse(newstate.exited)
        self.assertEquals(1, newstate.transCount)
        self.assertEquals(1, newstate.enterCount)
        self.assertEquals(newstate, cstate)

        # Repated loopbacks
        for i in xrange(1,5):
            self.machine.injectEvent(self._makeEvent("Update"))
        self.assertEquals(5, cstate.transCount)
        self.assertFalse(newstate.exited)
        self.assertEquals(1, newstate.enterCount)
        
    def testEvents(self):
        enterRecv = Reciever()
        exitRecv = Reciever()
        self.machine.subscribe(state.Machine.STATE_ENTERED, enterRecv)
        self.machine.subscribe(state.Machine.STATE_EXITED, exitRecv)

        startState = self.machine.currentState()
        self.machine.injectEvent(self._makeEvent("Start"))
        nextState = self.machine.currentState()

        # Check enter event
        self.assertEquals(state.Machine.STATE_ENTERED, enterRecv.event.type)
        self.assertEquals(self.machine, enterRecv.event.sender)
        self.assertEquals(nextState, enterRecv.event.state)

        # Check exit event
        self.assertEquals(state.Machine.STATE_EXITED, exitRecv.event.type)
        self.assertEquals(self.machine, exitRecv.event.sender)
        self.assertEquals(startState, exitRecv.event.state)
        
        
if __name__ == '__main__':
    unittest.main()
