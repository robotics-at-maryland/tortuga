# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: wrapper/network/test/src/TestNetworkHub.py

# STD Imports
import os
import unittest

# Project Imports
import ext.core as core
import ext.network

class TestNetworkPublisher(unittest.TestCase):
    def testSubsystemMaker(self):
        # Must pass an event hub to the network publisher
        eventHub = core.EventHub()
        deps = core.SubsystemList()
        deps.append(eventHub)

        cfg = {
            'type' : 'NetworkPublisher'
            }
        cfg = core.ConfigNode.fromString(str(cfg))
        obj = core.SubsystemMaker.newObject(cfg, deps)

        # Make sure we have the right methods on the network object
        self.assert_(hasattr(obj, 'subscribe'))
        self.assert_(hasattr(obj, 'publish'))

if __name__ == '__main__':
    unittest.main()
