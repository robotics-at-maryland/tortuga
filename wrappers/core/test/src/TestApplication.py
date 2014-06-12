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
        core.Subsystem.__init__(self, config['name'])
        self.config = config
        self.deps = deps

class SubsystemA(SubsystemBase):
    def __init__(self, config, deps):
        SubsystemBase.__init__(self, config, deps)

    def funcA(self):
        return self.config["AData"]

core.SubsystemMaker.registerSubsystem('SubsystemA', SubsystemA)

class SubsystemB(SubsystemBase):
    def __init__(self, config, deps):
        SubsystemBase.__init__(self, config, deps)

    def funcB(self):
        return self.config["BData"]
        
core.SubsystemMaker.registerSubsystem('SubsystemB', SubsystemB)

class LoopSubsystem(SubsystemBase):
    STOP = 'LOOPSUBSYSTEM_STOP'
    def __init__(self, config, deps):
        SubsystemBase.__init__(self, config, deps)
        self.iterations = config.get('iterations', 10)

    def update(self, timeSinceLastUpdate):
        if 0 == self.iterations:
            self.publish(LoopSubsystem.STOP, core.Event())
        self.iterations -= 1

core.SubsystemMaker.registerSubsystem('LoopSubsystem', LoopSubsystem)

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

    def testMainLoop(self):
        configPath = os.path.join(getConfigRoot(), 'loopSubsystems.yml')
        app = core.Application(configPath)

        def stop(event):
            app.stopMainLoop()

        app.getSubsystem('A').subscribe(LoopSubsystem.STOP,  stop)
        app.mainLoop()

        subsystemIter = (app.getSubsystem(name) for name in 
                         app.getSubsystemNames())
        for subsystem in subsystemIter:
            self.assertEquals(-1, subsystem.iterations)
        
if __name__ == '__main__':
    unittest.main()
