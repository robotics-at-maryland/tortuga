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
        self.assertRaises(AssertionError, ai.subsystem.AI, {}, 
                          core.SubsystemList())
        
    def testData(self):
        stateMachine = ai.state.Machine({'name' : 'MyMachine'})
        deps = core.SubsystemList()
        deps.append(stateMachine)
        aiSys = ai.subsystem.AI({'AIMachineName' : 'MyMachine'}, deps)
        
        aiSys.data['Bob'] = 10
        self.assertEqual(10, aiSys.data['Bob'])

                    