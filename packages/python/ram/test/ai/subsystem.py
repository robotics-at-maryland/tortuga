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
    
    cfg = {'AIMachineName' : 'MyMachine',
             'taskOrder' : ['ram.test.ai.task.TaskA',
                            'ram.test.ai.task.TaskB',
                            'ram.test.ai.task.TaskC'],
             'failureTasks' : {
                 'ram.test.ai.task.TaskB' : 'ram.test.ai.task.BRecovery',
                 'ram.test.ai.task.TaskC' : 'ram.test.ai.task.CRecovery'}
             }
    
    def testGetNextTask(self):
        """
        Tests to make sure the AI properly builds the next task data based on 
        the configuration file.
        """
        
        # Create an AI subsystem with a proper order of tasks
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(stateMachine)
        aiSys = ai.subsystem.AI(TestAI.cfg, deps)
        
        # Check to make sure the order is proper
        self.assertEqual(TaskB, aiSys.getNextTask(TaskA))
        self.assertEqual(TaskC, aiSys.getNextTask(TaskB))
        self.assertEqual(ai.task.End, aiSys.getNextTask(TaskC))
        
    def testGetFailureState(self):
        """
        Tests to make sure the AI properly determines the state to go to on 
        failure based on the configuration file.
        """
        
        # Create an AI subsystem with a proper order of tasks
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(stateMachine)
        aiSys = ai.subsystem.AI(TestAI.cfg, deps)
        
        self.assertEqual(None, aiSys.getFailureState(TaskA))
        self.assertEqual(BRecovery, aiSys.getFailureState(TaskB))
        self.assertEqual(CRecovery, aiSys.getFailureState(TaskC))
