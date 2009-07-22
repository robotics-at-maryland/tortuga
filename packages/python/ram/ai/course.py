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
        task.Task.enter(self, defaultTimeout = defaultTimeout)
        
        self._pipesToFind = self._config.get('pipesToFind', 1)
        self._pipeCount = 0
        self._className = type(self).__name__
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
    
    TIMEOUT = core.declareEventType('TIMEOUT')

    @staticmethod
    def _transitions(myState):
        trans = pipe.PipeTrackingState.transitions(myState)

        trans.update({ motion.basic.MotionManager.
                       QUEUED_MOTIONS_FINISHED : state.Branch(pipe.Searching),
                       pipe.PipeTrackingState.
                       FOUND_PIPE : state.Branch(pipe.Seeking),
                       PipeObjective.TIMEOUT : state.Branch(pipe.Searching),
                       pipe.Centering.SETTLED : task.Next })

        return trans

    def FOUND_PIPE(self, event):
        self.motionManager.stopCurrentMotion()
        self._foundPipe = True

    def enter(self, motion, *motionList):
        pipe.PipeTrackingState.enter(self)
        task.Task.enter(self)

        self.visionSystem.pipeLineDetectorOn()

        self._foundPipe = False
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
        task.Task.enter(self, defaultTimeout = defaultTimeout)

        # Save current orientation
        self.controller.holdCurrentHeading()
        self._initialOrientation = self.controller.getDesiredOrientation()
        
        self.stateMachine.start(state.Branch(light.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(light.Start)
        self.visionSystem.redLightDetectorOff()
    
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
        return { barbedwire.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(barbedwire.Start) }
    
    def enter(self, defaultTimeout = 120):
        task.Task.enter(self, defaultTimeout = defaultTimeout)
        self.stateMachine.start(state.Branch(barbedwire.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(barbedwire.Start)
        self.visionSystem.barbedWireDetectorOff()
    
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
    
    def enter(self, defaultTimeout = 180):
        # Initialize task part of class
        task.Task.enter(self, defaultTimeout = defaultTimeout)
        
        # Start the sub state
        self.stateMachine.start(state.Branch(target.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(target.Start)
        self.visionSystem.targetDetectorOff()
    
class Bin(task.Task):
    @staticmethod
    def _transitions():
        return { bin.COMPLETE : task.Next,
                 task.TIMEOUT : task.Next,
                 'GO' : state.Branch(bin.Start) }

    def enter(self):
        task.Task.enter(self, defaultTimeout = 600)
        self.stateMachine.start(state.Branch(bin.Start))
    
    def exit(self):
        task.Task.exit(self)
        self.stateMachine.stopBranch(bin.Start)
        self.visionSystem.binDetectorOff()
        
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
    
    def enter(self):
        # TODO: base the timeout off an offset from how much time we have left
        # in the mission
        task.Task.enter(self, defaultTimeout = 500)
        
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
