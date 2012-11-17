# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/course.py

"""
A state machine which controls the vehicle through the course. Currently it
assumes the vehicle can find all the pipes, but allows missing objectives other
then the pipes.
 
 
Requires the following subsystems:
 - 
"""

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.control as control
from ext.control import yawVehicleHelper
import ext.math as math

import ram.ai.task as task
import ram.ai.state as state
import ram.ai.gate as gate
import ram.ai.pipe as pipe
import ram.ai.light as light
import ram.ai.buoy as buoy
import ram.ai.barbedwire as barbedwire
import ram.ai.hedge as hedge
import ram.ai.target as target
import ram.ai.window as window
import ram.ai.statBin as bin
#import ram.ai.bin as bin
import ram.ai.safe as safe
import ram.ai.sonarSafe as sonarSafe
import ram.ai.sonar as sonar
import ram.ai.vase as vase
import ram.ai.lane as lane
import ram.ai.cupid as cupid
import ram.ai.OldBuoy as oldBuoy
import ram.ai.uprights as uprights
import ram.ai.grapes as grapes

import ram.motion as motion
import ram.motion.basic
from ram.motion.basic import Frame

class Gate(task.Task):
    """
    This State overseas the completion of the gate objective.  It turns on the 
    pipe detector after a certain delay, so it can catch the pipe as it drives
    through the gate if needed.
    """
    
    PIPE_ON = core.declareEventType('PIPE_ON_')
    
    @staticmethod
    def _transitions():
        return { gate.COMPLETE : task.Next,
                 vision.EventType.PIPE_FOUND : task.Next,
                 Gate.PIPE_ON : Gate,
                 'GO' : state.Branch(gate.Start) }

    @staticmethod
    def getattr():
        return set(['pipeDelay', 'gateOrientation']).union(task.Task.getattr())

    def PIPE_FOUND(self, event):
        self.ai.data['foundPipeEarly'] = True

    def PIPE_ON_(self, event):
        """Turn pipe detector on after a delay"""
        #self.visionSystem.pipeLineDetectorOn()
        return

    def enter(self):
        task.Task.enter(self)
        
        self.ai.data['foundPipeEarly'] = False
        self.exited = False
        
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Start))

        self.ai.data['gateOrientation'] = \
            self._config.get('gateOrientation', 0)
        
        if self.ai.data['gateOrientation'] == 0:
            # Save current heading
            self.ai.data['gateOrientation'] = \
                self.stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()

        self.ai.data['fakeGate'] = False
        
        # Setup timer to trigger pipe detector after a certain delay
        delay = self._config.get('pipeDelay', 30)
        self.timer = self.timerManager.newTimer(Gate.PIPE_ON, delay)
        self.timer.start()
        
    def exit(self):
        task.Task.exit(self)
        
        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Start)):
            self.stateMachine.stopBranch(gate.Start)
    
class FakeGate(task.Task):
    """
    A secondary gate task for if we need to easily dive and go in a direction
    """
    
    @staticmethod
    def _transitions():
        return { gate.COMPLETE : task.Next,
                 'GO' : state.Branch(gate.Start) }

    @staticmethod
    def getattr():
        return set(['pipeDelay', 'gateOrientation']).union(task.Task.getattr())

    def enter(self):
        task.Task.enter(self)
        
        self.exited = False
        
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Start))

        self.ai.data['gateOrientation'] = \
            self._config.get('gateOrientation', 0)

        self.ai.data['fakeGateDistance'] = \
            self._config.get('fakeGateDistance', 5)
        
        self.ai.data['fakeGate'] = True
        
        if self.ai.data['gateOrientation'] == 0:
            # Save current heading
            self.ai.data['gateOrientation'] = \
                self.stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()

    def exit(self):
        task.Task.exit(self)
        
        self.exited = True
        if (self.stateMachine.branches.has_key(gate.Start)):
            self.stateMachine.stopBranch(gate.Start)
    
