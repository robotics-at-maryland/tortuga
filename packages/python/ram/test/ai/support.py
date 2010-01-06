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
import types

# Project Imports
import ext.core as core
import ram.ai.state as state
import ram.timer as timer
import ram.motion as motion
import ram.motion.basic

from ram.ai.subsystem import AI
from ram.test.timer import TimerTester
from ram.test.motion.support import MockController, MockVehicle, MockTimer


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
        
class MockVisionSystem(core.Subsystem):
    def __init__(self):
        core.Subsystem.__init__(self, 'VisionSystem')
        self.redLightDetector = False
        self.pipeLineDetector = False
        self.binDetector = False
        self.ductDetector = False
        self.downwardSafeDetector = False
        self.targetDetector = False
        self.barbedWireDetector = False
         
    def redLightDetectorOn(self):
        self.redLightDetector = True
        
    def redLightDetectorOff(self):
        self.redLightDetector = False
        
    def pipeLineDetectorOn(self):
        self.pipeLineDetector = True
        
    def pipeLineDetectorOff(self):
        self.pipeLineDetector = False
        
    def binDetectorOn(self):
        self.binDetector = True
        
    def binDetectorOff(self):
        self.binDetector = False
        
    def downwardSafeDetectorOn(self):
        self.downwardSafeDetector = True
        
    def downwardSafeDetectorOff(self):
        self.downwardSafeDetector = False
        
    def ductDetectorOn(self):
        self.ductDetector = True
        
    def ductDetectorOff(self):
        self.ductDetector = False
        
    def targetDetectorOn(self):
        self.targetDetector = True
        
    def targetDetectorOff(self):
        self.targetDetector = False

    def barbedWireDetectorOn(self):
        self.barbedWireDetector = True
        
    def barbedWireDetectorOff(self):
        self.barbedWireDetector = False

        
class AITestCase(unittest.TestCase):
    TIMER_ORIG = timer.Timer
    __timerMocked = False

    def mockSleep(self, seconds):
        self.seconds = seconds
    
    def mockTimer(self):
        if AITestCase.__timerMocked:
            assert timer.Timer == MockTimer
            # Put the original timer class back
            timer.Timer = AITestCase.TIMER_ORIG

            AITestCase.__timerMocked = False
        else:
            assert timer.Timer != MockTimer
            # Replace Timer with out Mock Timer Class
            timer.Timer = MockTimer

            AITestCase.__timerMocked = True
    
    def setUp(self, extraDeps = None, cfg = None):
        # Handle case where the timer is still mocked for some reason, we make
        # sure to un mock it
        if AITestCase.__timerMocked:
            self.mockTimer()

        # Replace the Timer class with the Mock one
        self.mockTimer()
        
        if extraDeps is None:
            extraDeps = []
            
        if cfg is None:
            cfg = {}
                
        self.eventHub = core.EventHub()
        self.qeventHub = core.QueuedEventHub(self.eventHub)
        self.timerManager = timer.TimerManager(deps = [self.eventHub])
        self.controller = MockController(self.eventHub)
        self.vehicle = MockVehicle(cfg = cfg.get('Vehicle', {}))
        self.visionSystem = MockVisionSystem()
        
        aCfg = cfg.get('Ai', {})
        self.ai = AI(aCfg)
        
        deps = [self.controller, self.timerManager, self.eventHub, 
                self.qeventHub, self.vehicle, self.visionSystem, self.ai]
        
        mCfg = cfg.get('MotionManager', {})
        self.motionManager = motion.basic.MotionManager(mCfg, deps)
        deps.append(self.motionManager)
        
        deps.extend(extraDeps)
        sCfg = cfg.get('StateMachine', {})
        self.machine = state.Machine(cfg = sCfg, deps = deps)
    
    def tearDown(self):
        # Put back the normal timer class
        self.ai.unbackground()
        self.mockTimer()
        self.machine.stop()

    def _createEvent(self, etype, eclass = core.Event, *args, **kwargs):
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
            
        return event

    def injectEvent(self, etype, eclass = core.Event, *args, **kwargs):
        event = self._createEvent(etype, eclass, *args, **kwargs)
        
        sendToBranches = False
        if kwargs.has_key('sendToBranches'):
            sendToBranches = True
            del kwargs['sendToBranches']
        
        self.machine.injectEvent(event, _sendToBranches = sendToBranches)
     
    def publishQueuedEvent(self, publisher, etype, eclass = core.Event, *args, 
                           **kwargs):
        event = self._createEvent(etype, eclass, *args, **kwargs)
        self.qeventHub.publish(etype, event)
        self.qeventHub.publishEvents()
     
    def releaseTimer(self, eventType):
        timer = MockTimer.LOG[eventType]

        # Make sure the timer has actual been started
        self.assert_(timer.started)
        
        # Release the timer finished event
        timer.finish()
        
        self.qeventHub.publishEvents()
        
