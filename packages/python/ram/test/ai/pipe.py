# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/pipe.py

# Python Imports
import unittest

# Project Imports
import ram.ai.pipe as pipe
import ram.ai.state as state
import ext.core as core
import ext.vision as vision
import ext.math as math

import ram.motion as motion
import ram.motion.pipe

import ram.test.ai.support as aisupport

class PipeTest(aisupport.AITestCase):
    def injectPipeEvent(self, x, y, angle, id = 0):
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent,0,0,0, 
                         x = x, y = y, angle = angle, id = id)
    def publishQueuedPipeFound(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.PIPE_FOUND, 
                                vision.PipeEvent,0,0,0,
                                **kwargs)
    def publishQueuedPipeDropped(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.PIPE_DROPPED, 
                                vision.PipeEvent,0,0,0,
                                **kwargs)

class TestStart(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Start)
    
    def testStart(self):
        """Make sure we are diving with no detector on"""
        self.assertFalse(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertAIDataValue('pipeStartOrientation', 0)
        
    def testFinish(self):
        self.injectEvent(motion.basic.Motion.FINISHED)
        self.assertCurrentState(pipe.Searching)

class TestPipeTracking(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.PipeTrackingState)
        self._foundPipeEvent = None
        self.qeventHub.subscribeToType(pipe.PipeTrackingState.FOUND_PIPE,
                                       self._foundPipeHandler)
        
    def _foundPipeHandler(self, event):
        self._foundPipeEvent = event
        
    def testFoundPipe(self):
        cstate = self.machine.currentState()
        
        # No bias, no threshold
        cstate._biasDirection = None
        cstate._threshold = None
        self.publishQueuedPipeFound(angle = math.Degree(0), id = 0)
        
        self.assertNotEqual(None, self._foundPipeEvent)
        self._foundPipeEvent = None
        
        # Just bias
        cstate._biasDirection = math.Degree(0)
        cstate._threshold = None
        self.publishQueuedPipeFound(angle = math.Degree(0), id = 0)
        
        self.assertNotEqual(None, self._foundPipeEvent)
        self._foundPipeEvent = None
        
        # Bias and threshold inside threshold
        cstate._biasDirection = math.Degree(0)
        cstate._threshold = math.Degree(45)
        self.publishQueuedPipeFound(angle = math.Degree(35), id = 0)
        
        self.assertNotEqual(None, self._foundPipeEvent)
        self._foundPipeEvent = None
        
        # Bias and threshold outside threshold
        cstate._biasDirection = math.Degree(0)
        cstate._threshold = math.Degree(45)
        self.publishQueuedPipeFound(angle = math.Degree(50), id = 0)
        
        self.assertEqual(None, self._foundPipeEvent)
        self._foundPipeEvent = None

class TestFindAttempt(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        # Set a lastPipeEvent with the same x and y
        self.ai.data['lastPipeEvent'] = vision.PipeEvent(.5, .5, 0)
        self.machine.start(pipe.FindAttempt)

    def testStart(self):
        cstate = self.machine.currentState()
        
        self.assert_(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.basic.MoveDirection)
        self.assertAlmostEqual(cstate._direction.valueDegrees(), -45, 5)

    def testPipeFound(self):
        self.injectEvent(pipe.PipeTrackingState.FOUND_PIPE)
        self.assertCurrentState(pipe.Seeking)

    def testTimeout(self):
        self.releaseTimer(state.FindAttempt.TIMEOUT)
        self.assertCurrentState(pipe.Searching)

    def testNoPipe(self):
        # Stop the machine
        self.machine.stop()

        # Remove the last pipe event
        del self.ai.data['lastPipeEvent']

        # Start up FindAttempt
        self.machine.start(pipe.FindAttempt)
        self.assertCurrentMotion(type(None))
        self.assertAlmostEqual(0, self.controller.speed, 5)
        self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 5)
        self.assertAlmostEqual(0, self.controller.yawChange, 5)

        # Now try injecting an event to make sure it works correctly
        self.injectEvent(pipe.PipeTrackingState.FOUND_PIPE)
        self.qeventHub.publishEvents()
        self.assertCurrentState(pipe.Seeking)
        
class TestSearching(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Searching)        
    
    def testStart(self):
        """Make sure we have the detector on when starting"""
        self.assert_(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('pipeStartOrientation', 0)

    def testStartAlternate(self):
        # Stop the machine
        self.machine.stop()

        # Now set the initial direction to something other than 0
        self.ai.data['pipeStartOrientation'] = -45
        
        # Restart the machine
        self.machine.start(pipe.Searching)
        self.assert_(self.visionSystem.pipeLineDetector)
        self.assertCurrentMotion(motion.search.ForwardZigZag)
        self.assertAIDataValue('pipeStartOrientation', -45)
        self.assertLessThan(self.controller.yawChange, 0)
                
    def testPipeFound(self):
        # Now change states
        self.publishQueuedPipeFound()
        self.assertCurrentState(pipe.Seeking)
        
        # Leave and make sure its still on
        self.assert_(self.visionSystem.pipeLineDetector)

def pipeFoundHelper(self, myState = None):
    # Standard directions
    self.publishQueuedPipeFound(x = 0.5, y = -0.5, angle = math.Degree(15.0))
    self.qeventHub.publishEvents()
    self.qeventHub.publishEvents()
    self.qeventHub.publishEvents()
    
    self.assertLessThan(self.controller.speed, 0)
    self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now lets test biasing with vehicle at -22.5, and pipe 22.5 to the right
    # we will do both +90, and -90 (west and east) bias direction.  Absolute
    # pipe orientation is -45 degrees.
    self.vehicle.orientation = math.Quaternion(math.Degree(-22.5),
                                               math.Vector3.UNIT_Z)
    self.controller.setDesiredOrientation(self.vehicle.orientation)
    
    cstate = self.machine.currentState()
    cstate._biasDirection = math.Degree(-90)
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    cstate._biasDirection = math.Degree(90)
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers without bias, to ensure it moves the opposite way
    cstate._biasDirection = None
    
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers with the bias in the right direction
    cstate._biasDirection = math.Degree(-45)
    
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    # Now lets test biasing with vehicle at 22.5, and pipe 22.5 to the left
    # we will do both +90, and -90 (west and east) bias direction.  Absolute
    # pipe orientation is 45 degrees.
    self.vehicle.orientation = math.Quaternion(math.Degree(22.5),
                                               math.Vector3.UNIT_Z)
    self.controller.setDesiredOrientation(self.vehicle.orientation)
    
    cstate._biasDirection = math.Degree(-90)
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertLessThan(self.controller.yawChange, 0)
    
    cstate._biasDirection = math.Degree(90)
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now the same maneuvers without bias, to ensure it moves the opposite way
    cstate._biasDirection = None
    
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
        
    # Now the same maneuvers with the bias in the right direction
    cstate._biasDirection = math.Degree(45)
    
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(22.5))
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Check if it doesn't change it's pipe without a bias
    cstate._biasDirection = None
    
    # This should be ignored and the currentID should not be changed
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-45), id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 0)
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # This should cause the vehicle to change its current ID and begin moving
    # towards the new target
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-15), id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 1)
    self.assertLessThan(self.controller.yawChange, 0)
    
    # Now the drop the pipe current pipe, make sure it switches back to the 
    # old ID
    self.publishQueuedPipeDropped(id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 0)
    self.assertGreaterThan(self.controller.yawChange, 0)
    
    # Now send in a new event and make sure it properly picks out the right pipe
    # TODO: write me
    # jsternberg: ??? I'm ignoring this for now and continuing with the tests

    # Now check if it doesn't change it's pipe with a bias
    cstate._biasDirection = -90 # To account for the vehicle orientation

    # This should be ignored and the currentID should not be changed
    # NOTE TO SELF: Ask why y = -0
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(45), id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 0)
    self.assertGreaterThan(self.controller.yawChange, 0)

    # This should cause the vehicle to change its current ID and move towards
    # the new pipe
    self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-90), id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 1)
    self.assertLessThan(self.controller.yawChange, 0)

    # Now drop the old pipe and make sure we aren't still storing abs. direction
    self.publishQueuedPipeDropped(id = 0)
    self.assertFalse(self.ai.data['pipeData']['absoluteDirection'].has_key(0))

    # Drop the new pipe and start anew
    self.publishQueuedPipeDropped(id = 1)

    # Check the threshold
    cstate._biasDirection = math.Degree(0)
    cstate._threshold = math.Degree(45)

    # Now inject an event that should be ignored because it's out of the
    # threshold
    self.publishQueuedPipeFound(x = 0, y = 0, angle = math.Degree(90))
    self.assertEquals(self.ai.data['pipeData'].has_key('currentID'), False)

    # Inject an event with a pipe that goes the wrong way, but is in the
    # threshold
    self.publishQueuedPipeFound(x = 0, y = 0, angle = math.Degree(135), id = 1)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 1)
    self.assertLessThan(self.controller.yawChange, 0)

    # Now inject a normal event that is within the normal range
    self.publishQueuedPipeFound(x = 0, y = 0, angle = math.Degree(15), id = 2)
    self.assertDataValue(self.ai.data['pipeData'], 'currentID', 2)
    self.assertGreaterThan(self.controller.yawChange, 0)

    # Now drop the last pipe and make sure currentID is deleted
    self.publishQueuedPipeDropped(id = 0)
    self.publishQueuedPipeDropped(id = 1)
    self.publishQueuedPipeDropped(id = 2)
    self.assertEquals(self.ai.data['pipeData'].has_key('currentID'), False)

    # Set up the next tests
    self.ai.data['pipeData']['currentID'] = 2
    
    # One of the previous tests should have made the lastPipeEvent value
    self.assertTrue(self.ai.data.has_key('lastPipeEvent'))
        
    # Find the current pipe and check the event is called
    del self.ai.data['lastPipeEvent']
    self.publishQueuedPipeFound(id = 2)
    self.assertTrue(self.ai.data.has_key('lastPipeEvent'))

    # Find a different pipe and make sure it wasn't stored
    del self.ai.data['lastPipeEvent']
    self.publishQueuedPipeFound(id = 1)
    self.assertFalse(self.ai.data.has_key('lastPipeEvent'))
    
    # Delete pipe data for the next tests
    #self.ai.data['pipeData'] = {'currentIds' : set(),
    #                            'trackingEnabled' : True}
    
    # Subscribe to the PIPE_FOUND event and FOUND_PIPE event
    # Make sure that the id was stored before FOUND_PIPE happens
    del self.ai.data['pipeData']['absoluteDirection']
    del self.ai.data['pipeData']['currentID']
    del self.ai.data['pipeData']['currentIds']
    self.ai.data['pipeData']['currentIds'] = set()
    del self.ai.data['pipeData']['itemData']
    self.ai.data['pipeData']['itemData'] = {}
    
    self._foundPipe = False
    def foundPipeHandler(event):
        self._foundPipe = True
        self.qeventHub.publishEvents()

    self.qeventHub.subscribeToType(pipe.PipeTrackingState.FOUND_PIPE,
                                       foundPipeHandler)
    
    # Publish a found event
    event = self._createEvent(vision.EventType.PIPE_FOUND,
                              vision.PipeEvent, 0,0,0, id = 1)
    self.qeventHub.publish(vision.EventType.PIPE_FOUND, event)
    
    # Now lose it
    #event = self._createEvent(vision.EventType.PIPE_DROPPED,
    #                          vision.PipeEvent, 0,0,0, id = 1)
    #self.qeventHub.publish(vision.EventType.PIPE_DROPPED, event)
    
    # Publish the events
    self.qeventHub.publishEvents()
    
    # Make sure it didn't crash and isn't doing anything
    self.assertAlmostEqual(0, self.controller.speed, 2)
    self.assertAlmostEqual(0, self.controller.sidewaysSpeed, 2)
    self.assertAlmostEqual(0, self.controller.yawChange, 5)
    self.assert_(self._foundPipe)