class Pipe(task.Task):
    """
    Find and hover a pipe in the course
    """
    
    COMPLETE = core.declareEventType('COMPLETE')
    
    @staticmethod
    def _transitions(currentState = None):
        if currentState is None:
            currentState = Pipe
        return { pipe.Centering.SETTLED : currentState,
                 Pipe.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                'GO' : state.Branch(pipe.Start) }

    @staticmethod
    def getattr():
        return set(['pipesToFind']).union(task.Task.getattr())
        
    def SETTLED(self, event):
        """
        Sends a complete event after enough pipes have been found
        """
        
        # We found a pipe increment the count
        self._pipeCount += 1
        
        if self._pipeCount >= self._pipesToFind:
            # We found enough pipes move on
            self.publish(Pipe.COMPLETE, core.Event())
    
    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self._pipesToFind = self._config.get('pipesToFind', 1)
        self._pipeCount = 0
        self.ai.data['pipeBiasDirection'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'biasDirection', None)
        self.ai.data['pipeThreshold'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'threshold', None)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(pipe.Start))
        
    def exit(self):
        del self.ai.data['pipeBiasDirection']
        del self.ai.data['pipeThreshold']
        if self.ai.data.has_key('pipeStartOrientation'):
            del self.ai.data['pipeStartOrientation']
        
        task.Task.exit(self)
        self.stateMachine.stopBranch(pipe.Start)
        self.visionSystem.pipeLineDetectorOff()
        
    @property
    def pipesToFind(self):
        return self._pipesToFind

class Cupid(task.Task):
    """
    Finds and fires through through the Cupid
    """
    
    @staticmethod
    def _transitions():
        return { lane.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }

    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(cupid.Start))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(cupid.Start)
    

class LoversLane(task.Task):
    """
    Finds and goes through the Lovers Lane
    """
    
    @staticmethod
    def _transitions():
        return { lane.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }
        
    
    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self.ai.data['laneOrientation'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'orientation', 0)
        self.ai.data['laneDepth'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'depth', 5)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(lane.Start))
        
    def exit(self):
        del self.ai.data['laneDepth']
        del self.ai.data['laneOrientation']
        task.Task.exit(self)
        self.stateMachine.stopBranch(lane.Start)
    
class LoversLane1(LoversLane):
    pass
    
class LoversLane2(LoversLane):
    pass
    
class LoversLane3(LoversLane):
    pass
    
class PipeGate(task.Task):
    """
    Find and hover over the first pipe in the course
    """
    
    @staticmethod
    def _transitions():
        return { pipe.Centering.SETTLED : task.Next,
                 task.TIMEOUT : task.Next,
                 'GATE' : state.Branch(pipe.Searching),
                 'PIPE' : state.Branch(pipe.Seeking) }
    
    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self.ai.data['pipeBiasDirection'] = \
            self.ai.data['config'].get('PipeGate', {}).get(
                    'biasDirection', None)
        self.ai.data['pipeThreshold'] = \
            self.ai.data['config'].get('PipeGate', {}).get(
                    'threshold', None)
        
        # Branch off state machine for finding the pipe
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.start(state.Branch(pipe.Seeking))
        else:
            self.stateMachine.start(state.Branch(pipe.Searching))
        
    def exit(self):
        task.Task.exit(self)
        
        if self.ai.data.get('foundPipeEarly', False):
            self.stateMachine.stopBranch(pipe.Seeking)
        else:
            self.stateMachine.stopBranch(pipe.Searching)

        self.visionSystem.pipeLineDetectorOff()

class StagedTemplate(task.Task):
    """
    A template for any task. It has three steps, each
    with separate timeouts.

    1. This will timeout after a certain amount of time
    if the objective is not found.
    2. This will timeout if taking too long to complete
    the objective.
    3. This one activates when the object was found,
    has been lost, and timed out while searching for it.
    """

    # Generic timeout
    TIMEOUT = core.declareEventType('TIMEOUT')

    # Specific timeout types
    EARLY_TIMEOUT = core.declareEventType('EARLY_TIMEOUT')
    NORMAL_TIMEOUT = core.declareEventType('NORMAL_TIMEOUT')
    LOST_TIMEOUT = core.declareEventType('LOST_TIMEOUT')

    # Acts as an enum
    SEARCHING, TASK, LOST = range(3)

    """
    @type foundEvent: Event
    
    @type lostEvent: Event
    """
    @staticmethod
    def _transitions(myState, foundEvent, lostEvent):
        # These will lead to different types of failure
        # when the state machine itself is modified
        return { StagedTemplate.EARLY_TIMEOUT : task.Failure,
                 StagedTemplate.NORMAL_TIMEOUT : task.Failure,
                 StagedTemplate.LOST_TIMEOUT : task.Failure,
                 foundEvent : myState,
                 lostEvent : myState }

    def TIMEOUT(self, event):
        # Figure out what state it was in when it timed out
        if self._state == StagedTemplate.SEARCHING:
            self.publish(StagedTemplate.EARLY_TIMEOUT, core.Event())
        elif self._state == StagedTemplate.LOST:
            self.publish(StagedTemplate.LOST_TIMEOUT, core.Event())
        else:
            # Normal timeout is the default if an improper state was set
            self.publish(StagedTemplate.NORMAL_TIMEOUT, core.Event())

    def enter(self, searchingTimeout, taskTimeout, lostTimeout):
        self._state = StagedTemplate.SEARCHING
        self._timer = self.timerManager.newTimer(
            StagedTemplate.TIMEOUT, searchingTimeout)

        self._taskTimeout = taskTimeout
        self._lostTimeout = lostTimeout

    def exit(self):
        # If the timer still exists, stop it
        if self._timer is not None:
            self._timer.stop()

