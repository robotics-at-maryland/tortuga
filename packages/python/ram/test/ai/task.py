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

EVENT_A = core.declareEventType('A')
EVENT_B = core.declareEventType('B')
EVENT_C = core.declareEventType('C')
EVENT_D = core.declareEventType('D')
EVENT_E = core.declareEventType('E')
EVENT_F = core.declareEventType('F')

class TaskA(task.Task):
    def _transitions(self):
        return {EVENT_A : task.Next,
                EVENT_B : task.Next }

class TaskB(task.Task):
    def _transitions(self):
        return {EVENT_C : task.Next,
                EVENT_D : task.Next }

class TaskC(task.Task):
    def _transitions(self):
        return {EVENT_E : task.Next,
                EVENT_F : task.Next }


class TestTask(unittest.TestCase):
    def setUp(self):
        # Create an AI subsystem with a proper order of tasks
        self.aiSys = ai.subsystem.AI(
            {'AIMachineName' : 'MyMachine',
             'taskOrder' : ['ram.test.ai.task.TaskA',
                            'ram.test.ai.task.TaskB',
                            'ram.test.ai.task.TaskC']})
        
        deps = core.SubsystemList()
        deps.append(self.aiSys)
        self.stateMachine = ai.state.Machine({'name' : 'MyMachine'}, deps)

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
        
        taskA = self.TaskAcls(ai = self.aiSys)
        self.assertEqual(self.TaskBcls, taskA.transitions()[EVENT_A])
        self.assertEqual(self.TaskBcls, taskA.transitions()[EVENT_B])
        taskB = self.TaskBcls(ai = self.aiSys)
        self.assertEqual(self.TaskCcls, taskB.transitions()[EVENT_C])
        self.assertEqual(self.TaskCcls, taskB.transitions()[EVENT_D])
        taskC = self.TaskCcls(ai = self.aiSys)
        self.assertEqual(task.End, taskC.transitions()[EVENT_E])
        self.assertEqual(task.End, taskC.transitions()[EVENT_F])
        
    def _injectEvent(self, etype):
        event = core.Event()
        event.type = etype
        self.stateMachine.injectEvent(event)
        
    def testTransition(self):
        """
        Now make sure the whole thing works in a real statemachine
        """
        # Start up in the TaskA state
        self.stateMachine.start(self.TaskAcls)
        
        # Now inject events to move us from the start to the end
        self._injectEvent(EVENT_A)
        cstate = self.stateMachine.currentState()
        self.assertEquals(self.TaskBcls, type(cstate))
        
        self._injectEvent(EVENT_D)
        cstate = self.stateMachine.currentState()
        self.assertEquals(self.TaskCcls, type(cstate))
        
        self._injectEvent(EVENT_E)
        self.assert_(self.stateMachine.complete)
