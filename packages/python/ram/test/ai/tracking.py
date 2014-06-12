# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/python/ram/test/ai/tracking.py

# Python Imports
import unittest

# Project Imports
import ram.ai.bin as bin
import ram.ai.tracking as tracking
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.test.ai.support as aisupport
from ram.test import Mock
        
OBJECT_FOUND = core.declareEventType('OBJECT_FOUND')
OBJECT_DROPPED = core.declareEventType('OBJECT_DROPPED')
        
# Helper functions
class TestTracking(aisupport.AITestCase):
    def injectEventFound(self, id):
        self.publishQueuedEvent(None, OBJECT_FOUND, id = id)
    def injectEventDropped(self, id):
        self.publishQueuedEvent(None, OBJECT_DROPPED, id = id)

    def setUp(self):
        aisupport.AITestCase.setUp(self)
        tracking.ensureItemTracking(self.qeventHub, self.ai, 'trackingTest',
                                    OBJECT_FOUND, OBJECT_DROPPED)
    
    def testEnsureItemTracking(self):
        self.assertEquals(self.ai.data.has_key('trackingTest'), True)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds', set())
        self.assertDataValue(self.ai.data['trackingTest'], 'itemData', {})
        self.assertDataValue(self.ai.data['trackingTest'], 'trackingEnabled',
                             True)
        
    def testFound(self):
        # Inject an event with ID 5 and check that it's tracked
        self.injectEventFound(5)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds',
                             set([5]))
        # Check that a second event with ID 3 is found and stored
        self.injectEventFound(3)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds',
                             set([5, 3]))
        
        # Check to make sure a spot in 'itemData' has been created for both keys
        self.assertEquals(self.ai.data['trackingTest']['itemData'].has_key(5),
                          True)
        self.assertEquals(self.ai.data['trackingTest']['itemData'].has_key(3),
                          True)
        
        # Test to make sure that if 5 is found again, the currentIds don't
        # change
        self.injectEventFound(5)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds',
                             set([5, 3]))
        
    def testDropped(self):
        # Create three objects and have them tracked
        self.injectEventFound(1)
        self.injectEventFound(2)
        self.injectEventFound(3)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds',
                             set([1, 2, 3]))
        
        # Remove ID 2
        self.injectEventDropped(2)
        self.assertDataValue(self.ai.data['trackingTest'], 'currentIds',
                             set([1, 3]))
        self.assertEquals(self.ai.data['trackingTest']['itemData'].has_key(2),
                          False)