class PipeObjective(task.Task, pipe.PipeTrackingState):
    """
    Search for the pipe while doing a multi motion
    """
    
    TIMEOUT = core.declareEventType('TIMEOUT_')

    @staticmethod
    def _transitions(myState):
        trans = pipe.PipeTrackingState.transitions(myState)

        trans.update({ motion.basic.MotionManager.FINISHED : myState,
                       vision.EventType.PIPE_FOUND : myState,
                       PipeObjective.TIMEOUT : myState,
                       task.TIMEOUT : task.Next,
                       pipe.Centering.SETTLED : task.Next })

        return trans

    def FINISHED(self, event):
        if not self._branched:
            self.stateMachine.start(state.Branch(pipe.Searching))
            self._branched = True
            if self.timer is not None:
                self.timer.stop()

    def PIPE_FOUND(self, event):
        if not self._branched:
            self.motionManager.stopCurrentMotion()
            self.stateMachine.start(state.Branch(pipe.Seeking))
            self._branched = True
            if self.timer is not None:
                self.timer.stop()

    def TIMEOUT_(self, event):
        if not self._branched:
            self.motionManager.stopCurrentMotion()
            self.stateMachine.start(state.Branch(pipe.Searching))
            self._branched = True

    def enter(self):
        pipe.PipeTrackingState.enter(self)

        self.visionSystem.pipeLineDetectorOn()

        self._branched = False
        self._className = type(self).__name__
        self.ai.data['pipeBiasDirection'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'biasDirection', None)
        self.ai.data['pipeThreshold'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'threshold', None)

        taskTimeout = self.ai.data['config'].get(self._className, {}).get(
            'taskTimeout', 30)
        task.Task.enter(self, taskTimeout)

        timeout = self.ai.data['config'].get(self._className, {}).get(
            'motionTimeout', 10)
        self.timer = self.timerManager.newTimer(PipeObjective.TIMEOUT, timeout)
        self.timer.start()
        
        generateMotionList = motion.basic.MotionManager.generateMotionList
        self._motionList = self.ai.data['config'].get(self._className, {}).get(
            'motions', {})
        motions = generateMotionList(self._motionList)

        if len(motions) > 0:
            self.motionManager.setMotion(*motions)

    def exit(self):
        # Cleanup global variables
        del self.ai.data['pipeBiasDirection']
        del self.ai.data['pipeThreshold']
        if self.ai.data.has_key('pipeStartOrientation'):
            del self.ai.data['pipeStartOrientation']

        task.Task.exit(self)

        if self.timer is not None:
            self.timer.stop()

        if self.stateMachine.branches.has_key(pipe.Seeking):
            self.stateMachine.stopBranch(pipe.Seeking)
        elif self.stateMachine.branches.has_key(pipe.Searching):
            self.stateMachine.stopBranch(pipe.Searching)

        self.visionSystem.pipeLineDetectorOff()

class PipeBarbedWire(PipeObjective):
    @staticmethod
    def _transitions():
        return PipeObjective._transitions(PipeBarbedWire)

class PipeTarget(PipeObjective):
    @staticmethod
    def _transitions():
        return PipeObjective._transitions(PipeTarget)

class PipeBin(PipeObjective):
    @staticmethod
    def _transitions():
        return PipeObjective._transitions(PipeBin)