class TestSeeking(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Seeking)
    
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Hover)
        
    def testPipeFound(self):
        """Make sure new found events move the vehicle"""
        pipeFoundHelper(self, pipe.Seeking)
    
    def testPipeLost(self):
        """Make sure losing the pipe goes back to search"""
        # Setup data
        self.ai.data['pipeData']['absoluteDirection'] = {}
        self.ai.data['pipeData']['currentID'] = 0

        # Set the lastPipeEvent for FindAttempt
        self.ai.data['lastPipeEvent'] = vision.PipeEvent()
        
        self.injectEvent(vision.EventType.PIPE_LOST)

        # Make sure we changed state properly
        self.assertCurrentState(pipe.FindAttempt)

        # Make sure we dropped the currentID
        self.assertFalse(self.ai.data['pipeData'].has_key('currentID'))
        self.assertFalse(self.ai.data['pipeData'].has_key('absoluteDirection'))
        
class TestCentering(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.Centering)
        
    def testStart(self):
        """Make sure the motion and the timer are setup properly"""
        self.machine.stop()
        self.machine.start(pipe.Centering)
        
        self.assertCurrentMotion(motion.pipe.Hover)
        
        self.releaseTimer(pipe.Centering.SETTLED)
        self.assertCurrentState(pipe.AlongPipe)
        
    def testPipeLost(self):
        """Make sure we search when we lose the pipe"""
        # Set the last pipe event for FindAttempt
        self.ai.data['lastPipeEvent'] = vision.PipeEvent()
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(pipe.FindAttempt)
    
    def testPipeFound(self):
        """Make sure the loop back works"""
        pipeFoundHelper(self)
    
    def testSettled(self):
        """Make sure we move on after settling"""
        self.injectEvent(pipe.Centering.SETTLED)
        self.assertCurrentState(pipe.AlongPipe)

