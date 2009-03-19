# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/task.py

# Python Imports
import unittest

# Project Imports
import ext.core as core
import ram.ai as ai
import ram.ai.subsystem
import ram.ai.state
import ram.ai.task as task
from ram.logloader import resolve

import ram.test.ai.support as support

EVENT_A = core.declareEventType('A')
EVENT_B = core.declareEventType('B')
EVENT_C = core.declareEventType('C')
EVENT_D = core.declareEventType('D')
EVENT_E = core.declareEventType('E')
EVENT_F = core.declareEventType('F')

class TaskA(task.Task):
    DEFAULT_TIMEOUT = 16
    def _transitions(self):
        return {EVENT_A : task.Next,
                EVENT_B : task.Next,
                task.TIMEOUT : task.Next }
    def enter(self):
        task.Task.enter(self, TaskA.DEFAULT_TIMEOUT)

class TaskB(task.Task):
    def _transitions(self):
        return {EVENT_C : task.Next,
                EVENT_D : task.Next,
                task.TIMEOUT : task.Next }

class TaskC(task.Task):
    def _transitions(self):
        return {EVENT_E : task.Next,
                EVENT_F : task.Next }


class TestTask(support.AITestCase):
    CFG_TIMEOUT = 47
    
    def setUp(self):
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.test.ai.task.TaskA',
                               'ram.test.ai.task.TaskB',
                               'ram.test.ai.task.TaskC']
            },
            'StateMachine' : { 
                'States' : { 
                    'ram.test.ai.task.TaskB' : { 
                        'timeout' : TestTask.CFG_TIMEOUT
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)

        # We have to import these from the global level, because for some 
        # reason TaskA here != ram.test.ai.task.TaskA
        self.TaskAcls = resolve('ram.test.ai.task.TaskA')
        self.TaskBcls = resolve('ram.test.ai.task.TaskB')
        self.TaskCcls = resolve('ram.test.ai.task.TaskC')
        
    def testNextTransitions(self):
        """
        Make sure the marking "task.Next" states get replaced with the real 
        next states.
        """
        
        taskA = self.TaskAcls(ai = self.ai)
        self.assertEqual(self.TaskBcls, taskA.transitions()[EVENT_A])
        self.assertEqual(self.TaskBcls, taskA.transitions()[EVENT_B])
        taskB = self.TaskBcls(ai = self.ai)
        self.assertEqual(self.TaskCcls, taskB.transitions()[EVENT_C])
        self.assertEqual(self.TaskCcls, taskB.transitions()[EVENT_D])
        taskC = self.TaskCcls(ai = self.ai)
        self.assertEqual(task.End, taskC.transitions()[EVENT_E])
        self.assertEqual(task.End, taskC.transitions()[EVENT_F])
        
    def _injectEvent(self, etype):
        event = core.Event()
        event.type = etype
        self.machine.injectEvent(event)
        
    def testTransition(self):
        """
        Now make sure the whole thing works in a real statemachine
        """
        # Start up in the TaskA state
        self.machine.start(self.TaskAcls)
        
        # Now inject events to move us from the start to the end
        self._injectEvent(EVENT_A)
        cstate = self.machine.currentState()
        self.assertEquals(self.TaskBcls, type(cstate))
        
        self._injectEvent(EVENT_D)
        cstate = self.machine.currentState()
        self.assertEquals(self.TaskCcls, type(cstate))
        
        self._injectEvent(EVENT_E)
        self.assert_(self.machine.complete)
        
    def testDefaultTimeout(self):
        """
        Tests normal timeout procedure
        """
        self._timeoutTest(self.TaskAcls, self.TaskBcls, TaskA.DEFAULT_TIMEOUT)
        
    def testCfgTimeout(self):
        """
        Tests to make sure the timeout value was read from the cfg properly
        """
        self._timeoutTest(self.TaskBcls, self.TaskCcls, TestTask.CFG_TIMEOUT)
    
    def testStopTimer(self):
        """
        Tests to make sure the time doesn't fire when the state exits
        """
        # Register for the timer
        self._timerFired = False
        def timerHandler(event):
            self._timerFired = True
        self.eventHub.subscribeToType(self.TaskAcls(ai = self.ai).timeoutEvent,
                                      timerHandler)
        
        # Start up and make sure we are in the proper state
        self.machine.start(self.TaskAcls)
        startState = self.machine.currentState()
        self.assertEquals(self.TaskAcls, type(startState))
        
        # Move to the next state
        self._injectEvent(EVENT_A)
        cstate = self.machine.currentState()
        self.assertEquals(self.TaskBcls, type(cstate))
        
        # Release the timer and make sure it *wasn't* called
        self.releaseTimer(startState.timeoutEvent)
        self.assertEqual(False, self._timerFired)
        
    
    def _timeoutTest(self, startState, expectedEndState, expectedTimeout):
        """
        Helper function for testing whether or not the timeout timer works
        """
        # Start up and make sure we are in the proper state
        self.machine.start(startState)
        cstate = self.machine.currentState()
        self.assertEquals(startState, type(cstate))
        
        # Ensure that the time was read correctly
        self.assertEqual(expectedTimeout, cstate.timeoutDuration)
        
        # Release timer and make sure we are in the right state
        self.releaseTimer(cstate.timeoutEvent)
        cstate = self.machine.currentState()
        self.assertEquals(expectedEndState, type(cstate))
