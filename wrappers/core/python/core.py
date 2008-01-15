# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/core/python/core.py

# STD Imports
import sys
import StringIO
import inspect

# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # Project Imports
    import ext._core as _core
    from ext._core import *

finally:
    sys.stderr = stderr

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
        return self._createClass(eval(config.toString()), deps)

def registerSubsystem(name, cls):
    cls.maker = SubsystemMaker(name, cls)

SubsystemMaker.registerSubsystem = staticmethod(registerSubsystem)


class Subsystem(_core.Subsystem):
    """
    Implements the virtual methods or ram::core::Subsystem

    This is so that all python classes have default implementations
    """
    
    @staticmethod
    def getSubsystemOfType(_type, deps):
        """
        Returns the subsystem of the desired type from the given list
        """
        
        # Make sure we are checking subsystems
        assert issubclass(_type, _core.Subsystem)

        # Check each subsystem
        for d in deps:
            if isinstance(d, _type):
                return d

        raise Exception("Subsystem of that type not found")
    
    def __init__(self, name):
        _core.Subsystem.__init__(self, name)
        
    def background(self, interval = -1):
        pass

    def backgrounded(self):
        return False
    
    def unbackground(self, join = False):
        pass

    def update(self, timestep):
        pass
        

def declareEventType(name):
    """
    Defines an event type in a manner which will avoid collosions
    
    It defines it in the same format as in done in c++: <file>:<line> <EVENT>
    
    @rtype : str
    @return: The new event type
    """
    stack = inspect.stack()
    try:
        frame = stack[1][0]
        line = frame.f_lineno
        fileName = frame.f_code.co_filename
        return '%s:%d %s' % (fileName, line, name.replace(' ', '_').upper())
    finally:
        del stack
