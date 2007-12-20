# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestSubsytemMaker.py

# STD Imports
import unittest

# Project Imports
import ext.core as core

class TestSubsystem(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'])
        self.config = config
        self.deps = deps
        
core.SubsystemMaker.registerSubsystem('TestSubsystem', TestSubsystem)

class TestSubsystemMaker(unittest.TestCase):
    def test(self):
        cfg = { 'name' : 'Test', 'type' : 'TestSubsystem' }
        cfg = core.ConfigNode.fromString(str(cfg))
        obj = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        self.assertEquals('Test', obj.getName())
        self.assertEquals(0, len(obj.deps))
        self.assert_(isinstance(obj, core.Subsystem))
        

if __name__ == '__main__':
    unittest.main()
