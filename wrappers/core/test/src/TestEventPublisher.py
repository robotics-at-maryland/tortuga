# STD Imports
import unittest

# Project Imports
import ext.core as core

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

        # Send message to that function
#        self.epub.publish("TestEvent", self, testArg = 5, other = "A");

        # Make sure the event got through
        #self.assertEquals(5, reciever.args.
        #self.assertEquals("A", reciever.args.other)

        # Run a second event to make sure the events aren't cross fed
#        self.epub.publish("AnotherEvent", self, bob = "John")

    def testMultiple(self):       
#        self.senders = set()
        self.etypes = set()
#        self.args = set()
        self.aCalled = False
        self.bCalled = False
        
        def factory(signal):
            def recieve(event):
                self.etypes.add(event.type)
                #self.senders.add(event.sender)
                #self.args.add(args_)
                # Ensure that each makes a unique action, so we can be sure
                # we have two distinct functions
                setattr(self, signal, True)
            return recieve
            
        self.epub.subscribe("TestEvent", factory("aCalled"))
        self.epub.subscribe("TestEvent", factory("bCalled"))
        self.epub.publish("TestEvent", core.Event())
        self.epub.publish("AnotherEvent", core.Event())

#        self.assertEquals(self.senders, set([self]))
        self.assertEquals(self.etypes, set(["TestEvent"]))
#        self.assertEquals(self.args, set([event.EventArgs(test = 1984)]))
        self.assert_(self.aCalled)
        self.assert_(self.bCalled)


#class TestQueuedEventPublisher(unittest.TestCase):
#    def test(self):
#        emgr = event.EventPublisher()
#        recv = Reciever()

        # Create a queued event manager to listen queue up recieved events
#        qemgr = event.QueuedEventPublisher(emgr)
#        qemgr.subscribe("TestEvent", recv)

        # Publish a message and make sure its not recieved
#        emgr.publish("TestEvent", self, a = 10)
#        self.assertEquals(False, recv.called)

        # Release all messages
#        qemgr.processMessages()

#        self.assertEquals("TestEvent", recv.eventType)
#        self.assertEquals(10, recv.args.a)
#        self.assertEquals(self, recv.sender)
