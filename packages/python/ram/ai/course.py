# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/gate.py

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
import ram.ai.barbedwire as barbedwire
import ram.ai.target as target
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

class PipeObjective(task.Task, pipe.PipeTrackingState):
    """
    Search for the pipe while doing a multi motion
    """
    
    TIMEOUT = core.declareEventType('TIMEOUT_')

    @staticmethod
    def _transitions(myState):
        trans = pipe.PipeTrackingState.transitions(myState)

        trans.update({ motion.basic.MotionManager.
                       QUEUED_MOTIONS_FINISHED : myState,
                       pipe.PipeTrackingState.FOUND_PIPE : myState,
                       PipeObjective.TIMEOUT : myState,
                       pipe.Centering.SETTLED : task.Next })

        return trans

    def QUEUED_MOTIONS_FINISHED(self, event):
        if not self._branched:
            self.stateMachine.start(state.Branch(pipe.Searching))
            self._branched = True
            if self.timer is not None:
                self.timer.stop()

    def FOUND_PIPE(self, event):
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
        pipe.PipeTrackingState.enter(self)
        task.Task.enter(self)

        self.visionSystem.pipeLineDetectorOn()

        self._branched = False
        self._className = type(self).__name__
        self.ai.data['pipeBiasDirection'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'biasDirection', None)
        self.ai.data['pipeThreshold'] = \
            self.ai.data['config'].get(self._className, {}).get(
                    'threshold', None)

        timeout = self.ai.data['config'].get(self._className, {}).get(
            'timeout', 30)

        self.timer = self.timerManager.newTimer(PipeObjective.TIMEOUT, timeout)
        self.timer.start()

        self._motion = motion
        self._motionList = motionList
        self.motionManager.setQueuedMotions(motion, *motionList)

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
        return { motion.basic.Motion.FINISHED : thisState,
                 light.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(light.Start) }

    def FINISHED(self, event):
        # Branch after the first finished motion only
        if self._first:
            self.stateMachine.start(state.Branch(light.Start, 
                                                 branchingEvent = event))
            self._first = False

    def enter(self, defaultTimeout = 90):
        self._className = type(self).__name__
        timeout = self.ai.data['config'].get(self._className, {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        # Save current orientation
        #self.controller.holdCurrentHeading()
        #self._initialOrientation = self.controller.getDesiredOrientation()

        # Turn the vehicle if needed
        self._heading = self.ai.data['config'].get(self._className, {}).get(
            'heading', 0)
        self._speed = self.ai.data['config'].get(self._className, {}).get(
            'speed', 10)
        self._absolute = self.ai.data['config'].get(self._className, {}).get(
            'absolute', False)

        self._first = True
        self._headingChange = motion.basic.RateChangeHeading(
            desiredHeading = self._heading, speed = self._speed,
            absolute = self._absolute)
        self.motionManager.setMotion(self._headingChange)
    
    def exit(self):
        task.Task.exit(self)
        if not self._first:
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
    
class BarbedWire(task.Task):
    """
    Task for completion of the BarbedWire objective within a certain timelimit.
    """
    @staticmethod
    def _transitions():
        return { motion.basic.Motion.FINISHED : BarbedWire,
                 barbedwire.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(barbedwire.Start) }
    
    def FINISHED(self, event):
        # Branch after the first finished motion only
        if self._first:
            self.stateMachine.start(state.Branch(barbedwire.Start,
                                                 branchingEvent = event))
            self._first = False

    def enter(self, defaultTimeout = 120):
        timeout = self.ai.data['config'].get('BarbedWire', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self._heading = self.ai.data['config'].get('BarbedWire', {}).get(
            'heading', 0)
        self._speed = self.ai.data['config'].get('BarbedWire', {}).get(
            'speed', 10)
        self._absolute = self.ai.data['config'].get('BarbedWire', {}).get(
            'absolute', False)

        self._first = True
        self._headingChange = motion.basic.RateChangeHeading(
            desiredHeading = self._heading, speed = self._speed,
            absolute = self._absolute)
        self.motionManager.setMotion(self._headingChange)
    
    def exit(self):
        task.Task.exit(self)
        if not self._first:
            self.stateMachine.stopBranch(barbedwire.Start)
        self.visionSystem.barbedWireDetectorOff()
        self.motionManager.stopCurrentMotion()
    
class Target(task.Task):
    """
    Task for completion of the BarbedWire objective within a certain timelimit.
    """
    
    MOVE_ON = core.declareEventType('MOVE_ON')
    
    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.QUEUED_MOTIONS_FINISHED : Target,
                 task.TIMEOUT : task.Next,
                 target.COMPLETE : task.Next,
                 'GO' : state.Branch(target.Start) }

    def QUEUED_MOTIONS_FINISHED(self, event):
        # Branch after the first finished motion only
        if self._first:
            self.stateMachine.start(state.Branch(target.Start,
                                                 branchingEvent = event))
            self._first = False
    
    def enter(self, defaultTimeout = 120):
        # Initialize task part of class
        timeout = self.ai.data['config'].get('Target', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)
        
        self._heading = self.ai.data['config'].get('Target', {}).get(
            'heading', 0)
        self._speed = self.ai.data['config'].get('Target', {}).get(
            'speed', 10)
        self._absolute = self.ai.data['config'].get('Target', {}).get(
            'absolute', False)
        self._duration = self.ai.data['config'].get('Target', {}).get(
            'duration', 0)

        self._first = True
        self._headingChange = motion.basic.RateChangeHeading(
            desiredHeading = self._heading, speed = self._speed,
            absolute = self._absolute)
        self._forward = motion.basic.TimedMoveDirection(
            desiredHeading = 0, speed = 3, duration = self._duration,
            absolute = False)
        self.motionManager.setQueuedMotions(self._headingChange, self._forward)
    
    def exit(self):
        task.Task.exit(self)
        if not self._first:
            self.stateMachine.stopBranch(target.Start)
        self.visionSystem.targetDetectorOff()
        self.motionManager.stopCurrentMotion()
    
class Bin(task.Task):
    @staticmethod
    def _transitions():
        return { motion.basic.MotionManager.QUEUED_MOTIONS_FINISHED : Bin,
                 bin.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next }

    def COMPLETE(self, event):
        self.ai.data['binComplete'] = True

    def QUEUED_MOTIONS_FINISHED(self, event):
        # Branch after the first finished motion only
        if self._first:
            self.stateMachine.start(state.Branch(bin.Start,
                                                 branchingEvent = event))
            self._first = False

    def enter(self, defaultTimeout = 240):
        timeout = self.ai.data['config'].get('Bin', {}).get(
                    'taskTimeout', defaultTimeout)
        task.Task.enter(self, defaultTimeout = timeout)

        self._heading = self.ai.data['config'].get('Bin', {}).get(
            'heading', 0)
        self._speed = self.ai.data['config'].get('Bin', {}).get(
            'speed', 10)
        self._absolute = self.ai.data['config'].get('Bin', {}).get(
            'absolute', False)
        self._duration = self.ai.data['config'].get('Bin', {}).get(
            'duration', 0)

        self._first = True
        self._headingChange = motion.basic.RateChangeHeading(
            desiredHeading = self._heading, speed = self._speed,
            absolute = self._absolute)
        self._depthChange = motion.basic.RateChangeDepth(
            desiredDepth = 4.5, speed = (1.0/3.0))
        self._forward = motion.basic.TimedMoveDirection(
            desiredHeading = 0, speed = 3, duration = self._duration,
            absolute = False)
        self.motionManager.setQueuedMotions(self._headingChange,
                                            self._depthChange, self._forward)

        self.timer = None
        self._failure = False
    
    def exit(self):
        task.Task.exit(self)
        if not self._first:
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
        return { motion.basic.Motion.FINISHED : task.Next,
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
    @staticmethod
    def _transitions():
        return { motion.basic.Motion.FINISHED : task.Next }

    @staticmethod
    def getattr():
        return set(['depth', 'diveSpeed']).union(task.Task.getattr())

    def enter(self):
        task.Task.enter(self)
        # Start our dive
        diveMotion = motion.basic.RateChangeDepth(
            desiredDepth = self._config.get('depth', 0),
            speed = self._config.get('diveSpeed', 0.3))
        
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        task.Task.exit(self)
        self.motionManager.stopCurrentMotion()

class RecoverFromSafe(task.Task):
    """
    Gets us back in the octagon if the safe fails for some reason.
    
    Dives to good pinger depth, and activates the sonar
    """
    pass