class TestAlongPipe(PipeTest):
    def setUp(self):
        PipeTest.setUp(self)
        self.machine.start(pipe.AlongPipe)
        
    def testStart(self):
        self.assertCurrentMotion(motion.pipe.Follow)
        
    def testPipeFound(self):
        """Make sure it has an effect on the vehicle, and throws event"""
        
        # The outside angle case
        self.publishQueuedPipeFound(x = 0.5, y = -0.5, angle = math.Degree(15.0))
        
        # Goes forward even when pipe behind vehicle
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertEqual(0, self.controller.yawChange)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
        # The inside range case
        self.publishQueuedPipeFound(x = 0.4, y = -0.4, angle = math.Degree(15.0))
        self.assertGreaterThan(self.controller.speed, 0)
        self.assertGreaterThan(self.controller.sidewaysSpeed, 0)
        self.assertGreaterThan(self.controller.yawChange, 0)
        self.controller.speed = 0
        self.controller.sidewaysSpeed = 0
        
    
        # Test the transition to a new pipe (ie, now pipes in front)
        self.called = False
        def handler(event):
            self.called = True
        self.eventHub.subscribeToType(pipe.AlongPipe.FOUND_NEW_PIPE, handler)
        
        self.publishQueuedPipeFound(x = 0.4, y = 0.4, angle = math.Degree(15.0))
        
        self.assert_(self.called)
        
    def testPipeLost(self):
        """Make sure it goes to between pipes, when pipe is lost"""
        self.ai.data['pipeData']['currentID'] = 0
        self.ai.data['pipeData']['absoluteDirection'] = {}
        
        self.injectEvent(vision.EventType.PIPE_LOST)
        
        # Make sure we changed state
        self.assertCurrentState(pipe.BetweenPipes)
        # Make sure we dropped the currentID
        self.assertFalse(self.ai.data['pipeData'].has_key('currentID'))
        self.assertFalse(self.ai.data['pipeData'].has_key('absoluteDirection'))

    def testPipeDropped(self):
        """Ensure that when the current pipe is dropped its no longer current"""

        # Get it tracking a pipe
        self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-90),
                                    id = 1)
        self.qeventHub.publishEvents()
        self.assertDataValue(self.ai.data['pipeData'], 'currentID', 1)

        # Now drop the pipe and make sure the current id is removed
        self.publishQueuedPipeDropped(id = 1)
        self.assertFalse(self.ai.data['pipeData'].has_key('currentID'))


class TestBetweenPipes(PipeTest):
    def testStart(self):
        # Make sure we start driving forward
        self.machine.start(pipe.BetweenPipes)
        self.assert_(self.controller.speed > 0)
        
        # Now make sure we stop
        self.releaseTimer(pipe.BetweenPipes.LOST_PATH)
        self.assertEqual(0, self.controller.speed)
        
        # Make sure we hit the end state
        self.assert_(self.machine.complete)
    
    def testPipeFound(self):
        """If we find a pipe, seek it"""
        self.machine.start(pipe.BetweenPipes)
        self.publishQueuedPipeFound()
        self.assertCurrentState(pipe.Seeking)
