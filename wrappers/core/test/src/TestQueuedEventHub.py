# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File: wrapper/core/test/src/TestQueuedEventHub.py

# STD Imports
import unittest

# Project Imports
import ext.core as core

class Reciever(object):
    def __init__(self):
        self.etypes = []
        self.sender = None
        self.args = None
        self.calls = 0
        self.called = False
    
    def __call__(self, event):
        self.etypes.append(event.type)
        self.sender = event.sender
        self.args = event
        self.calls += 1
        self.called = True


class TestQueuedEventHub(unittest.TestCase):
    
    def setUp(self):
        self.ehub = core.EventHub()
        self.qehub = core.QueuedEventHub(self.ehub)
        self.epubA = core.EventPublisher(self.ehub)
        self.epubB = core.EventPublisher(self.ehub)

    def testMultiple(self):
        recv = Reciever()
        types = ['A','B','C','D','E']
        for t in types:
            self.qehub.subscribeToType(t, recv)
        self.assertEquals(0, recv.calls)


        self.epubA.publish("A", core.Event())
        self.epubA.publish("B", core.Event())
        self.epubB.publish("C", core.Event())
        self.epubB.publish("D", core.Event())
        self.epubA.publish("E", core.Event())
        self.assertEquals(0, recv.calls)

        self.qehub.publishEvents()
        self.assertEquals(5, recv.calls)        

        for expType, actType in zip(types, recv.etypes):
            self.assertEquals(expType, actType)

    def testSubscrie(self):
        recv = Reciever()
        self.qehub.subscribe('A', self.epubA, recv)

        self.assertEquals(0, recv.calls)
        self.epubB.publish('A', core.Event())
        self.assertEquals(0, recv.calls)

        self.epubA.publish('A', core.Event())
        self.assertEquals(0, recv.calls)
        self.qehub.publishEvents()
        self.assertEquals(1, recv.calls)

if __name__ == '__main__':
    unittest.main()
