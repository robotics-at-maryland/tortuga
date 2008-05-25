# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/state.py


# STD Imports
import unittest
import StringIO

# Project Imports
import ext.core as core
import ram.ai.state as state

# --------------------------------------------------------------------------- #
#                      S U P P P O R T    O B J E C T S                       #
# --------------------------------------------------------------------------- #

class Reciever(object):
    def __init__(self):
        self.event = None
        self.called = False
    
    def __call__(self, event):
        self.event = event
        self.called = True

class MockEventSource(core.EventPublisher):
    THING_UPDATED = core.declareEventType('THING_UPDATED')
    ANOTHER_EVT = core.declareEventType('ANOTHER_EVT')
    
    def __init__(self, eventHub = None):
        core.EventPublisher.__init__(self, eventHub)
        
    def sendEvent(self, etype, **kwargs):
        event = core.Event()
        for key, value in kwargs.iteritems():
            setattr(event, key, value)
        self.publish(etype, event)

class TrackedState(state.State):
    """Records whether a state has been entered or exited"""
    def __init__(self, *args, **kwargs):
        state.State.__init__(self, *args, **kwargs)
        self.entered = False
        self.exited = False

    def enter(self):
        self.entered = True

    def exit(self):
        self.exited = True

# --------------------------------------------------------------------------- #
#                           T E S T   S T A T E S                             #
# --------------------------------------------------------------------------- #

# Test States (Consider Magic base class to take care of the init method)

class Start(TrackedState):
    def __init__(self, *args, **kwargs):
        TrackedState.__init__(self, *args, **kwargs)
        self.event = None
        self.func = None
        self.thingUpdatedEvent = None
        self.anotherEvtEvent = None
        
    @staticmethod
    def transitions():
        return { "Start" : End,
                 "Change" : Simple,
                 "LoopBack" : LoopBack ,
                 MockEventSource.THING_UPDATED : End,
                 MockEventSource.ANOTHER_EVT : QueueTestState}
 #                "Stuck" : state.PushState(Unstick) }

    def Start(self, event):
        self.event = event
        
    def THING_UPDATED(self, event):
        self.thingUpdatedEvent = event
        
    def ANOTHER_EVT(self, event):
        self.anotherEvtEvent = event

class QueueTestState(TrackedState):
    @staticmethod
    def transitions():
        return {MockEventSource.ANOTHER_EVT : End}
    
#class Unstick(TrackedState):
#    

class Simple(state.State):
    @staticmethod
    def transitions():
        return {MockEventSource.ANOTHER_EVT : Start}

class LoopBack(TrackedState):
    def __init__(self, *args, **kwargs):
        TrackedState.__init__(self, *args, **kwargs)
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

