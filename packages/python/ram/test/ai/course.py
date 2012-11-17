# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/ai/course.py

"""
This module tests the ram.ai.course state Machine
"""

# Python Imports
import unittest

# Project Imports
import ext.vision as vision
import ext.math as math

import ram.ai.course as course
import ram.ai.task as task
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.buoy as buoy
import ram.ai.barbedwire as barbedwire
import ram.ai.hedge as hedge
import ram.ai.target as target
import ram.ai.window as window
import ram.ai.bin as bin
import ram.ai.sonar as sonar
import ram.ai.safe as safe
import ram.ai.sonarSafe as sonarSafe

import ram.motion as motion
import ram.motion.basic
import ram.motion.pipe
import ram.motion.search

import ram.test.ai.support as support
from ram.test.motion.support import MockTimer

class PipeTestCase(support.AITestCase):
    def setUp(self, testState = pipe.Searching, cfg = None):
        support.AITestCase.setUp(self, cfg = cfg)
        self.testState = testState
    
    def checkStart(self, currentState):
        self.assertCurrentState(currentState)
        self.assertCurrentBranches([self.testState])
        
    def checkSettled(self, nextState = None):
        self.injectEvent(pipe.Centering.SETTLED, sendToBranches = True)
        self.qeventHub.publishEvents()
        if not (nextState is None):
            self.assertCurrentState(nextState)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(self.testState))

        # Make sure we are not moving
        self.assertEqual(0, self.controller.speed)
        self.assertEqual(0, self.controller.sidewaysSpeed)
        
        # For the time being this is off
        self.assertFalse(self.visionSystem.pipeLineDetector)
        
    def injectPipeEvent(self, x, y, angle):
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent,0,0,0, 
                         x = x, y = y, angle = angle, sendToBranches = True)    

