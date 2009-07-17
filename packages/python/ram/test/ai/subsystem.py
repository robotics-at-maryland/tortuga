# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/subsystem.py

# Python Imports
import unittest

# Project Imports
import ext.core as core
import ram.ai as ai
import ram.ai.subsystem

from ram.test import Mock
from ram.test.ai.task import TaskA, TaskB, TaskC, BRecovery, CRecovery
from ram.test.ai.support import AITestCase

class TestAI(unittest.TestCase):
    def testCreate(self):
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(core.Subsystem('A',core.SubsystemList()))
        deps.append(core.Subsystem('B',core.SubsystemList()))
        deps.append(ai.state.Machine())
        deps.append(core.Subsystem('C', core.SubsystemList()))
        deps.append(stateMachine)
        
        aiSys = ai.subsystem.AI({'AIMachineName' : 'MyMachine'}, deps)
        
        # Make sure it finds the AI with a custom name
        self.assertEqual(stateMachine, aiSys.mainStateMachine)

        # Make sure we get an error when its not found
        # TODO: resolve circular dep
        #self.assertRaises(AssertionError, ai.subsystem.AI, {}, 
        #                  core.SubsystemList())
        
    def testData(self):
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(stateMachine)
        aiSys = ai.subsystem.AI({'AIMachineName' : 'MyMachine'}, deps)
        
        aiSys.data['Bob'] = 10
        self.assertEqual(10, aiSys.data['Bob'])
        self.assertEqual({}, aiSys.data['config'])

    def testDiconnect(self):
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(stateMachine)
        aiSys = ai.subsystem.AI({'AIMachineName' : 'MyMachine'}, deps)
        
        # Stub connections
        def conFuncA():
            self.connA = True
        self.connA = False
        mockConnA = Mock(disconnect=conFuncA)
        aiSys.addConnection(mockConnA)
        
        def conFuncB():
            self.connB = True
        self.connB = False
        mockConnB = Mock(disconnect=conFuncB)
        aiSys.addConnection(mockConnB)
        
        # Make sure we disconnect them
        aiSys.unbackground()
        self.assert_(self.connA)
        self.assert_(self.connB)
    
    
class TestAITasks(AITestCase):
    def setUp(self):
        cfg = {'Ai' : {
             'taskOrder' : ['ram.test.ai.task.TaskA',
                            'ram.test.ai.task.TaskB',
                            'ram.test.ai.task.TaskC'],
             'failureTasks' : {
                 'ram.test.ai.task.TaskB' : 'ram.test.ai.task.BRecovery',
                 'ram.test.ai.task.TaskC' : 'ram.test.ai.task.CRecovery'}
             }
        }
        AITestCase.setUp(self, cfg = cfg)
    
    def testGetNextTask(self):
        """
        Tests to make sure the AI properly builds the next task data based on 
        the configuration file.
        """
        
        # Check to make sure the order is proper
        self.assertEqual(TaskB, self.ai.getNextTask(TaskA))
        self.assertEqual(TaskC, self.ai.getNextTask(TaskB))
        self.assertEqual(ai.task.End, self.ai.getNextTask(TaskC))
        
    def testGetFailureState(self):
        """
        Tests to make sure the AI properly determines the state to go to on 
        failure based on the configuration file.
        """
                
        self.assertEqual(None, self.ai.getFailureState(TaskA))
        self.assertEqual(BRecovery, self.ai.getFailureState(TaskB))
        self.assertEqual(CRecovery, self.ai.getFailureState(TaskC))
        
    def testStartStop(self):
        
        # Start up the AI and make sure we in the proper state
        self.ai.start()
        self.assertEqual(TaskA, type(self.machine.currentState()))
        
        # Now stop it and make sure we are no longer in a state
        self.ai.stop()
        self.assertEqual(None, self.machine.currentState())
