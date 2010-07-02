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
import ram.ai.bin as bin
import ram.ai.randombin as randombin
import ram.ai.safe as safe
import ram.ai.sonarSafe as sonarSafe
import ram.ai.sonar as sonar

import ram.motion as motion
import ram.motion.basic

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
        return set(['pipeDelay']).union(task.Task.getattr())

    def PIPE_FOUND(self, event):
        self.ai.data['foundPipeEarly'] = True

    def PIPE_ON_(self, event):
        """Turn pipe detector on after a delay"""
        self.visionSystem.pipeLineDetectorOn()

    def enter(self):
        task.Task.enter(self)
        
        self.ai.data['foundPipeEarly'] = False
        self.exited = False
        
        # Branch of state machine for gate
        self.stateMachine.start(state.Branch(gate.Start))
        
        # Save current heading
        self.ai.data['gateOrientation'] = \
            self.controller.getDesiredOrientation()
        
        # Setup timer to trigger pipe detector after a certain delay
        delay = self._config.get('pipeDelay', 30)
        self.timer = self.timerManager.newTimer(Gate.PIPE_ON, delay)
        self.timer.start()
        
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
    
    def enter(self, defaultTimeout = 10):
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
        
        task.Task.exit(self)
        self.stateMachine.stopBranch(pipe.Start)
        self.visionSystem.pipeLineDetectorOff()
        
    @property
    def pipesToFind(self):
        return self._pipesToFind
    
class PipeGate(task.Task):
    """
    Find and hover over the first pipe in the course
    """
    
    @staticmethod
    def _transitions():
        return { pipe.Centering.SETTLED : task.Next,
                 'GATE' : state.Branch(pipe.Searching),
                 'PIPE' : state.Branch(pipe.Seeking) }
    
    def enter(self):
        task.Task.enter(self)
        
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

    def enter(self, motion, *motionList):
        # Cleanup pipeStartOrientation if it exists in the ai data
        if self.ai.data.has_key('pipeStartOrientation'):
            del self.ai.data['pipeStartOrientation']

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
            'timeout', 10)
        self.timer = self.timerManager.newTimer(PipeObjective.TIMEOUT, timeout)
        self.timer.start()

        self._motion = motion
        self._motionList = motionList
        self.motionManager.setMotion(motion, *motionList)

    def exit(self):
        del self.ai.data['pipeBiasDirection']
        del self.ai.data['pipeThreshold']

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

    def enter(self):
        self._rotation = self.ai.data['config'].get('PipeBarbedWire', {}).get(
                'rotation', -30)
        self._legTime = self.ai.data['config'].get('PipeBarbedWire', {}).get(
            'legTime', 5)
        self._sweepAngle = self.ai.data['config'].get('PipeBarbedWire', {}).get(
            'sweepAngle', 30)
        self._sweepSpeed = self.ai.data['config'].get('PipeBarbedWire', {}).get(
            'sweepSpeed', 3)

        m1 = motion.basic.RateChangeDepth(
            self.ai.data['config'].get('pipeDepth', 6), 0.3)
        m2 = motion.basic.RateChangeHeading(
            self._rotation, 10, absolute = False)
        m3 = motion.search.ForwardZigZag(self._legTime, self._sweepAngle,
                                         self._sweepSpeed)

        PipeObjective.enter(self, m1, m2, m3)

class PipeTarget(PipeObjective):
    @staticmethod
    def _transitions():
        return PipeObjective._transitions(PipeTarget)

    def enter(self):
        self._rotation = self.ai.data['config'].get('PipeTarget', {}).get(
                'rotation', -30)
        self._legTime = self.ai.data['config'].get('PipeTarget', {}).get(
            'legTime', 5)
        self._sweepAngle = self.ai.data['config'].get('PipeTarget', {}).get(
            'sweepAngle', 30)
        self._sweepSpeed = self.ai.data['config'].get('PipeTarget', {}).get(
            'sweepSpeed', 3)
        self._absolute = self.ai.data['config'].get('PipeTarget', {}).get(
            'absolute', False)

        m1 = motion.basic.RateChangeDepth(
            self.ai.data['config'].get('pipeDepth', 6), 0.3)
        m2 = motion.basic.RateChangeHeading(
            self._rotation, 10, absolute = self._absolute)
        m3 = motion.search.ForwardZigZag(self._legTime, self._sweepAngle,
                                         self._sweepSpeed)

        PipeObjective.enter(self, m1, m2, m3)