class TestPipeBias(PipeTestCase):
    def publishQueuedPipeFound(self, **kwargs):
        self.publishQueuedEvent(self.ai, vision.EventType.PIPE_FOUND, 
                                vision.PipeEvent,0,0,0,
                                **kwargs)
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Pipe', 'ram.ai.course.Bin'],
                        'config' : {
                                'Pipe' : {
                                        'biasDirection' : 0
                                }
                        }
                }
            }
        PipeTestCase.setUp(self, pipe.Start, cfg)
        self.machine.start(course.Pipe)
        
    def testDirection(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentBranchState(pipe.Start, pipe.Start)
        
        # Get us from the start state to the Seeking state
        self.injectEvent(motion.basic.MotionManager.FINISHED,
                         sendToBranches = True)
        self.assertCurrentBranchState(pipe.Start, pipe.Searching)
        
        self.publishQueuedPipeFound(x = 0, y = 0, angle = math.Degree(0))
        self.assertCurrentBranchState(pipe.Start, pipe.Seeking)
        
        # Now make sure the biasing has taken effect
        
        # Now lets test biasing with vehicle at -80, and pipe 75 to the left
        # The bias is 0 degrees (north), absolute pipe direction is -155 deg.
        self.estimator.orientation = math.Quaternion(math.Degree(-80),
                                                     math.Vector3.UNIT_Z)
        self.controller.setDesiredOrientation(self.estimator.orientation)
        
        self.publishQueuedPipeFound(x = 0, y = -0, angle = math.Degree(-75))
        self.assertGreaterThan(self.controller.yawChange, 0)


class TestGate(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Gate', 'ram.ai.course.PipeGate'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Gate)
        
    def testStart(self):
        self.assertCurrentState(course.Gate)
        
        # Make sure we branched to the right state machine
        self.assertCurrentBranches([gate.Start])
        self.assertFalse(self.visionSystem.pipeLineDetector)
        self.assertAIDataValue('foundPipeEarly', False)
    
    def testStoreDesiredQuaternion(self):
        # Setup a desired orientation
        expected = math.Quaternion(math.Degree(45), math.Vector3.UNIT_Z)
        self.controller.desiredOrientation = expected
        
        # Restart state machine so it loads the orientation
        self.machine.stop()
        self.machine.start(course.Gate)
        
        # Make sure we have the desired quaterion saved properly
        self.assertAIDataValue('gateOrientation', expected)
    
    def testPipeOn(self):
        self.releaseTimer(course.Gate.PIPE_ON)
        self.assert_(self.visionSystem.pipeLineDetectorOn)
    
    def testPipeFound(self):
        self.assertCurrentState(course.Gate)
        self.injectEvent(vision.EventType.PIPE_FOUND, vision.PipeEvent, 0, 
                         0, 0)
        self.assertCurrentState(course.PipeGate)
        self.assertAIDataValue('foundPipeEarly', True)
        
    def testGateComplete(self):
        # Make sure we have moved onto the next state
        self.injectEvent(gate.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.PipeGate)
        
        # Make sure the gate.Dive branch is gone
        self.assertFalse(self.machine.branches.has_key(gate.Start))
        
        # Make sure we are watching for the pipe
        self.assert_(self.visionSystem.pipeLineDetector)
        
class TestPipe(PipeTestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Pipe', 'ram.ai.course.Bin'] } }
        PipeTestCase.setUp(self, pipe.Start, cfg)
        self.machine.start(course.Pipe)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.Pipe)
        #self.assert_(self.visionSystem.pipeLineDetector)
        
    def testFinish(self):
        """
        Makes sure that the pipe task cleans up after itself
        """
        self.ai.data['pipeBiasDirection'] = 45
        self.ai.data['pipeThreshold'] = 45
        self.ai.data['pipeStartOrientation'] = 45

        self.machine.stop()

        self.assert_(not self.ai.data.has_key('pipeBiasDirection'))
        self.assert_(not self.ai.data.has_key('pipeThreshold'))
        self.assert_(not self.ai.data.has_key('pipeStartOrientation'))

    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, course.Bin)
        
class TestPipeMultiple(PipeTestCase):
    """
    Tests the Pipe state machine when its trying to find multiple pipes
    """
    PIPES_TO_FIND = 2
    
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.course.Pipe' : {
                        'pipesToFind' : TestPipeMultiple.PIPES_TO_FIND,
                    },
                }
            }, 
            'Ai' : {'taskOrder' : 
                    ['ram.ai.course.Pipe', 'ram.ai.course.Bin'] } 
        }
        
        PipeTestCase.setUp(self, pipe.Start, cfg)
        self.machine.start(course.Pipe)
        
        
    def testConfig(self):
        """
        Make sure the configuration settings are read properly
        """
        cstate = self.machine.currentState()
        self.assertEqual(self.PIPES_TO_FIND, cstate.pipesToFind)
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        
        # Send one settle event and make sure we are still in the proper state
        self.injectEvent(pipe.Centering.SETTLED, sendToBranches = True)
        self.qeventHub.publishEvents()
        self.assertCurrentState(course.Pipe)
        
        # Now run the normal check to ensure we end properly
        PipeTestCase.checkSettled(self, course.Bin)
        
        
class TestPipeGate(PipeTestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.PipeGate', 'ram.ai.course.Light'] } }
        PipeTestCase.setUp(self, cfg = cfg)
        self.machine.start(course.PipeGate)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.PipeGate)
        self.assert_(self.visionSystem.pipeLineDetector)
        
    def testEarlyEnter(self):
        # Stop machine, and restart as if we found the pipe early
        self.machine.stop()
        
        self.ai.data['foundPipeEarly'] = True
        self.machine.start(course.PipeGate)
        
        self.assertCurrentState(course.PipeGate)
        self.assertCurrentBranches([pipe.Seeking])
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, course.Light)
        
