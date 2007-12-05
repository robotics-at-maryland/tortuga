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

    def makeObject(self, args):
        return self._createClass(args.first, args.second)

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
        args = core.SubsystemMakerArgs(cfg, core.SubsystemList())
        obj = SubsystemMaker.newObject(args)

        self.assertEquals('Test', obj.getName())
        self.assertEquals(0, obj.deps.size())
        self.assert_(isinstance(obj, Subsystem))
        

if __name__ == '__main__':
    unittest.main()