class PipeBin(PipeObjective):
    @staticmethod
    def _transitions():
        return PipeObjective._transitions(PipeBin)

    def enter(self):
        self._rotation = self.ai.data['config'].get('PipeBin', {}).get(
                'rotation', -30)
        self._legTime = self.ai.data['config'].get('PipeBin', {}).get(
            'legTime', 5)
        self._sweepAngle = self.ai.data['config'].get('PipeBin', {}).get(
            'sweepAngle', 30)
        self._sweepSpeed = self.ai.data['config'].get('PipeBin', {}).get(
            'sweepSpeed', 3)

        m1 = motion.basic.RateChangeDepth(
            self.ai.data['config'].get('pipeDepth', 6), 0.3)
        m2 = motion.basic.RateChangeHeading(
            self._rotation, 10, absolute = False)
        m3 = motion.search.ForwardZigZag(self._legTime, self._sweepAngle,
                                         self._sweepSpeed)

        PipeObjective.enter(self, m1, m2, m3)

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

class Buoy(task.Task):
    """
    Task for completion of the Buoy objective within a certain time limit.
    """
    @staticmethod
    def _transitions():
        return { buoy.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(buoy.Start) }

    def enter(self, defaultTimeout = 90):
        timeout = self.ai.data['config'].get('Buoy', {}).get(
            'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(buoy.Start))

    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(buoy.Start)
        self.visionSystem.buoyDetectorOff()
        self.motionManager.stopCurrentMotion()
    
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
                 window.COMPLETE : task.Next,
                 'GO' : state.Branch(window.Start) }
    
    def enter(self, defaultTimeout = 120):
        # Initialize task part of class
        timeout = self.ai.data['config'].get('Window', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

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
        timeout = self.ai.data['config'].get('Bin', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self.stateMachine.start(state.Branch(bin.Start))
    
    def exit(self):
        task.Task.exit(self)

        self.stateMachine.stopBranch(bin.Start)
        self.visionSystem.binDetectorOff()
        self.motionManager.stopCurrentMotion()

class RandomBin(task.Task):

    MOVE_ON = core.declareEventType('MOVE_ON')

    @staticmethod
    def _transitions():
        return { bin.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 RandomBin.MOVE_ON : task.Next,
                 'GO' : state.Branch(bin.Start) }

    def enter(self, defaultTimeout = 60):
        timeout = self.ai.data['config'].get('RandomBin', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        if self.ai.data.get('binComplete', False):
            self.publish(RandomBin.MOVE_ON, core.Event())
        else:
            self.stateMachine.start(state.Branch(randombin.Start))
    
    def exit(self):
        task.Task.exit(self)
        if not self.ai.data.get('binComplete', False):
            self.stateMachine.stopBranch(randombin.Start)
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

class Octagon(task.Task):
    """
    Surface in the octagon with or without the treasure, but with the sonar on
    """
    SURFACED = core.declareEventType('SURFACED_')

    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.FINISHED : Octagon,
                 Octagon.SURFACED : task.Next }

    @staticmethod
    def getattr():
        return set(['depth', 'diveSpeed', 'delay', 'release']).union(
            task.Task.getattr())

    def FINISHED(self, event):
        """
        Waits for the vehicle to balance out on the top of the water
        before releasing the grabber.
        """
        # Safe the thrusters (that way we float to the absolute top)
        self.vehicle.safeThrusters()
        self._timer = self.timerManager.newTimer(Octagon.SURFACED, self._delay)
        self._timer.start()

    def SURFACED_(self, event):
        """
        Releases the grabber.
        """
        if self._release:
            self.vehicle.releaseGrabber()

    def enter(self):
        task.Task.enter(self)

        self._delay = self._config.get('delay', 5)
        self._release = self._config.get('release', True)

        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 0),
            speed = self._config.get('diveSpeed', 0.3))
        
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