class TestPipeStaged(PipeTestCase):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.course.PipeStaged' : {
                        'timeout' : 37,
                        'doTimeout' : 27,
                    },
                }
            },
            'Ai' : {'taskOrder' : ['ram.ai.course.PipeStaged', 
                                   'ram.ai.course.LightStaged'] }
        }
        
        PipeTestCase.setUp(self, pipe.Start, cfg = cfg)
        self.machine.start(course.PipeStaged)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        PipeTestCase.checkStart(self, course.PipeStaged)
        #self.assert_(self.visionSystem.pipeLineDetector)
        
    def testSettled(self):
        """
        Make sure that we move onto the light once we get over the pipe
        """
        PipeTestCase.checkSettled(self, course.LightStaged)
        
    def testPipeLost(self):
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(course.PipeStaged)
        
        # Now the timer is active, make sure we don't create new ones
        timer = MockTimer.LOG[course.PipeStaged.LOST_TIMEOUT]
        self.injectEvent(vision.EventType.PIPE_LOST)
        self.assertCurrentState(course.PipeStaged)
        timer2 = MockTimer.LOG[course.PipeStaged.LOST_TIMEOUT]
        self.assertEqual(timer, timer2)

        # Release the time and make sure we move on
        self.releaseTimer(course.PipeStaged.LOST_TIMEOUT)
        self.assertCurrentState(course.LightStaged)
        #self.assertEqual(expected, self.controller.desiredOrientation)
        
    def testLostTimeout(self):
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        
        self.assertCurrentState(course.PipeStaged)
        self.injectEvent(course.PipeStaged.LOST_TIMEOUT)
        self.qeventHub.publishEvents()
        self.assertCurrentState(course.LightStaged)
        #self.assertEqual(expected, self.controller.desiredOrientation)
        
    def testPipeFound(self):
        # Grab the current running timer
        timer = MockTimer.LOG[self.machine.currentState().timeoutEvent]
        
        # Inject found event and make sure it cancels timer, starts new one
        self.injectEvent(vision.EventType.PIPE_FOUND)
        self.assert_(timer.stopped)
        self.assert_(MockTimer.LOG.has_key(course.PipeStaged.DO_TIMEOUT))
        
        # Make sure repeated events don't create new timers
        timer = MockTimer.LOG[course.PipeStaged.DO_TIMEOUT]
        self.injectEvent(vision.EventType.PIPE_FOUND)
        timer2 = MockTimer.LOG[course.PipeStaged.DO_TIMEOUT]
        self.assertEqual(timer, timer2)

        # Check the timer config
        self.assertEqual(27, timer._sleepTime)
        
        # Release the time and make sure we move on
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        self.releaseTimer(course.PipeStaged.DO_TIMEOUT)
        self.assertCurrentState(course.LightStaged)
        #self.assertEqual(expected, self.controller.desiredOrientation)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        expected = math.Quaternion(math.Degree(25), math.Vector3.UNIT_Z)
        self.ai.data['gateOrientation'] = expected
        
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.PipeStaged)
        
        # Release timer
        self.assertEqual(37, 
            MockTimer.LOG[self.machine.currentState().timeoutEvent]._sleepTime)
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.LightStaged)
        
        #self.qeventHub.publishEvents()
        #self.assertCurrentState(course.LightStaged)
        self.assertFalse(self.machine.branches.has_key(pipe.Start))
        self.assertFalse(self.visionSystem.pipeLineDetector)
        #self.assertEqual(expected, self.controller.desiredOrientation)

