# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/safe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.safe as safe 
import ext.core as core
import ext.vision as vision

import ram.motion as motion
import ram.motion.common

import ram.test.ai.support as aisupport

class TestSearching(aisupport.AITestCase):
    def setUp(self):
        aisupport.AITestCase.setUp(self)
        self.machine.start(safe.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.downwardSafeDetector)
        #self.assertCurrentMotion(motion.search.ForwardZigZag)
                
    def testLightFound(self):
        # Now change states
        self.injectEvent(vision.EventType.SAFE_FOUND, vision.SafeEvent, 0, 
                         0) #0)
        self.assertCurrentState(safe.Seeking)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.downwardSafeDetector)
        