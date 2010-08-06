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
import ram.ai.subsystem as aisys
import ram.motion as motion
import ram.logloader as logloader

import ram.test.ai.support as aisupport

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
                 MockEventSource.THING_UPDATED : Simple,
                 MockEventSource.ANOTHER_EVT : QueueTestState,
                 "Branch" : state.Branch(BranchedState) }

    def Change(self, event):
        self.event = event
        
    def THING_UPDATED(self, event):
        self.thingUpdatedEvent = event
        
    def ANOTHER_EVT(self, event):
        self.anotherEvtEvent = event

class QueueTestState(TrackedState):
    @staticmethod
    def transitions():
        return {MockEventSource.ANOTHER_EVT : Simple}

class Simple(TrackedState):
    @staticmethod
    def transitions():
        return {MockEventSource.ANOTHER_EVT : Start}

class LoopBack(TrackedState):
    @staticmethod
    def transitions():
        return { "Update" : LoopBack }
    
    def __init__(self, *args, **kwargs):
        TrackedState.__init__(self, *args, **kwargs)
        self.transCount = 0
        self.enterCount = 0

    def enter(self):
        TrackedState.enter(self)
        self.enterCount += 1

    def Update(self, event):
        self.transCount += 1

class Veto(TrackedState):
    @staticmethod
    def transitions():
        return { MockEventSource.ANOTHER_EVT : Start }

    def ANOTHER_EVT(self, event):
        return False

class End(TrackedState):
    pass

class BranchedState(TrackedState):
    @staticmethod
    def transitions():
        return { "InBranchEvent" : BranchedMiddle }
    
class BranchedMiddle(TrackedState):
    @staticmethod
    def transitions():
        return { "InBranchEndEvent" : BranchedEnd }
    
class BranchedEnd(state.End):
    def __init__(self):
        state.End.__init__(self, *args, **kwargs)
        self.entered = False
        self.exited = False
        
    def enter(self):
        self.entered = True

    def exit(self):
        self.exited = True

# States which attempt to find the loopback error
class First(state.State):
    @staticmethod
    def transitions():
        return { "GO" : Second }
    
class Second(state.State):
    @staticmethod
    def transitions():
        return { "GO" : Simple }
    
class FirstParent(state.State):
    @staticmethod
    def transitions():
        return { "GO" : SecondParent }
        