class PipeStaged(Pipe):
    """
    Find and hover over the second pipe in the course
    """
    DO_TIMEOUT = core.declareEventType('DO_TIMEOUT_')
    MOVE_ON = core.declareEventType('MOVE_ON')
    LOST_TIMEOUT = core.declareEventType('LOST_TIMEOUT_')
    
    @staticmethod
    def _transitions():
        trans = Pipe._transitions(PipeStaged)
        trans.update({ vision.EventType.PIPE_LOST : PipeStaged,
                 vision.EventType.PIPE_FOUND : PipeStaged,
                 task.TIMEOUT : PipeStaged,
                 PipeStaged.DO_TIMEOUT : PipeStaged,
                 PipeStaged.LOST_TIMEOUT : PipeStaged,
                 PipeStaged.MOVE_ON : task.Next })
        return trans

    @staticmethod
    def getattr():
        return set(['lostTimeout', 'doTimeout']).union(task.Task.getattr())
        
    def PIPE_LOST(self, event):
        if self.lostTimer is None:
            timeout = self._config.get('lostTimeout', 2)
            self.lostTimer = \
                self.timerManager.newTimer(PipeStaged.LOST_TIMEOUT, timeout)
            self.lostTimer.start()
        
    def PIPE_FOUND(self, event):
        # Stop old
        self._timer.stop()
        
        if self.doTimer is None:
            timeout = self._config.get('doTimeout', 20)
            self.doTimer = \
                self.timerManager.newTimer(PipeStaged.DO_TIMEOUT, timeout)
            self.doTimer.start()
        
        # Remove the lost timer if needed
        if self.lostTimer is not None:
            self.lostTimer.stop()
            self.lostTimer = None
        
    def LOST_TIMEOUT_(self, event):
        self._moveOn()
        
    def TIMEOUT_PIPESTAGED(self, event):
        self._moveOn()
        
    def DO_TIMEOUT_(self, event):

        self._moveOn()
        
    def _moveOn(self):
        # Go back to the gates orientation
        #gateOrientation = \
        #    self.ai.data['gateOrientation'].getYaw().valueDegrees()
        #self._rotationMotion = motion.basic.RateChangeHeading(
        #    desiredHeading = gateOrientation, speed = 10, absolute = False)

        #self.motionManager.setMotion(self._rotationMotion)
        self.controller.setDesiredOrientation(self.ai.data['gateOrientation'])
        
        # Move on to the Light hitting task
        self.publish(PipeStaged.MOVE_ON, core.Event())
    
    def enter(self):
        Pipe.enter(self, defaultTimeout = 20)
        
        # Set default value do timer, so we only create it once
        self.doTimer = None
        self.lostTimer = None
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(pipe.Start)
        self.visionSystem.pipeLineDetectorOff()
       
class Pipe1(Pipe):
    @staticmethod
    def _transitions():
        return Pipe._transitions(Pipe1)

class Pipe2(Pipe):
    @staticmethod
    def _transitions():
        return Pipe._transitions(Pipe2)

class Pipe3(Pipe):
    @staticmethod
    def _transitions():
        return Pipe._transitions(Pipe3)
        
class Light(task.Task):
    """
    Task for completion of the light objective within a certain timelimit. It
    also remembers its initial orientation and returns to that orientation 
    before moving onto the next state.
    """

    @staticmethod
    def _transitions(thisState = None):
        if thisState is None:
            thisState = Light
        return { light.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(light.Start) }

    def enter(self, defaultTimeout = 90):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(light.Start))
    
    def exit(self):
        task.Task.exit(self)
        
        self.stateMachine.stopBranch(light.Start)
        self.visionSystem.redLightDetectorOff()
        self.motionManager.stopCurrentMotion()
    
class LightStaged(Light):
    """
    Does the light task in such with intermediate timers, that attempt to quit
    early if the light is lost.
    """
    DO_TIMEOUT = core.declareEventType('DO_TIMEOUT_')
    
    @staticmethod
    def _transitions():
        trans = Light._transitions(LightStaged)
        trans.update({
            vision.EventType.LIGHT_FOUND : LightStaged,
            LightStaged.DO_TIMEOUT : task.Next })
        return trans

    @staticmethod
    def getattr():
        return set(['doTimeout']).union(task.Task.getattr())
    
    def LIGHT_FOUND(self, event):
        # Stop old
        self._timer.stop()
        
        if self.doTimer is None:
            timeout = self._config.get('doTimeout', 25)
            self.doTimer = self.timerManager.newTimer(LightStaged.DO_TIMEOUT, 
                                                      timeout)
            self.doTimer.start()
            
    def enter(self):
        Light.enter(self, defaultTimeout = 40)
        
        # Set time to none
        self.doTimer = None

