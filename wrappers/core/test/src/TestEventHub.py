# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestEventHub.py

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


class TestEventHub(unittest.TestCase):
    
    def setUp(self):
        self.ehub = core.EventHub()
        self.epubA = core.EventPublisher(self.ehub)
        self.epubB = core.EventPublisher(self.ehub)

    def testSubscribe(self):
        recvA = Reciever()
        recvB = Reciever()
        self.ehub.subscribe("Type", self.epubA, recvA)
        self.ehub.subscribe("Type", self.epubB, recvB)

        self.epubA.publish("Type", core.Event())
        self.assertEquals(1, recvA.calls)
        self.assertEquals(0, recvB.calls)

        self.epubB.publish("Type", core.Event())
        self.assertEquals(1, recvA.calls)
        self.assertEquals(1, recvB.calls)
        
        
    def testSubscribeToType(self):
        recv = Reciever()
        self.ehub.subscribeToType("Type", recv)
        self.assertEquals(0, recv.calls)

        self.epubA.publish("Type", core.Event())
        self.assertEquals(1, recv.calls)

        self.epubB.publish("Type", core.Event())
        self.assertEquals(2, recv.calls)

    def testMakeEventHub(self):
        cfg = { 'name' : 'Test', 'type' : 'EventHub' }
        cfg = core.ConfigNode.fromString(str(cfg))
        ehub = core.SubsystemMaker.newObject(cfg, core.SubsystemList())

        recv = Reciever()
        epub = core.EventPublisher(ehub)
        ehub.subscribeToType("Type", recv)
        
        epub.publish("Type", core.Event())
        self.assertEquals(1, recv.calls)

if __name__ == '__main__':
    unittest.main()
