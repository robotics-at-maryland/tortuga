# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/core/python/core.py

# STD Imports
import warnings

# Import everything (filter out warnings from double wrapping of classes)
warnings.simplefilter('ignore', RuntimeWarning)
import ext._core as _core
from ext._core import *
warnings.simplefilter('default', RuntimeWarning)


# Create some nice wrappers for the Subsystem and Maker systems
class SubsystemMaker(_core.SubsystemMaker):
    """
    A helper class to registery subsystem makers.  An example of the usage
    of this class:

    class TestSubsystem(core.Subsystem):        
        def __init__(self, config, deps):
            core.Subsystem.__init__(self, config['name'].asString())
            ... code ...
    core.SubsystemMaker.registerSubsystem('TestSubsystem', TestSubsystem)
    """
    def __init__(self, name, createClass):
        _core.SubsystemMaker.__init__(self, name)
        self.args = None
        self._createClass = createClass

    def makeObject(self, config, deps):
        return self._createClass(config, deps)

def registerSubsystem(name, cls):
    cls.maker = SubsystemMaker(name, cls)

SubsystemMaker.registerSubsystem = staticmethod(registerSubsystem)

        
