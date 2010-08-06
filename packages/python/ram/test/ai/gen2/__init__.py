# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/gen2/light.py

# Python Imports
import unittest

# Project Imports
import ram.ai.gen2.light as light
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.search

import ram.test.ai.support as support

def initializeConfig(name, loc):
    cfg = {
        'Vehicle' : {
            'StateEstimator' : {
                'Objects' : {
                    name : loc
                    }
                }
            }
        }
    return cfg

class TestStart(support.AITestCase):
    def setUp(self):
        cfg = initializeConfig('buoy', [0, 0, 0, 0])

        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(light.Start)

    def testStart(self):
        """Make sure it can find the buoy in the map"""
        self.qeventHub.publishEvents()
        self.assertCurrentState(light.Dive)