class PipeObjectiveTest(object):
    def setUp(self, myState, endState, motion, *motionList):
        self.myState = myState
        self.endState = endState
        self.motion = motion
        self.motionList = motionList

        # This value should be deleted by the task
        self.ai.data['pipeStartOrientation'] = 35

    def testStart(self):
        self.assertCurrentState(self.myState)

        #self.assert_(not self.ai.data.has_key('pipeStartOrientation'))

        # Make sure we've started the first motion
        self.assertCurrentMotion(self.motion)

        # We should have no branches yet
        self.assertCurrentBranches([])

    def testFinish(self):
        self.machine.stop()

        # Make sure it's cleaned up after itself
        self.assert_(not self.ai.data.has_key('pipeStartOrientation'))

    def testMultiMotion(self):
        self.assertCurrentState(self.myState)
        self.assertCurrentMotion(self.motion)

        # There should be no branches
        self.assertCurrentBranches([])

        # Now finish the first motion
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()

        # Now make sure it goes through all motions and doesn't enter
        # any branches without a timeout or found pipe
        for name in self.motionList:
            self.assertCurrentState(self.myState)
            self.assertCurrentBranches([])
            self.assertCurrentMotion(name)
            self.motionManager.currentMotion._finish()
            self.qeventHub.publishEvents()

        self.qeventHub.publishEvents()
        # When it's finished, make sure that it starts the searching branch
        self.assertCurrentState(self.myState)
        self.assertCurrentBranches([pipe.Searching])

        # Make sure another queued motions finished doesn't branch twice
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(self.myState)
        self.assertCurrentBranches([pipe.Searching])

    def testPipeFound(self):
        self.assertCurrentState(self.myState)
        self.assertCurrentMotion(self.motion)

        # There should be no branches
        self.assertCurrentBranches([])

        self.publishQueuedEvent(self.ai, vision.EventType.PIPE_FOUND,
                                vision.PipeEvent,0,0,0, id = 0)
        self.qeventHub.publishEvents()
        
        # Check if it branched into seeking
        self.assertCurrentState(self.myState)
        self.assertCurrentMotion(motion.pipe.Hover)
        self.assertCurrentBranches([pipe.Seeking])

        # Check that another pipe found doesn't try to branch again
        self.publishQueuedEvent(self.ai, vision.EventType.PIPE_FOUND,
                                vision.PipeEvent,0,0,0, id = 0)
        self.qeventHub.publishEvents()
        self.assertCurrentState(self.myState)
        self.assertCurrentMotion(motion.pipe.Hover)
        self.assertCurrentBranches([pipe.Seeking])

    def testTimeout(self):
        self.assertCurrentState(self.myState)
        self.assertCurrentMotion(self.motion)

        # There should be no branches
        self.assertCurrentBranches([])

        self.releaseTimer(course.PipeObjective.TIMEOUT)
        
        # Check if it branched into seeking
        self.assertCurrentState(self.myState)
        self.assertCurrentBranches([pipe.Searching])

class TestPipeBarbedWire(PipeObjectiveTest, support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'taskOrder' :
                    [ 'ram.ai.course.PipeBarbedWire',
                      'ram.ai.course.Target' ],
                'config' : {
                    'PipeBarbedWire' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.RateChangeDepth',
                                'desiredDepth' : 6,
                                'speed' : 0.3,
                                },
                            '2' : {
                                'type' : 'ram.motion.basic.RateChangeHeading',
                                'desiredHeading' : -30,
                                'speed' : 10,
                                'absolute' : False
                                },
                            '3' : {
                                'type' : 'ram.motion.search.ForwardZigZag',
                                'legTime' : 5,
                                'sweepAngle' : 30,
                                'speed' : 3
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        PipeObjectiveTest.setUp(self, course.PipeBarbedWire, course.Target,
                                motion.basic.RateChangeDepth,
                                motion.basic.RateChangeHeading,
                                motion.search.ForwardZigZag)
        self.machine.start(course.PipeBarbedWire)

class TestLight(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Light', 'ram.ai.course.Pipe'],
                        }
                }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Light)
        self._stateType = course.Light
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)
        
        self.assertCurrentBranches([light.Start])

        # Finish the depth change and make sure the red light detector is on
        self.injectEvent(motion.basic.MotionManager.FINISHED,
                         sendToBranches = True)

        self.assert_(self.visionSystem.redLightDetector)

        # Make sure we held the current heading
        #self.assertEqual(1, self.controller.headingHolds)
        
    def testLightHit(self):
        """
        Make sure that we move on once we hit the light
        """

        self.injectEvent(light.LIGHT_HIT, sendToBranches = True)
        self.injectEvent(light.COMPLETE, sendToBranches = True)
        self.controller.publishAtOrientation(math.Quaternion.IDENTITY)
        self.qeventHub.publishEvents();

        self.assertCurrentState(course.Pipe)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(light.Start))
        self.assertFalse(self.visionSystem.redLightDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)

        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(light.Start))
        self.assertFalse(self.visionSystem.redLightDetector)

    def testOrientationReset(self):
        """
        Tests to make sure we go back to the proper orientation after we have
        hit the light
        """

        # Setup a desired orientation
        expected = math.Quaternion(math.Degree(45), math.Vector3.UNIT_Z)
        self.controller.desiredOrientation = expected

        # Restart with so we have a set orientation
        self.machine.stop()
        self.machine.start(self._stateType)

        # Hit the light
        self.injectEvent(light.LIGHT_HIT, sendToBranches = True)
        self.injectEvent(light.COMPLETE, sendToBranches = True)
        self.controller.publishAtOrientation(math.Quaternion.IDENTITY)
        self.qeventHub.publishEvents();
        self.assertCurrentState(course.Pipe)

        # It does this as part of the Continue state now, not needed
        # Make sure we are not at the proper orientation
        #self.assertEqual(expected, self.controller.desiredOrientation)