# --------------------------------------------------------------------------- #
#                                 T E S T S                                   #
# --------------------------------------------------------------------------- #

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
        self.assertNotEquals(None, startState.event)
        self.assertEquals(startState.event.sender, self.machine)
        self.assertEquals(startState.event.value, 1)

    def testStop(self):
        # No States
        machine = state.Machine()
        machine.stop()

        # Normal
        startState = self.machine.currentState()

        # Stop the machine and make sure events have no effect
        self.machine.stop()

        self.assertRaises(Exception, self.machine.injectEvent,
                          self._makeEvent("Start", value = 1))
        cstate = self.machine.currentState()

        self.assertNotEquals(End, type(cstate))
        self.assertNotEquals(startState, cstate)

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
        
    def testDeclaredEvents(self):
        startState = self.machine.currentState()

        self.machine.injectEvent(self._makeEvent(MockEventSource.THING_UPDATED,
                                                 value = 4))
        cstate = self.machine.currentState()

        # Check to me sure we left the start state
        self.assert_(startState.exited)

        # Check to make sure we reached the proper state
        self.assertEquals(End, type(cstate))
        self.assert_(cstate)

        # Make sure the transition function was called
        self.assertNotEquals(None, startState.thingUpdatedEvent)
        self.assertEquals(startState.thingUpdatedEvent.sender, self.machine)
        self.assertEquals(startState.thingUpdatedEvent.value, 4)
        
    def testEventHub(self):
        eventHub = core.EventHub()
        qeventHub = core.QueuedEventHub(eventHub)
        mockEventSource = MockEventSource(eventHub)
        
        machine = state.Machine(deps = [eventHub, qeventHub])
        machine.start(Start)
        
        # Send an event, and make sure it does get through until the queue
        # releases it
        mockEventSource.sendEvent(MockEventSource.ANOTHER_EVT, value = 20)
        startState = machine.currentState()
        self.assertEquals(Start, type(startState))
        
        # Release events and make sure we have transition properly
        qeventHub.publishEvents()
        self.assertEquals(QueueTestState, type(machine.currentState()))
        self.assertEquals(20, startState.anotherEvtEvent.value)
        
        # Fire off another event and make sure we haven't gone anywhere
        mockEventSource.sendEvent(MockEventSource.THING_UPDATED, value = 34)
        qeventHub.publishEvents()
        self.assertEquals(QueueTestState, type(machine.currentState()))
        
    def testConfig(self):
        cfg = { 
            'param' : 5,
            'States' : {
                'state.StateTestConfig' : {
                    'val' : 10,
                    'other' : 'job'
                }
            }
        }
        machine = state.Machine(cfg = cfg)
        
        machine.start(StateTestConfig)
        current = machine.currentState()
        self.assertEqual(10, current.getConfig('val'))
        self.assertEqual('job', current.getConfig('other'))
        
    def testSubsystemPassing(self):
        eventHub = core.EventHub("EventHub")
        qeventHub = core.QueuedEventHub(eventHub, "QueuedEventHub")
        machine = state.Machine(deps = [eventHub, qeventHub])
        
        machine.start(Start)
        startState = machine.currentState()
        
        # Check for subsystems
        self.assertEquals(eventHub, startState.eventHub)
        self.assertEquals(qeventHub, startState.queuedEventHub)
    
    def testWriteGraph(self):
        mockFile = StringIO.StringIO()
        state = Start
        self.machine.writeStateGraph(mockFile,state, ordered = True)
        output = mockFile.getvalue()
        expected = "digraph aistate {\n" + \
            "LoopBack->LoopBack[label=Update]\n" + \
            "QueueTestState->End[label=ANOTHER_EVT]\n" + \
            "Simple->Start[label=ANOTHER_EVT]\n" + \
            "Start->End[label=Start]\n" + \
            "Start->End[label=THING_UPDATED]\n" + \
            "Start->LoopBack[label=LoopBack]\n" + \
            "Start->QueueTestState[label=ANOTHER_EVT]\n" + \
            "Start->Simple[label=Change]\n" + \
            "}"
        self.assertEquals(expected,output)
        
        
# Testing of State Class
class StateTestConfig(state.State):
    def getConfig(self, val):
        return self._config[val]

class TestState(unittest.TestCase):
    def testSubsystemArgs(self):
        s = state.State(a = 5, bob = 'A')
        self.assertEqual(5, s.a)
        self.assertEqual('A', s.bob)
        
        # Make sure config isn't considered a subsystem
        s = state.State(config = 5)
        self.assertFalse(hasattr(s, 'config'))
        
    def testConfig(self):
        s = StateTestConfig({'ram' : 50}, john = 10)
        self.assertEqual(10, s.john)
        self.assertFalse(hasattr(s, 'config'))
        
        self.assertEqual(50, s.getConfig('ram'))
        
    def testPublish(self):
        self.called = False
        self.sender = None
        self.type = None
        
        def reciever(event):
            self.called = True
            self.type = event.type
            self.sender = event.sender
        
        machine = state.Machine()
        machine.start(Start)
        
        machine.subscribe("TestEvent", reciever)
        machine.currentState().publish("TestEvent", core.Event())
        
        
        self.assert_(self.called)
        self.assertEqual("TestEvent", self.type)
        self.assertEqual(machine, self.sender)
        
        
if __name__ == '__main__':
    unittest.main()