#    def releaseTimer(self, timer):
#        """
#        Stops a spinning background timer, joins it, then releases its event
#        """
#        assert self.timerBlock, "Must set self.timerBlock to be true"
#        
#        # Stops the background loop
#        self.timerBlock = False
#        timer.join()
#        # Releases the event the timer published when the loop stopped
#        self.qeventHub.publishEvents()
        
    def assertCurrentState(self, stateType):
        """
        Compares the type of the current state machine state to the given one
        """
        self.assertEquals(stateType, type(self.machine.currentState()))
        
    def assertCurrentBranchState(self, branchState, stateType):
        """
        Compares the type of the current state machine state to the given one
        """
        self.assert_(self.machine.branches.has_key(branchState))
        branch = self.machine.branches[branchState]
        self.assertEquals(stateType, type(branch.currentState()))
        
    def assertCurrentMotion(self, motionType):
        """
        Compares the type of the current motion to the given one
        """
        current = self.motionManager.currentMotion
        currentType = type(self.motionManager.currentMotion)
        
        def getType(val):
            if val is None:
                return None
            else:
                return type(val)
            
        if types.TupleType == currentType:
            currentType = (getType(current[0]), getType(current[1]), 
                           getType(current[2]))
            
        self.assertEquals(motionType, currentType)

    def assertCurrentMotionList(self, motionList):
        """
        Compares the type of the current motion queue to the given one.
        It includes the current motion.
        """
        # Get the current motion list
        currentList = self.motionManager.currentMotionList

        # Check if None
        if motionList is None:
            return currentList is None

        # The motionList should be a list if it isn't None
        if types.ListType != type(motionList):
            raise Exception('A list must be sent into assertCurrentMotionList')

        # Check the current motion first
        current = motionList[0]
        self.assertCurrentMotion(current)

        # Parse out the current motion from the motion list
        currentList = currentList[1:]
        motionList = motionList[1:]

        # Make sure there are the same number of motions in both lists
        if len(currentList) != len(motionList):
            return False

        # Iterate through the list and check the types
        for c, m in zip(currentList, motionList):
            if type(c) != m:
                return False

        # The motion list is the same if it's reached this point in the code
        return True

    def assertCurrentBranches(self, branches):
        for branch in branches:
            self.assert_(self.machine.branches.has_key(branch))
        self.assertEqual(len(branches), len(self.machine.branches))
        
    def assertLessThan(self, a, b):
        if not (a < b):
            self.fail(str(a) + " is not less than " + str(b))
            
    def assertGreaterThan(self, a, b):
        if not (a > b):
            self.fail(str(a) + " is not greater than " + str(b))
            
    def assertAIDataValue(self, key, value):
        self.assert_(self.ai.data.has_key(key))
        self.assertEqual(value, self.ai.data[key])
        
    def assertDataValue(self, data, key, value):
        self.assert_(data.has_key(key))
        self.assertEqual(value, data[key])