class TestPipeTarget(PipeObjectiveTest, support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'taskOrder' :
                    [ 'ram.ai.course.PipeTarget',
                      'ram.ai.course.Bin' ],
                'config' : {
                    'PipeTarget' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.RateChangeDepth',
                                'desiredDepth' : 6,
                                'speed' : 0.3,
                                },
                            '2' : {
                                'type' : 'ram.motion.basic.RateChangeHeading',
                                'desiredHeading' : -30,
                                'speed' : 10,
                                'absolute' : False
                                },
                            '3' : {
                                'type' : 'ram.motion.search.ForwardZigZag',
                                'legTime' : 5,
                                'sweepAngle' : 30,
                                'speed' : 3
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        PipeObjectiveTest.setUp(self, course.PipeTarget, course.Bin,
                                motion.basic.RateChangeDepth,
                                motion.basic.RateChangeHeading,
                                motion.search.ForwardZigZag)
        self.machine.start(course.PipeTarget)

class TestPipeBin(PipeObjectiveTest, support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'taskOrder' :
                    [ 'ram.ai.course.PipeBin',
                      'ram.ai.course.Pinger' ],
                'config' : {
                    'PipeBin' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.RateChangeDepth',
                                'desiredDepth' : 6,
                                'speed' : 0.3,
                                },
                            '2' : {
                                'type' : 'ram.motion.basic.RateChangeHeading',
                                'desiredHeading' : -30,
                                'speed' : 10,
                                'absolute' : False
                                },
                            '3' : {
                                'type' : 'ram.motion.search.ForwardZigZag',
                                'legTime' : 5,
                                'sweepAngle' : 30,
                                'speed' : 3
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        PipeObjectiveTest.setUp(self, course.PipeBin, course.Pinger,
                                motion.basic.RateChangeDepth,
                                motion.basic.RateChangeHeading,
                                motion.search.ForwardZigZag)
        self.machine.start(course.PipeBin)
       
class TestLightStaged(TestLight):
    def setUp(self):
        cfg = {
            'StateMachine' : {
                'States' : {
                    'ram.ai.course.LightStaged' : {
                        'timeout' : 97,
                        'doTimeout' : 108,
                    },
                }
            },
            'Ai' : {'taskOrder' : ['ram.ai.course.LightStaged', 
                                   'ram.ai.course.Pipe']
            }
        }
        
        
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.LightStaged)
        self._stateType = course.LightStaged
     
    def testLightFound(self):
        # Grab the current running timer
        timer = MockTimer.LOG[self.machine.currentState().timeoutEvent]
        
        # Inject found event and make sure it cancels timer, starts new one
        self.injectEvent(vision.EventType.LIGHT_FOUND)
        self.assert_(timer.stopped)
        self.assert_(MockTimer.LOG.has_key(course.LightStaged.DO_TIMEOUT))
        
        # Make sure repeated events don't create new timers
        timer = MockTimer.LOG[course.LightStaged.DO_TIMEOUT]
        self.injectEvent(vision.EventType.LIGHT_FOUND)
        timer2 = MockTimer.LOG[course.LightStaged.DO_TIMEOUT]
        self.assertEqual(timer, timer2)

        # Check the timer config
        self.assertEqual(108, timer._sleepTime)
        
        # Release the time and make sure we move on
        self.releaseTimer(course.LightStaged.DO_TIMEOUT)
        self.assertCurrentState(course.Pipe)

class TestBuoy(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.ai.course.Buoy',
                                   'ram.ai.course.Pipe'],
                'config' : {
                    'Buoy' : {
                        'searchTimeout' : 20,
                        'lostTimeout' : 10
                    }
                }
            }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Buoy)
        self._stateType = course.Buoy

    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)

        self.assertCurrentBranches([buoy.Start])

        # Finish the depth change and check that the detector is on
        self.injectEvent(motion.basic.MotionManager.FINISHED,
                         sendToBranches = True)
        self.assert_(self.visionSystem.buoyDetector)

    def testBuoyDone(self):
        """
        Make sure that we move on once we hit the light
        """

        self.injectEvent(buoy.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(buoy.Start))
        self.assertFalse(self.visionSystem.buoyDetector)

    def testTaskTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)
        
        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(buoy.Start))
        self.assertFalse(self.visionSystem.buoyDetector)
    
    def testSearchingTimeouts(self):
        """
        Make sure that the timeouts for the Searching state work properly
        """
        # Restart
        self.machine.stop()
        self.machine.start(self._stateType)

        # Test the searchTimeout timeout

        # Stop the task timeout timer
        taskTimer = MockTimer.LOG[self.machine.currentState().timeoutEvent]
        taskTimer.stop()

        self.injectEvent(buoy.Searching.BUOY_SEARCHING, sendToBranches = True)

        # Release the searchTimeout
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(buoy.Start))
        self.assertFalse(self.visionSystem.buoyDetector)

        
        # Restart to test the lostTimeout timeout
        self.machine.stop()
        self.machine.start(self._stateType)
        
        # Stop the task timeout timer
        taskTimer = MockTimer.LOG[self.machine.currentState().timeoutEvent]
        taskTimer.stop()

        self.injectEvent(vision.EventType.BUOY_FOUND, sendToBranches = True)
        self.injectEvent(buoy.Searching.BUOY_SEARCHING, sendToBranches = True)

        # Stop searchTimeout timer
        self.machine.currentState()._searchTimeout.stop()

        # Release the lostTimeout
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(buoy.Start))
        self.assertFalse(self.visionSystem.buoyDetector)

