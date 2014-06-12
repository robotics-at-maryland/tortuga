# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/vision/test/src/TestEvents.py

# STD Imports
import unittest

# Project Imports
import ext.vision as vision

TEST = True
try:
    import ext.core as core
except ImportError:
    TEST = False

if TEST:
    class TestEvents(unittest.TestCase):
        def testProps(self):
            # Get all of our event class 
            eventTypes = []
            for val in dir(vision):
                obj = getattr(vision, val)
                if isinstance(obj, type(core.Event)):
                    if issubclass(obj, core.Event):
                        eventTypes.append(obj)

            # Make sure they have the core event type props
            for eType in eventTypes:
                self.assert_(hasattr(eType, 'type'))
                self.assert_(hasattr(eType, 'sender'))

if __name__ == '__main__':
    unittest.main()
    
