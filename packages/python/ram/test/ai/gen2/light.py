# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/gen2/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.gen2.light as light2
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

"""
NOTES:

This file will conflict with the ram.ai.light test file if any of the
names are the same. I am working on a solution, but to get around this
problems, merely put a 2 at the end of any conflicting variable.

I think I know the reason why this conflict happens and I'll try to fix it.
"""

def initializeConfig(name, pos):
    cfg = {
        'Vehicle' : {
            'StateEstimator' : {
                name : pos
                }
            }
        }
    return cfg

class TestStart2(support.AITestCase):
    def testSuccess(self):
        """Makes sure the state continues when it has the proper config"""
        cfg = initializeConfig('buoy', [0, 0, 0, 0])

        support.AITestCase.setUp(self, cfg = cfg)
        
        self._success = False
        def _handler(event):
            self._success = True
        self.qeventHub.subscribeToType(light2.Start.SUCCESS, _handler)
        self.machine.start(light2.Start)

        self.qeventHub.publishEvents()
        self.assert_(self._success)

    def testFailure(self):
        support.AITestCase.setUp(self)

        self._failure = False
        def _handler(event):
            self._failure = True
        self.qeventHub.subscribeToType(light2.Start.FAIL, _handler)
        self.machine.start(light2.Start)

        self.qeventHub.publishEvents()
        self.assert_(self._failure)

    def testInit(self):
        cfg = initializeConfig('buoy', [0, 0, 0, 0])
        support.AITestCase.setUp(self, cfg = cfg)

        self.machine.start(light2.Start)

        # Interstate data has been declared
        self.assert_(self.ai.data.has_key('light'))
