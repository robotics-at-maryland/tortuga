# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestApplication.py

# STD Imports
import unittest
import os
import os.path

# Project Imports
import ext.core as core

def getConfigRoot():
    root = os.environ['RAM_SVN_DIR']
    return os.path.join(root, 'wrappers', 'core', 'test', 'data')

# Test subsystems
class SubsystemBase(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'].asString())
        self.config = config
        self.deps = deps

class SubsystemA(SubsystemBase):
    def __init__(self, config, deps):
        SubsystemBase.__init__(self, config, deps)

    def funcA(self):
        return self.config["AData"].asInt()

core.SubsystemMaker.registerSubsystem('SubsystemA', SubsystemA)

class SubsystemB(SubsystemBase):
    def __init__(self, config, deps):
        SubsystemBase.__init__(self, config, deps)

    def funcB(self):
        return self.config["BData"].asInt()
        
core.SubsystemMaker.registerSubsystem('SubsystemB', SubsystemB)

class TestApplication(unittest.TestCase):
    def testGetSubsytem(self):
        configPath = os.path.join(getConfigRoot(), 'simpleSubsystem.yml')
        app = core.Application(configPath)

        subsystemA = app.getSubsystem("SystemA")
        self.assertEquals("SystemA", subsystemA.getName())
        self.assertEquals(27, subsystemA.funcA())

        subsystemB = app.getSubsystem("SystemB")
        self.assertEquals("SystemB", subsystemB.getName())
        self.assertEquals(5, subsystemB.funcB())
        
if __name__ == '__main__':
    unittest.main()