class TestBarbedWire(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.BarbedWire',
                         'ram.ai.course.PipeBarbedWire'],
                        }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.BarbedWire)
        self._stateType = course.BarbedWire
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)

        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentBranches([barbedwire.Start])

        # Finish the depth change and check that the detector is on
        self.injectEvent(motion.basic.MotionManager.FINISHED,
                         sendToBranches = True)
        self.assert_(self.visionSystem.barbedWireDetector)
        
    def testBarbedWireDone(self):
        """
        Make sure that we move on once we hit the light
        """

        self.injectEvent(barbedwire.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.PipeBarbedWire)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(barbedwire.Start))
        self.assertFalse(self.visionSystem.barbedWireDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)
        
        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.PipeBarbedWire)
        self.assertFalse(self.machine.branches.has_key(barbedwire.Start))
        self.assertFalse(self.visionSystem.barbedWireDetector)

class TestHedge(support.AITestCase):
    def setUp(self):
        cfg = {
            'Ai' : {'taskOrder' : ['ram.ai.course.Hedge',
                                   'ram.ai.course.Pipe']
                    }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Hedge)
        self._stateType = course.Hedge

    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)

        self.assertCurrentBranches([hedge.Start])

        # Finish the depth change and check that the detector is on
        self.injectEvent(motion.basic.MotionManager.FINISHED,
                         sendToBranches = True)
        self.assert_(self.visionSystem.hedgeDetector)

    def testHedgeDone(self):
        """
        Make sure that we move on once we hit the light
        """

        self.injectEvent(hedge.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(hedge.Start))
        self.assertFalse(self.visionSystem.hedgeDetector)

    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)
        
        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(hedge.Start))
        self.assertFalse(self.visionSystem.hedgeDetector)
        
class TestTarget(support.AITestCase):
    def setUp(self):
        cfg = { 
            'Ai' : {'taskOrder' : 
                    ['ram.ai.course.Target', 'ram.ai.course.PipeTarget'],
                    }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Target)
        self._stateType = course.Target
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)
        
        self.assertCurrentBranches([target.Start])

        #self.assert_(self.visionSystem.targetDetector)
        
    def testTargetDone(self):
        """
        Make sure that we go to the next task once we finish the target
        """

        # Make sure we are still in the same state
        self.injectEvent(target.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.PipeTarget)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)

        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.PipeTarget)
        self.assertFalse(self.machine.branches.has_key(target.Start))
        self.assertFalse(self.visionSystem.targetDetector)