class OldBuoy(task.Task):
    """
    Task for completion of the Buoy objective within a certain time limit.
    """

    @staticmethod
    def _transitions():
        return { oldBuoy.COMPLETE : task.Next,
                 oldBuoy.Searching.BUOY_SEARCHING : OldBuoy,
                 vision.EventType.BUOY_FOUND : OldBuoy,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(oldBuoy.Start) }

    def BUOY_SEARCHING(self, event):
        # This is defensive, it should never happen
        if self._lostTimeout is not None:
            self._lostTimeout.stop()

        if self._searchTimeout is None:
            self._searchTimeout = self.timerManager.newTimer(
                self._timeoutEvent, self._searchDelay)
            self._searchTimeout.start()

        if self._buoyFound:
            # We should not continue searching for too long
            self._lostTimeout = self.timerManager.newTimer(
                self._timeoutEvent, self._lostDelay)
            self._lostTimeout.start()

    def BUOY_FOUND(self, event):
        # Stop the lost timeout if we find a buoy
        # Set it so we are in the vicinity of the buoys
        if self._lostTimeout is not None:
            self._lostTimeout.stop()

        if self._searchTimeout is not None:
            self._searchTimeout.stop()

        self._buoyFound = True

    def enter(self, defaultTimeout = 90):
        timeout = self.ai.data['config'].get('Buoy', {}).get(
            'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self._lostDelay = self.ai.data['config'].get('Buoy', {}).get(
            'lostTimeout', 5)
        self._lostTimeout = None
        self._searchDelay = self.ai.data['config'].get('Buoy', {}).get(
            'searchTimeout', 10)
        self._searchTimeout = None
        self._buoyFound = False

        self.stateMachine.start(state.Branch(oldBuoy.Start))

    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(oldBuoy.Start)
        self.visionSystem.buoyDetectorOff()
        self.motionManager.stopCurrentMotion()

        if self._lostTimeout is not None:
            self._lostTimeout.stop()
        if self._searchTimeout is not None:
            self._searchTimeout.stop()
    
