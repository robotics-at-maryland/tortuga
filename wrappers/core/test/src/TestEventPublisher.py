# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestEventPublisher.py

# STD Imports
import unittest

# Project Imports
import ext.core as core

class PythonEvent(core.Event):
    def __init__(self):
        core.Event.__init__(self)
        self.someValue = 10

class Reciever(object):
    def __init__(self):
        self.etype = None
        self.sender = None
        self.args = None
        self.calls = 0
        self.called = False
    
    def __call__(self, event):
        self.etype = event.type
        self.sender = event.sender
        self.args = event
        self.calls += 1
        self.called = True

class TestEventPublisher(unittest.TestCase):
    EVTP_NAME = "OUR_NAME"
    
    def setUp(self):
        self.epub = core.EventPublisher(name = TestEventPublisher.EVTP_NAME)

    def memberHandler(self, event):
        pass

    def testBadCallable(self):
        # Non Function
        self.assertRaises(TypeError, self.epub.subscribe, "Test", 1)

        # Wrong function type
        def noArgs():
            pass
        self.assertRaises(TypeError, self.epub.subscribe, "Test", noArgs)

        def tooManyArgs(a, b):
            pass
        self.assertRaises(TypeError, self.epub.subscribe, "Test", tooManyArgs)

        def tooManyArgsVar(a, b, *args):
            pass
        self.assertRaises(TypeError, self.epub.subscribe, "Test",
                          tooManyArgsVar)

        def tooManyDefArgs(a, b, c = 1):
            pass
        self.assertRaises(TypeError, self.epub.subscribe, "Test",
                          tooManyDefArgs)
        
        # Corner cases, that should still pass
        def varArgs(*args):
            pass
        self.epub.subscribe("Test", varArgs)
        
        def goodWithVar(a, *args):
            pass
        self.epub.subscribe("Test", goodWithVar)

        def goodWithDefault(a = 1):
            pass
        self.epub.subscribe("Test", goodWithDefault)

        def goodWithDefArgs(a, b = 1):
            pass
        self.epub.subscribe("Test", goodWithDefArgs)

        self.epub.subscribe("Test", self.memberHandler)

    def testSend(self):
        self.epub = core.EventPublisher(name = TestEventPublisher.EVTP_NAME)        # Handler for the function
        reciever = Reciever()

        # Register function to recieve the event
        self.epub.subscribe("TestEvent", reciever)

        # Test Basic event publishing
        self.epub.publish("TestEvent", core.Event())

        self.assert_(reciever.called)
        self.assertEquals(1, reciever.calls)
        self.assertEquals("TestEvent", reciever.etype)

    def testConnections(self):
        # Handler for the function
        reciever = Reciever()

        # Register function to recieve the event
        connection = self.epub.subscribe("TestEvent", reciever)

        self.epub.publish("TestEvent", core.Event())
        self.assertEquals(1, reciever.calls)

        # Remove connection then make sure we get no more events
        connection.disconnect();
        self.assertEquals(1, reciever.calls)

    def testPythonEvent(self):
        reciever = Reciever()
        self.epub.subscribe("TestEvent", reciever)
        pyEvent = PythonEvent()
        self.epub.publish("TestEvent", pyEvent)
        self.assert_(reciever.called)
        self.assertEquals(1, reciever.calls)
        self.assertEquals("TestEvent", reciever.etype)
        self.assertEquals(10,reciever.args.someValue)

    def testMultiple(self):       
        self.etypes = set()
        self.aCalled = False
        self.bCalled = False
        
        def factory(signal):
            def recieve(event):
                self.etypes.add(event.type)
                setattr(self, signal, True)
            return recieve
            
        self.epub.subscribe("TestEvent", factory("aCalled"))
        self.epub.subscribe("TestEvent", factory("bCalled"))
        self.epub.publish("TestEvent", core.Event())
        self.epub.publish("AnotherEvent", core.Event())

        self.assertEquals(self.etypes, set(["TestEvent"]))
        self.assert_(self.aCalled)
        self.assert_(self.bCalled)

    def testLookupByName(self):
        self.called = False
        def recieve(event):
            self.called = True

        # Create a publisher with an event hub
        eventHub = core.EventHub()
        mypub = core.EventPublisher(eventHub = eventHub,
                                    name = TestEventPublisher.EVTP_NAME)

        # Look it up and make sure it works
        epub = core.EventPublisher.lookupByName(TestEventPublisher.EVTP_NAME)
        self.assertNotEquals(None, epub)
        self.assertEquals(TestEventPublisher.EVTP_NAME, epub.getPublisherName())

        # Subscribe to the new one (which is wrapper of the same underlying
        # C++ object as self.epub)
        # BUG: This line below should work instead of the one two down
#        eventHub.subscribe("TestEvent", epub, recieve)
        eventHub.subscribe("TestEvent", mypub, recieve)

        # Send through our local
        mypub.publish("TestEvent", core.Event())

        # Make sure we got it
        self.assert_(self.called)

class TestQueuedEventPublisher(unittest.TestCase):
    def test(self):
        epub = core.EventPublisher()
        recv = Reciever()

        # Create a queued event manager to listen queue up recieved events
        qepub = core.QueuedEventPublisher(epub)
        qepub.subscribe("TestEvent", recv)


        # Test Basic event publishing
        epub.publish("TestEvent", core.Event())
        self.assertEquals(0, recv.calls)

        # Release all messages
        qepub.publishEvents()
        
        self.assertEquals(1, recv.calls)
        self.assertEquals("TestEvent", recv.etype)
        
if __name__ == '__main__':
    unittest.main()