class TestWindow(support.AITestCase):
    def setUp(self):
        cfg = { 
            'Ai' : {'taskOrder' : 
                    ['ram.ai.course.Window', 'ram.ai.course.Pipe'],
                    }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Window)
        self._stateType = course.Window
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(self._stateType)
        
        self.assertCurrentBranches([window.Start])

        #self.assert_(self.visionSystem.windowDetector)
        
    def testWindowDone(self):
        """
        Make sure that we go to the next task once we finish the window
        """

        # Make sure we are still in the same state
        self.injectEvent(window.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(self._stateType)

        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(window.Start))
        self.assertFalse(self.visionSystem.windowDetector)
        
class TestBin(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Bin', 'ram.ai.course.Pipe'],
                        'config' : {
                            'Bin' : {
                                'heading' : 10
                                }
                            }
                        }
        }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Bin)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Bin)

        self.assertCurrentBranches([bin.Start])
        #self.assert_(self.visionSystem.binDetector)
        
    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """

        # Check that it continues to the next task
        self.injectEvent(bin.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Pipe)
        
        # Make sure the light seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(bin.Start))
        self.assertFalse(self.visionSystem.binDetector)
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.Bin)

        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Pipe)
        self.assertFalse(self.machine.branches.has_key(bin.Start))
        self.assertFalse(self.visionSystem.binDetector)

class TestPinger(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : 
                        ['ram.ai.course.Pinger', 'ram.ai.course.SafeDive'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Pinger)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.Pinger)
        self.assertCurrentBranches([sonar.Start])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        
        self.injectEvent(sonar.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.SafeDive)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Start))

class TestSafeDive(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : ['ram.ai.course.SafeDive', 
                                       'ram.ai.course.SafeVision'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.estimator.depth = 0
        self.machine.start(course.SafeDive)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(
            (motion.pipe.Hover, motion.basic.RateChangeDepth,
             motion.pipe.Hover))
        
        self.assertCurrentState(course.SafeDive)
        self.assertCurrentBranches([sonar.Hovering])
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)
        self.assertCurrentState(course.SafeVision)
        
        # Make sure the pinger seeking branch is gone
        self.assertFalse(self.machine.branches.has_key(sonar.Hovering))
        
class TestSafeVision(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : ['ram.ai.course.SafeVision', 
                                       'ram.ai.course.Octagon'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.estimator.depth = 5
        self.machine.start(course.SafeVision)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.SafeVision)
        self.assertCurrentBranches([safe.Searching])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Octagon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(safe.Searching))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.SafeVision)
        
        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Octagon)
        self.assertFalse(self.machine.branches.has_key(safe.Searching))

class TestSafeSonar(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : ['ram.ai.course.SafeSonar', 
                                       'ram.ai.course.Octagon'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.estimator.depth = 5
        self.machine.start(course.SafeSonar)
        
    def testStart(self):
        """
        Make sure that when we start we are doing the right thing
        """
        self.assertCurrentState(course.SafeSonar)
        self.assertCurrentBranches([sonarSafe.Settling])

    def testComplete(self):
        """
        Make sure that we move on once we hit the light
        """
        self.injectEvent(safe.COMPLETE, sendToBranches = True)
        self.assertCurrentState(course.Octagon)
        
        # Make sure the safe grabbing branch is gone
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Settling))
        
    def testTimeout(self):
        """
        Make sure that the timeout works properly
        """
        # Restart with a working timer
        self.machine.stop()
        self.machine.start(course.SafeSonar)
        
        # Release timer
        self.releaseTimer(self.machine.currentState().timeoutEvent)
        
        # Test that the timeout worked properly
        self.assertCurrentState(course.Octagon)
        self.assertFalse(self.machine.branches.has_key(sonarSafe.Dive))

class TestOctagon(support.AITestCase):
    def setUp(self):
        cfg = { 'Ai' : {'taskOrder' : ['ram.ai.course.Octagon'] } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.estimator.depth = 5
        self.machine.start(course.Octagon)
    
    def testStart(self):
        """Make sure we start diving"""
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentState(course.Octagon)
                
    def testDiveFinished(self):
        self.injectEvent(motion.basic.MotionManager.FINISHED)

    def testRelease(self):
        # Finish the motion to start the timer
        self.injectEvent(motion.basic.MotionManager.FINISHED)

        # Release the timer
        self.releaseTimer(course.Octagon.SURFACED)
        self.assert_(self.vehicle.grabberReleased)
        self.assert_(self.machine.complete)

    def testExit(self):
        """
        Test an early exit to make sure it doesn't crash
        """
        self.machine.stop()

class TestTimedTravel(support.AITestCase):
    def setUp(self):
        # Config file with the heading put in so it doesn't skip over the
        # RateChangeHeading
        cfg = { 'Ai' : {'taskOrder' : ['ram.ai.course.TimedTravel'],
                        'config' : {'TimedTravel' : {'heading' : 45} } } }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.TimedTravel)

    def testStart(self):
        """
        Run through the motions and make sure that
        it doesn't exit too early
        """
        self.assertCurrentMotionList([motion.basic.RateChangeDepth,
                                      motion.basic.RateChangeHeading,
                                      motion.basic.TimedMoveDirection])

        # Depth motion
        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        
        self.assertCurrentState(course.TimedTravel)
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()

        # Heading motion
        self.assertCurrentMotion(motion.basic.RateChangeHeading)

        self.assertCurrentState(course.TimedTravel)
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()

        # Forward motion
        self.assertCurrentMotion(motion.basic.TimedMoveDirection)

        self.assertCurrentState(course.TimedTravel)
        self.motionManager.currentMotion._finish()
        self.qeventHub.publishEvents()

        self.assert_(self.machine.complete)

class TestTravel(support.AITestCase):
    def setUp(self):
        # Allow each individual test to make their own config file
        pass

    def testSingleMotion(self):
        """
        Test a single motion in the config file
        """
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.ai.course.Travel'],
                'config' : {
                    'Travel' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.RateChangeHeading',
                                'desiredHeading' : 45,
                                'speed' : 10
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Travel)

        self.assertCurrentMotion(motion.basic.RateChangeHeading)
        self.assertCurrentState(course.Travel)
        self.motionManager.currentMotion._finish()

        self.qeventHub.publishEvents()

        self.assertCurrentMotion(type(None))
        self.assertCurrentState(type(None))

    def testMultiMotion(self):
        """
        Test multiple motions in the config file
        """
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.ai.course.Travel'],
                'config' : {
                    'Travel' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.TimedMoveDirection',
                                'desiredHeading' : 45,
                                'speed' : 3,
                                'duration' : 5
                                },
                            '2' : {
                                'type' : 'ram.motion.basic.RateChangeDepth',
                                'desiredDepth' : 5,
                                'speed' : 1.0/3.0
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Travel)

        self.assertCurrentMotion(motion.basic.TimedMoveDirection)
        self.assertCurrentState(course.Travel)
        self.motionManager.currentMotion._finish()

        self.assertCurrentMotion(motion.basic.RateChangeDepth)
        self.assertCurrentState(course.Travel)
        self.motionManager.currentMotion._finish()

        self.qeventHub.publishEvents()

        self.assertCurrentMotion(type(None))
        self.assertCurrentState(type(None))

    def testNoMotion(self):
        """
        Test no motions in the config file
        """
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.ai.course.Travel']
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        self.machine.start(course.Travel)
        self.qeventHub.publishEvents()
        self.assertCurrentState(type(None))

    def testIncompleteMotion(self):
        """
        Tests that no incomplete motions are allowed (even if they're
        the last motion)
        """
        cfg = {
            'Ai' : {
                'taskOrder' : ['ram.ai.course.Travel'],
                'config' : {
                    'Travel' : {
                        'motions' : {
                            '1' : {
                                'type' : 'ram.motion.basic.MoveDirection',
                                'desiredHeading' : 45,
                                'speed' : 3
                                }
                            }
                        }
                    }
                }
            }
        support.AITestCase.setUp(self, cfg = cfg)
        exception = False
        try:
            # This should throw an exception because MoveDirection is incomplete
            self.machine.start(course.Travel)
        except Exception, e:
            exception = True
        self.assert_(exception)