class Buoy(task.Task):
    """
    Task for completion of the Buoy objective within a certain time limit.
    """

    @staticmethod
    def _transitions():
        return { buoy.COMPLETE : task.Next,
                 vision.EventType.BUOY_FOUND : Buoy,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(buoy.Start) }

    def BUOY_SEARCHING(self, event):
        # This is defensive, it should never happen
        if self._lostTimeout is not None:
            self._lostTimeout.stop()

        if self._searchTimeout is None:
            self._searchTimeout = self.timerManager.newTimer(
                self._timeoutEvent, self._searchDelay)
            self._searchTimeout.start()

        if self._buoyFound:
            # We should not continue searching for too long
            self._lostTimeout = self.timerManager.newTimer(
                self._timeoutEvent, self._lostDelay)
            self._lostTimeout.start()

    def BUOY_FOUND(self, event):
        # Stop the lost timeout if we find a buoy
        # Set it so we are in the vicinity of the buoys
        if self._lostTimeout is not None:
            self._lostTimeout.stop()

        if self._searchTimeout is not None:
            self._searchTimeout.stop()

        self._buoyFound = True

    def enter(self, defaultTimeout = 90):
        timeout = self.ai.data['config'].get('Buoy', {}).get(
            'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        # get the list of buoys we want to hit
        self.ai.data['buoyList'] = \
            self.ai.data['config'].get('targetBuoys', [])
        if len(self.ai.data['buoyList']) == 0:
            raise LookupError, "No buoys specified"
        
        self._lostDelay = self.ai.data['config'].get('Buoy', {}).get(
            'lostTimeout', 5)
        self._lostTimeout = None
        self._searchDelay = self.ai.data['config'].get('Buoy', {}).get(
            'searchTimeout', 10)
        self._searchTimeout = None
        self._buoyFound = False

        self.stateMachine.start(state.Branch(buoy.Start))

    def exit(self):
        task.Task.exit(self)

        del self.ai.data['buoyList']

        self.stateMachine.stopBranch(buoy.Start)
        self.visionSystem.buoyDetectorOff()
        self.motionManager.stopCurrentMotion()

        if self._lostTimeout is not None:
            self._lostTimeout.stop()
        if self._searchTimeout is not None:
            self._searchTimeout.stop()
    
class BarbedWire(task.Task):
    """
    Task for completion of the BarbedWire objective within a certain timelimit.
    """
    @staticmethod
    def _transitions():
        return { barbedwire.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(barbedwire.Start) }

    def enter(self, defaultTimeout = 120):
        timeout = self.ai.data['config'].get('BarbedWire', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(barbedwire.Start))
    
    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(barbedwire.Start)
        self.visionSystem.barbedWireDetectorOff()
        self.motionManager.stopCurrentMotion()

class Hedge(task.Task):
    """
    Task for completion of the Hedge objective within a certain time limit
    """
    @staticmethod
    def _transitions():
        return { hedge.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(hedge.Start) }

    def enter(self, defaultTimeout = 120):
        timeout = self.ai.data['config'].get('Hedge', {}).get(
            'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(hedge.Start))

    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(hedge.Start)
        self.visionSystem.hedgeDetectorOff()
        self.motionManager.stopCurrentMotion()

class Hedge2(Hedge):
    @staticmethod
    def _transitions():
        return Hedge._transitions()

class Hedge3(Hedge):
    @staticmethod
    def _transitions():
        return Hedge._transitions()
    
class Target(task.Task):
    """
    Task for completion of the BarbedWire objective within a certain timelimit.
    """
    
    MOVE_ON = core.declareEventType('MOVE_ON')
    
    @staticmethod
    def _transitions():
        return { task.TIMEOUT : task.Next,
                 target.COMPLETE : task.Next,
                 'GO' : state.Branch(target.Start) }
    
    def enter(self, defaultTimeout = 120):
        # Initialize task part of class
        timeout = self.ai.data['config'].get('Target', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(target.Start))
    
    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(target.Start)
        self.visionSystem.targetDetectorOff()
        self.motionManager.stopCurrentMotion()

class Window(task.Task):
    """
    Task for completion of the BarbedWire objective within a certain timelimit.
    """
    
    MOVE_ON = core.declareEventType('MOVE_ON')
    
    @staticmethod
    def _transitions():
        return { task.TIMEOUT : task.Next,
                 #vision.EventType.WINDOW_FOUND : Window,
                 #window.Searching.WINDOW_SEARCHING : Window,
                 window.COMPLETE : task.Next,
                 'GO' : state.Branch(window.Start) }

    def WINDOW_FOUND(self, event):
        if self._lostTimeout is not None:
            self._lostTimeout.stop()

        self._windowFound = True

    def TIMEOUT_WINDOW(self, event):
        pass
    
    def enter(self, defaultTimeout = 120):
        # Initialize task part of class
        timeout = self.ai.data['config'].get('Window', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self._lostDelay = self.ai.data['config'].get('Window', {}).get(
            'lostTimeout', 5)
        self._lostTimeout = None
        self._windowFound = False

        self.stateMachine.start(state.Branch(window.Start))
    
    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(window.Start)
        self.visionSystem.windowDetectorOff()
        self.motionManager.stopCurrentMotion()
    
class Bin(task.Task):
    @staticmethod
    def _transitions():
        return { bin.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(bin.Start) }

    def COMPLETE(self, event):
        self.ai.data['binComplete'] = True

    def enter(self, defaultTimeout = 240):

        self._className = type(self).__name__
        self.ai.data['binOrientation'] = self.ai.data['config'].get(self._className, {}).get('orientation', 0)

        # get the list of buoys we want to hit
        self.ai.data['symbolList'] = \
            self.ai.data['config'].get('targetSymbols', [])
        if len(self.ai.data['symbolList']) == 0:
            raise LookupError, "No symbols specified"

#         if self.ai.data['buoyOrientation'] is None:
#             raise LookupError, "No orientation specified"

        timeout = self.ai.data['config'].get('Bin', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(bin.Start))
    
    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(bin.Start)
        self.visionSystem.binDetectorOff()
        self.motionManager.stopCurrentMotion()

        
class Pinger(task.Task):
    """
    Move until we are over the pinger
    """
    @staticmethod
    def _transitions():
        return {  sonar.COMPLETE : task.Next,
                 'GO' : state.Branch(sonar.Start) }
    
    def enter(self):
        task.Task.enter(self)
        # Branch off state machine for finding the sonar
        self.stateMachine.start(state.Branch(sonar.Start))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(sonar.Start)
        
class Pinger2(task.Task):
    """
    Move until we are over the pinger
    """
    @staticmethod
    def _transitions():
        return {  sonar.COMPLETE : task.Next,
                 'GO' : state.Branch(sonar.Start) }
    
    def enter(self):
        task.Task.enter(self)
        # Branch off state machine for finding the sonar
        self.stateMachine.start(state.Branch(sonar.Start))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(sonar.Start)

class SafeDive(task.Task):
    """
    Dive to a depth at which we can activate the safe vision system.  The sonar
    system is active during this time.
    """
    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : task.Next,
                 'GO' : state.Branch(sonar.Hovering) } 

    @staticmethod
    def getattr():
        return set(['depth', 'diveSpeed']).union(task.Task.getattr())

    def enter(self):
        task.Task.enter(self)
        # Activate the sonar system
        self.stateMachine.start(state.Branch(sonar.Hovering))
        
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 10),
            speed = self._config.get('diveSpeed', 0.3))
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()
        self.stateMachine.stopBranch(sonar.Hovering)

