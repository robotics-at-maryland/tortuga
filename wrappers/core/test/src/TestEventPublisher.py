# STD Imports
import unittest
import os
import os.path
import sys

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
    
    def setUp(self):
        self.epub = core.EventPublisher()

    def testSend(self):
        # Handler for the function
        reciever = Reciever()

        # Register function to recieve the event
        self.epub.subscribe("TestEvent", reciever)

        # Test Basic event publishing
        self.epub.publish("TestEvent", core.Event())

        self.assert_(reciever.called)
        self.assertEquals(1, reciever.calls)
        self.assertEquals("TestEvent", reciever.etype)

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
