# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestSubsytemMaker.py

import warnings
import unittest

warnings.simplefilter('ignore', RuntimeWarning)
import ext.core as core
warnings.simplefilter('default', RuntimeWarning)


class SubsystemMaker(core.SubsystemMaker):
    """
    A helper class to registery subsystem makers
    """
    def __init__(self, name, createClass):
        core.SubsystemMaker.__init__(self, name)
        self.args = None
        self._createClass = createClass

    def makeObject(self, config, deps):
        return self._createClass(config, deps)

    @staticmethod
    def register(name, cls):
        return SubsystemMaker(name, cls)
        
class Subsystem(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config['name'].asString())
        self.config = config
        self.deps = deps

maker = SubsystemMaker.register('PythonSubsystem', Subsystem)


class TestSubsystemMaker(unittest.TestCase):
    def test(self):
        cfg = { 'name' : 'Test', 'type' : 'PythonSubsystem' }
        cfg = core.ConfigNode.fromString(str(cfg))
        obj = SubsystemMaker.newObject(cfg, core.SubsystemList())

        self.assertEquals('Test', obj.getName())
        self.assertEquals(0, len(obj.deps))
        self.assert_(isinstance(obj, Subsystem))
        

if __name__ == '__main__':
    unittest.main()