class SafeVision(task.Task):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
        
    @staticmethod
    def _transitions():
        return { safe.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(safe.Searching) }
    
    def enter(self):
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        task.Task.enter(self, defaultTimeout = 60)
        self.stateMachine.start(state.Branch(safe.Searching))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(safe.Searching)
        self.visionSystem.downwardSafeDetectorOff()

class SafeSonar(task.Task):
    """
    Grabs the safe and surfaces to a predefined depth, when it times out, it
    goes into a recovering mode, and does a normal surface.
    """
    
    @staticmethod
    def _transitions():
        return { safe.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(sonarSafe.Settling) }
    
    def enter(self, defaultTimeout = 500):
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        timeout = self.ai.data['config'].get('SafeSonar', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self.stateMachine.start(state.Branch(sonarSafe.Settling))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(sonarSafe.Settling)

class Vase(task.Task):
    """
    Dives to the depth of the Vase, grabs it and surfaces to a predefined
    depth
    """

    @staticmethod
    def _transitions():
        return { vase.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }
    
    @staticmethod
    def getattr():
        return set(['vaseOrientation']).union(task.Task.getattr())

    def enter(self, defaultTimeout = 500):
        timeout = self.ai.data['config'].get('Vase', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.ai.data['vaseOrientation'] = \
            self._config.get('vaseOrientation', 0)
        
        if self.ai.data['vaseOrientation'] == 0:
            # Save current heading
            self.ai.data['vaseOrientation'] = \
                self.stateEstimator.getEstimatedOrientation().getYaw().valueDegrees()
        
        self.stateMachine.start(state.Branch(vase.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(vase.Start)

class Octagon(task.Task):
    """
    Surface in the octagon with or without the treasure, but with the sonar on
    """
    SURFACED = core.declareEventType('SURFACED_')

    @staticmethod
    def _transitions():
        return { Octagon.SURFACED : task.Next }

    @staticmethod
    def getattr():
        return set(['depth', 'diveSpeed', 'delay', 'release']).union(
            task.Task.getattr())

    def SURFACED_(self, event):
        """
        Releases the grabber.
        """
        if self._release:
            print 'releasing'
            self.vehicle.releaseGrabber()
        

    def enter(self):
        task.Task.enter(self)

        self._delay = self._config.get('delay', 25)
        self._release = self._config.get('release', True)


        self._timer = self.timerManager.newTimer(Octagon.SURFACED, self._delay)
        self._timer.start()

        # Start our dive
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._config.get('depth', 0),
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._config.get('diveSpeed', 0.3))

        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)

        self.motionManager.setMotion(diveMotion)

    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()

        if self._timer is not None:
            self._timer.stop()


class SimpleDive(task.Task):
    """
    Dive to a specified depth and drops the object
    """
    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : task.Next }

    @staticmethod
    def getattr():
        return set(['depth', 'diveSpeed', 'release']).union(
            task.Task.getattr())

    def FINISHED(self, event):
        """
        Releases the grabber.
        """
        if self._release:
            print 'releasing'
            self.vehicle.releaseGrabber()
        

    def enter(self):
        task.Task.enter(self)

        self._depth = self._config.get('depth', 6)
        self._release = self._config.get('release', True)

        # Start our dive
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._config.get('diveSpeed', 0.3))

        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)

        self.motionManager.setMotion(diveMotion)

    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()

        if self._timer is not None:
            self._timer.stop()

