# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestSubsytem.py

# STD Imports
import unittest

# Project Imports
import ext.core as core

class SubsystemA(core.Subsystem):
    def __init__(self, config, deps):
        eventHub = core.Subsystem.getSubsystemOfType(core.EventHub, deps)
        core.Subsystem.__init__(self, config.get('name', 'SubsystemA'),
                                eventHub)

class SubsystemB(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'SubsystemB'))

class SubsystemC(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'SubsystemC'), deps)

class SubsystemD(core.Subsystem):
    pass

class TestSubsystem(unittest.TestCase):
    def testInit(self):
        # A
        subsystemA = SubsystemA({}, [])
        subsystemA = SubsystemA({'name' : 'John'}, [core.EventHub()])

        # B
        subsystemB = SubsystemB({}, None)
        subsystemB = SubsystemB({'name' : 'Bob'}, None)

        # C
        subsystemC = SubsystemC({}, None)
        
        subsystemC = SubsystemC({}, core.SubsystemList())
        subsystemC = SubsystemC({}, [])

        deps = core.SubsystemList()
        deps.append(core.EventHub())
        subsystemC = SubsystemC({'name' : 'Dave'}, deps)
        subsystemC = SubsystemC({'name' : 'Dave'}, [core.EventHub()])

    def testGetSubsystemOfType(self):
        a = core.Subsystem('Bob')
        b = SubsystemD('John')
        deps = [b, a]

        # Empty List
        core.Subsystem.getSubsystemOfType(SubsystemD, [])
        self.assertRaises(Exception,core.Subsystem.getSubsystemOfType,
                          SubsystemD, [], nonNone = True)

        # Actual List
        subsystem = core.Subsystem.getSubsystemOfType(SubsystemD, deps)
        self.assertEquals(b, subsystem)

        # Note how we get a 'b' where, which is really of type SubsystemD
        subsystem = core.Subsystem.getSubsystemOfType(core.Subsystem, deps)
        self.assertEquals(b, subsystem)
                    

    def testGetSubsystemOfExactType(self):
        a = core.Subsystem('Bob')
        b = SubsystemD('John')
        deps = [a, b]

        # Empty List
        core.Subsystem.getSubsystemOfExactType(SubsystemD, [])
        self.assertRaises(Exception,core.Subsystem.getSubsystemOfExactType,
                          SubsystemD, [], nonNone = True)

        # Actual List
        subsystem = core.Subsystem.getSubsystemOfExactType(SubsystemD, deps)
        self.assertEquals(b, subsystem)
                          
        subsystem = core.Subsystem.getSubsystemOfExactType(core.Subsystem, deps)
        self.assertEquals(a, subsystem)
        

if __name__ == '__main__':
    unittest.main()