class SecondParent(state.State):
    @staticmethod
    def transitions():
        return { "BOB" : End }
    
    def enter(self):
        self.stateMachine.start(state.Branch(First))
    
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

        self.machine.injectEvent(self._makeEvent("Change", value = 1))
        cstate = self.machine.currentState()

        # Check to me sure we left the start state
        self.assert_(startState.exited)

        # Check to make sure we reached the proper state
        self.assertEquals(Simple, type(cstate))
        self.assert_(cstate)

        # Make sure the transition function was called
        self.assertNotEquals(None, startState.event)
        # TODO: Why are you failing!!!
        #self.assertEquals(startState.event.sender, self.machine)
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
       
    def testComplete(self):
        enterRecv = Reciever()
        exitRecv = Reciever()
        self.machine.subscribe(state.Machine.STATE_ENTERED, enterRecv)
        self.machine.subscribe(state.Machine.STATE_EXITED, exitRecv)
        
        # Ensure that completion is detected
        self.assertEqual(False, self.machine.complete)
        self.machine.injectEvent(self._makeEvent("Start"))
        self.assert_(self.machine.complete)
        
        # State machine is done, make sure there is no current state
        self.assertEqual(None, self.machine.currentState())
        
        # Ensure we entered and exited the exit state
        endName = '%s.%s' % (End.__module__, End.__name__)
        self.assertEquals(endName, enterRecv.event.string)
        self.assertEquals(endName, exitRecv.event.string)

    def testEvents(self):
        enterRecv = Reciever()
        exitRecv = Reciever()
        completeRecv = Reciever()
        self.machine.subscribe(state.Machine.STATE_ENTERED, enterRecv)
        self.machine.subscribe(state.Machine.STATE_EXITED, exitRecv)
        self.machine.subscribe(state.Machine.COMPLETE, completeRecv)

        startState = self.machine.currentState()
        self.machine.injectEvent(self._makeEvent("Change"))
        nextState = self.machine.currentState()

        self.assertNotEqual(startState, nextState)
        
        # Check enter event
        nextStateName = '%s.%s' % (nextState.__class__.__module__,
                                   nextState.__class__.__name__)
        eventStr = enterRecv.event.string
        self.assertEquals(state.Machine.STATE_ENTERED, enterRecv.event.type)
        self.assertEquals(self.machine, enterRecv.event.sender)
        self.assertEquals(nextStateName, eventStr)
        
        # Ensure the state resolves to the proper state
        self.assertEqual(nextState.__class__, logloader.resolve(eventStr))

        # Check exit event
        startStateName = '%s.%s' % (startState.__class__.__module__,
                                   startState.__class__.__name__)
        eventStr = exitRecv.event.string
        self.assertEquals(state.Machine.STATE_EXITED, exitRecv.event.type)
        self.assertEquals(self.machine, exitRecv.event.sender)
        self.assertEquals(startStateName, exitRecv.event.string)
        
        # Ensure the state resolves to the proper state
        self.assertEqual(startState.__class__, logloader.resolve(eventStr))
        
        # Check completion event
        self.machine.injectEvent(self._makeEvent(MockEventSource.ANOTHER_EVT))
        self.machine.injectEvent(self._makeEvent("Start"))
        
        self.assertEquals(state.Machine.COMPLETE, completeRecv.event.type)
        self.assertEquals(self.machine, completeRecv.event.sender)
        
    def testDeclaredEvents(self):
        startState = self.machine.currentState()

        self.machine.injectEvent(self._makeEvent(MockEventSource.THING_UPDATED,
                                                 value = 4))
        cstate = self.machine.currentState()

        # Check to me sure we left the start state
        self.assert_(startState.exited)

        # Check to make sure we reached the proper state
        self.assertEquals(Simple, type(cstate))
        self.assert_(cstate)

        # Make sure the transition function was called
        self.assertNotEquals(None, startState.thingUpdatedEvent)
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
        self.assert_(startState.anotherEvtEvent)
        self.assertEquals(20, startState.anotherEvtEvent.value)
        
        # Fire off another event and make sure we haven't gone anywhere
        mockEventSource.sendEvent(MockEventSource.THING_UPDATED, value = 34)
        qeventHub.publishEvents()
        self.assertEquals(QueueTestState, type(machine.currentState()))
        
    def testConfig(self):
        cfg = { 
            'param' : 5,
            'States' : {
                'ram.test.ai.state.StateTestConfig' : {
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
            "ram_test_ai_state_BranchedEnd " + \
            "[label=BranchedEnd,shape=doubleoctagon]\n" + \
            "ram_test_ai_state_BranchedMiddle " + \
            "[label=BranchedMiddle,shape=ellipse]\n" + \
            "ram_test_ai_state_BranchedState " + \
            "[label=BranchedState,shape=ellipse]\n" + \
            "ram_test_ai_state_End [label=End,shape=doubleoctagon]\n" + \
            "ram_test_ai_state_LoopBack [label=LoopBack,shape=ellipse]\n" + \
            "ram_test_ai_state_QueueTestState " + \
            "[label=QueueTestState,shape=ellipse]\n" + \
            "ram_test_ai_state_Simple [label=Simple,shape=ellipse]\n" + \
            "ram_test_ai_state_Start [label=Start,shape=ellipse]\n" + \
            "ram_test_ai_state_BranchedMiddle -> " + \
            "ram_test_ai_state_BranchedEnd " + \
            "[label=InBranchEndEvent,style=solid]\n" + \
            "ram_test_ai_state_BranchedState -> " + \
            "ram_test_ai_state_BranchedMiddle " + \
            "[label=InBranchEvent,style=solid]\n" + \
            "ram_test_ai_state_LoopBack -> " + \
            "ram_test_ai_state_LoopBack [label=Update,style=solid]\n" + \
            "ram_test_ai_state_QueueTestState -> " + \
            "ram_test_ai_state_Simple [label=ANOTHER_EVT,style=solid]\n" + \
            "ram_test_ai_state_Simple -> ram_test_ai_state_Start " + \
            "[label=ANOTHER_EVT,style=solid]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_BranchedState " + \
            "[label=Branch,style=dotted]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_End " + \
            "[label=Start,style=solid]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_LoopBack " + \
            "[label=LoopBack,style=solid]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_QueueTestState " + \
            "[label=ANOTHER_EVT,style=solid]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_Simple " + \
            "[label=Change,style=solid]\n" + \
            "ram_test_ai_state_Start -> ram_test_ai_state_Simple " + \
            "[label=THING_UPDATED,style=solid]\n" + \
            "}"

        self.assertEquals(expected,output)
    
    def testBasicBranching(self):
        # Test Branching
        self.machine.injectEvent(self._makeEvent("Branch"), 
                                 _sendToBranches = True)
        
        # Make sure we are still in the start state
        self.assertEqual(Start, type(self.machine.currentState()))

        # Ensure the branch was created properly
        self.assertEqual(1, len(self.machine.branches))
        
        # Make sure it was entered properly
        branchedMachine = self.machine.branches[BranchedState]
        branchStartState = branchedMachine.currentState()
        self.assertEqual(BranchedState, type(branchStartState))
        self.assert_(branchStartState.entered)
        self.assertEqual(False, branchStartState.exited)
        
        # Make sure branched machine doesn't impair state changes events
        self.machine.injectEvent(self._makeEvent("Change"),
                                 _sendToBranches = True)
        self.assertEqual(Simple, type(self.machine.currentState()))
        
        # Make sure events reach the branched machine
        self.machine.injectEvent(self._makeEvent("InBranchEvent"),
                                 _sendToBranches = True)
        self.assert_(branchStartState.exited)
        self.assertEqual(BranchedMiddle, type(branchedMachine.currentState()))
        
        # Make sure we are still in the proper main state machine state
        self.assertEqual(Simple, type(self.machine.currentState()))
        
    def testRepeatBranching(self):
        self.machine.injectEvent(self._makeEvent("Branch"), 
                                 _sendToBranches = True)
        
        # Make sure branching again throws an error
        self.assertRaises(Exception, self.machine.injectEvent, 
                          self._makeEvent("Branch"), _sendToBranches = True)
        
    def testBranchState(self):
        self.machine.start(state.Branch(Simple))
        
        # Make sure current state stayed
        self.assertEqual(Start, type(self.machine.currentState()))
        
        # Ensure we actually branched
        self.assertEqual(1, len(self.machine.branches))
        self.assert_(self.machine.branches.has_key(Simple))
   
    def testBranchStop(self):
        self.machine.start(state.Branch(Simple))
        branchedState = self.machine.branches[Simple].currentState()
        
        self.machine.stop()
        
        # Ensure we actually stopped the branch
        self.assertEqual(0, len(self.machine.branches))
        self.assert_(branchedState.exited)
        
        # Now test stopping just one branch
        self.machine.start(state.Branch(Simple))
        self.machine.start(state.Branch(Start))
        self.assertEqual(2, len(self.machine.branches))
        
        self.machine.stopBranch(Simple)
        self.assertEqual(1, len(self.machine.branches))
        self.assertFalse(self.machine.branches.has_key(Simple))
        
        self.machine.stopBranch(Start)
        self.assertEqual(0, len(self.machine.branches))
        self.assertFalse(self.machine.branches.has_key(Start))
        
    def testDoubleTransitions(self):
        self.machine.start(First)
        self.assertEqual(First, type(self.machine.currentState()))
        
        self.machine.injectEvent(self._makeEvent("GO", value = 1))
        self.assertEqual(Second, type(self.machine.currentState()))
        
    def testDoubleBranchTransitions(self):
        # Start us up
        self.machine.start(FirstParent)
        self.assertEqual(FirstParent, type(self.machine.currentState()))
        
        self.machine.injectEvent(self._makeEvent("GO", value = 1))
        self.assertEqual(SecondParent, type(self.machine.currentState()))
        
        # Make sure I branched properly
        self.assert_(self.machine.branches.has_key(First))
        branch = self.machine.branches[First]
        
        cstate = branch.currentState()
        self.assertEqual(First, type(cstate))
        
    def testAiSet(self):
        ai = aisys.AI()
        machine = state.Machine(deps = [ai])
        self.assertEquals(machine, ai.mainStateMachine)

    def testVetoState(self):
        self.machine.start(Veto)
        self.assertEqual(Veto, type(self.machine.currentState()))

        self.machine.injectEvent(self._makeEvent(MockEventSource.ANOTHER_EVT))
        self.assertEqual(Veto, type(self.machine.currentState()))
        
# Testing of State Class
class StateTestConfig(state.State):
    @staticmethod
    def transitions():
        return {'TEST' : StateTestConfig }
    @staticmethod
    def getattr():
        return set(['val', 'other'])
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
        
class TestFindAttempt(aisupport.AITestCase):
    OBJECT_FOUND = core.declareEventType('OBJECT_FOUND')

    class OriginalState(state.State):
        @staticmethod
        def transitions():
            return {'DUMMY' : 'TRANSITION'}

    class TimeoutState(state.State):
        @staticmethod
        def transitions():
            return {'DUMMY' : 'TRANSITION'}

    class FindAttemptState(state.FindAttempt):
        @staticmethod
        def transitions():
            return state.FindAttempt.transitions(TestFindAttempt.OBJECT_FOUND,
                                                 TestFindAttempt.OriginalState,
                                                 TestFindAttempt.TimeoutState)

    def injectEventFound(self):
        self.injectEvent(TestFindAttempt.OBJECT_FOUND)
    def injectEventTimeout(self):
        # TODO: Real timer test
        self.releaseTimer(state.FindAttempt.TIMEOUT)

    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(TestFindAttempt.FindAttemptState)

    def testFound(self):
        self.assertCurrentState(TestFindAttempt.FindAttemptState)
        self.injectEventFound()
        self.assertCurrentState(TestFindAttempt.OriginalState)

    def testTimeout(self):
        self.assertCurrentState(TestFindAttempt.FindAttemptState)
        self.injectEventTimeout()
        self.assertCurrentState(TestFindAttempt.TimeoutState)
                
if __name__ == '__main__':
    unittest.main()