class RecoverFromSafe(task.Task):
    """
    Gets us back in the octagon if the safe fails for some reason.
    
    Dives to good pinger depth, and activates the sonar
    """
    pass

class Travel(task.Task):
    """
    Loads the motion from the config and does it.
    """

    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : task.Next,
                 Travel.FINISHED : task.Next }

    def enter(self):
        self._className = type(self).__name__

        taskTimeout = self.ai.data['config'].get(self._className, {}).get(
            'taskTimeout', 30)
        task.Task.enter(self, defaultTimeout = taskTimeout)

        motions = self.ai.data['config'].get(self._className, {}).get(
            'motions', {})
        motionList = self.motionManager.generateMotionList(motions,
                                                           strict = True)

        if len(motionList) > 0:
            self.motionManager.setMotion(*motionList)
        else:
            self.publish(Travel.FINISHED, core.Event())

class Travel1(Travel):
    pass

class Travel2(Travel):
    pass

class Travel3(Travel):
    pass

class TravelPoint(task.Task):
    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : task.Next}

    def enter(self):
        self._className = type(self).__name__

        taskTimeout = self.ai.data['config'].get(self._className, {}).get(
            'taskTimeout', 30)
        Xpos = self.ai.data['config'].get(self._className, {}).get(
            'X', 0)
        Ypos = self.ai.data['config'].get(self._className, {}).get(
            'Y', 0)
        yaw = self.ai.data['config'].get(self._className, {}).get(
            'orientation', 0)
        speed = self.ai.data['config'].get(self._className, {}).get(
            'speed', 0.2)

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, yaw),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(Ypos,Xpos),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = speed)

        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        self.motionManager.setMotion(yawMotion, translateMotion)

class TravelPoint1(TravelPoint):
    pass

class TravelPoint2(TravelPoint):
    pass

class TravelPoint3(TravelPoint):
    pass

class TimedTravel(task.Task):
    """
    A pre-determined move that changes depth, changes heading, then moves
    forward for a pre-determined amount of time. It dynamically loads its
    class name.

    A RateChangeDepth -> RateChangeHeading -> TimedMoveDirection.
    """

    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : task.Next }

    @staticmethod
    def getattr():
        return set(['depth', 'heading', 'speed', 'duration',
                    'turnSpeed', 'depthSpeed']).union(
            task.Task.getattr())

    def enter(self, *motions):
        task.Task.enter(self)

        # Dynamically load the name of the class
        self._className = type(self).__name__

        # Load all of the config values from the ai file (so they are easily
        # changed)
        self._depth = self.ai.data['config'].get(self._className, {}).get(
            'depth', 3)
        self._depthSpeed = self.ai.data['config'].get(self._className, {}).get(
            'depthSpeed', (1.0/3.0))
        self._heading = self.ai.data['config'].get(self._className, {}).get(
            'heading', 0)
        self._headingSpeed = self.ai.data['config'].get(self._className,
                                                        {}).get(
            'turnSpeed', 10)
        self._speed = self.ai.data['config'].get(self._className, {}).get(
            'speed', 3)
        self._duration = self.ai.data['config'].get(self._className, {}).get(
            'duration', 5)

        # Create the motions
        self._depthMotion = motion.basic.RateChangeDepth(self._depth,
                                                         self._depthSpeed)
        self._headingMotion = motion.basic.RateChangeHeading(self._heading,
                                                             self._headingSpeed)
        self._forwardMotion = motion.basic.TimedMoveDirection(self._heading,
                                                              self._speed,
                                                              self._duration)

        # Set the motion and the queued motions
        self.motionManager.setMotion(self._depthMotion, self._headingMotion,
                                     self._forwardMotion)

    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()

class Uprights(task.Task):
    """
    Finds the uprights and swims through them.
    """
    
    @staticmethod
    def _transitions():
        return { uprights.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }

    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(uprights.Start))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(uprights.Start)

class Grapes(task.Task):
    """
    Pushes the grapes off of the stands using the Derpy thruster.
    """
    
    @staticmethod
    def _transitions():
        return { grapes.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }

    def enter(self, defaultTimeout = 60):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        # Branch off state machine for finding the pipe
        self.stateMachine.start(state.Branch(grapes.Start))
        
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(grapes.Start)
