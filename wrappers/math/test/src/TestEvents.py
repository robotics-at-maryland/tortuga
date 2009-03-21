# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/math/test/src/TestEvents.py

# STD Imports
import unittest

# Project Imports
import ext.math as math

TEST = True
try:
    import ext.core as core
except ImportError:
    TEST = False

if TEST:
    class TestEvents(unittest.TestCase):
        def setUp(self):
            self.eventHub = core.EventHub()
            self.qeventHub = core.QueuedEventHub(self.eventHub)
            self.epub = core.EventPublisher()

        def testProps(self):
            # Get all of our event class 
            eventTypes = []
            for val in dir(math):
                obj = getattr(math, val)
                if isinstance(obj, type(core.Event)):
                    if issubclass(obj, core.Event):
                        eventTypes.append(obj)

            # Make sure they have the core event type props
            for eType in eventTypes:
                self.assert_(hasattr(eType, 'type'))
                self.assert_(hasattr(eType, 'sender'))
            
        def testNumericEvent(self):
            def handler(event):
                self.number = event.number
        
            self.epub.subscribe("TEST", handler)

            newEvent = math.NumericEvent()
            newEvent.number = 5 
            self.epub.publish("TEST", newEvent)
            self.assertEquals(5, self.number)
            
            # Try with the hub
            self.qeventHub.subscribeToType("TEST", handler)

            newEvent = math.NumericEvent()
            newEvent.number = 8
            self.epub.publish("TEST", newEvent)
            self.qeventHub.publishEvents()
            self.assertEquals(8, self.number)

if __name__ == '__main__':
    unittest.main()
    
