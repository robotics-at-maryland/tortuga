# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/pipe.py


"""
A state machine for following the pipeline
 - Searches for a pipe
 - Centers over the first pipe, and waits to settle
 - Drives forward until it doesn't see the pipe
 - Goes back to the first step
 
Requires the following subsystems:
 - timerManager - ram.timer.TimerManager
 - motionManager - ram.motion.MotionManager
 - controller - ext.control.IController
"""

# Python imports
import math

# Project Imports
import ext.core as core
import ext.vision as vision
from ext.control import yawVehicleHelper
import ext.math

import ram.ai.state as state
import ram.ai.tracking as tracking
import ram.motion as motion
import ram.motion.search
import ram.motion.pipe
import ram.timer
from ram.motion.basic import Frame

def ensurePipeTracking(qeventHub, ai):        
    tracking.ensureItemTracking(qeventHub, ai, 'pipeData',
                                vision.EventType.PIPE_FOUND,
                                vision.EventType.PIPE_DROPPED)

class PipeInfo(object):
    """
    A class to hold information about the pipe for tracking it
    """
    def __init__(self, x, y, relativeAngle):
        self._x = x
        self._y = y
        self._angle = relativeAngle
        
    def getAngle(self):
        return self._angle
    
    def getX(self):
        return self._x
    
    def getY(self):
        return self._y

    def setState(self, x, y, relativeAngle):
        self._x = x
        self._y = y
        self._angle = relativeAngle

class PipeTrackingState(state.State):
    """
    A more generic version of PipeFollowingState that filters the PIPE_FOUND
    input to ignore all pipes that are outside of the threshold.
    """
    
    @staticmethod
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = PipeTrackingState
        if trans is None:
            trans = {}

        newTrans = {vision.EventType.PIPE_FOUND : myState}
        newTrans.update(trans)
        return newTrans
    
    def PIPE_FOUND(self, event):
        """
        Check if the found pipe should be tracked. If it shouldn't, ignore it.
        This only does something if a threshold has been set.
        """
        pipeData = self.ai.data['pipeData']
        angle = event.angle
        
        # Find absolute vehicle direction
        vehicleOrientation = self.stateEstimator.getEstimatedOrientation()
        vehicleDirection = vehicleOrientation.getYaw(True)

        # Determine the absolute pipe direction
        absPipeDirection = ext.math.Degree(vehicleDirection + angle)
        
        # Store the absolute pipe direction in the itemData
        pipeData.setdefault('absoluteDirection', {})[event.id] = \
            absPipeDirection
        
        if self._threshold is not None:
            if self._biasDirection is not None:
                # If outside of the threshold, return without continuing
                if not (absPipeDirection - self._threshold <= \
                            self._biasDirection <= \
                            absPipeDirection + self._threshold):
                    # Now check to make sure the pipe isn't the wrong direction
                    # TODO: Make it look pretty. This is a mess.
                    if ((absPipeDirection - self._threshold <= \
                                 self._biasDirection - ext.math.Degree(180) <= \
                                 absPipeDirection + self._threshold) or
                            (absPipeDirection - self._threshold <= \
                                 self._biasDirection + ext.math.Degree(180) <= \
                                 absPipeDirection + self._threshold)):
                            return True
                else:
                    return True
            else: # If there isn't a biasDirection, raise an error
                raise Exception("A threshold is set with no biasDirection")
        else:
            # If a currentID exists or there is no threshold set
            return True
        
        return False
            
    def enter(self):
        # Ensure pipe tracking
        ensurePipeTracking(self.queuedEventHub, self.ai)
        
        self._pipe = PipeInfo(0, 0, 0)
        
        self._biasDirection = self.ai.data.get('pipeBiasDirection', None)
        self._threshold = self.ai.data.get('pipeThreshold', None)
        if self._biasDirection is not None:
            self._biasDirection = ext.math.Degree(self._biasDirection)
        if self._threshold is not None:
            self._threshold = ext.math.Degree(self._threshold)
                
    def exit(self):
        self.motionManager.stopCurrentMotion()

    def _cleanupAbsoluteDirection(self, event):
        """Remove the stored absolute direction"""
        pipeData = self.ai.data['pipeData']
        absDirection = pipeData.get('absoluteDirection', None)
        if absDirection is not None:
            if absDirection.has_key(event.id):
                del absDirection[event.id]

    def _currentPipe(self, event):
        return self.ai.data['pipeData'].get('currentID', 0) == event.id

class PipeFollowingState(PipeTrackingState):
    """
    State for following a pipe. Returns True if the FOUND_PIPE it is given
    is valid. False if the FOUND_PIPE was outdated.
    """
    @staticmethod
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = PipeFollowingState
        if trans is None:
            trans = {}
        trans.update({vision.EventType.PIPE_LOST : FindAttempt,
                      vision.EventType.PIPE_DROPPED : myState,
                      vision.EventType.PIPE_FOUND: myState})
        
        return PipeTrackingState.transitions(myState, trans)

    @staticmethod
    def getattr():
        return set(['speedGain', 'dSpeedGain', 'iSpeedGain',
                    'sidewaysSpeedGain', 'iSidewaysSpeedGain',
                    'dSidewaysSpeedGain', 'yawGain', 'forwardSpeed', 'maxSidewaysSpeed'])

    def PIPE_LOST(self, event):
        """
        We were just told all current pipes are lost that means we can't have
        a currentID, so lets drop it if we do have one
        """

        # Remove currentID if needed
        pipeData = self.ai.data['pipeData']
        if pipeData.has_key('currentID'):
            del pipeData['currentID']

        # Remove absolute direction if needed
        if pipeData.has_key('absoluteDirection'):
            del pipeData['absoluteDirection']
    
    def PIPE_DROPPED(self, event):
        """Update the tracking system when a pipe is lost. If the pipe
           dropped is the current pipe, find the closest pipe to the
           vehicles orientation and switch to it."""

        # Declare to make it easier to read
        pipeData = self.ai.data['pipeData']

        self._cleanupAbsoluteDirection(event)

        # Check if the pipe dropped is the current pipe, and change to the
        # next best pipe if it is.
        if self._currentPipe(event):
            # The current pipe has been dropped, find the best new match
            # Do this by checking the absolute directions of each pipe and
            # comparing them to the vehicles

            # Find absolute vehicle direction
            vehicleOrientation = self.stateEstimator.getEstimatedOrientation()
            vehicleDirection = vehicleOrientation.getYaw(True)
            
            # Find the id with the absolute direction closest to the
            # vehicle direction
            minID = None
            for i in pipeData['currentIds']:
                if i is not pipeData['currentID']:
                    if minID is None:
                        minID = i
                    else:
                        if pipeData['absoluteDirection'][i] \
                                < pipeData['absoluteDirection'][minID]:
                            minID = i
            if minID is None:
                if pipeData.has_key('currentID'):
                    del pipeData['currentID']
            else:
                pipeData['currentID'] = minID
                self._pipe.setState(pipeData['itemData'][minID].x,
                                    pipeData['itemData'][minID].y,
                                    pipeData['itemData'][minID].angle)

    def PIPE_FOUND(self, event):
        """Update the state of the light, this moves the vehicle"""
        pipeData = self.ai.data['pipeData']
        angle = event.angle
        
        goodPipe = PipeTrackingState.PIPE_FOUND(self, event)
        
        # If it is not vetoed
        if goodPipe is False:
            return False

        # Find absolute vehicle direction
        vehicleOrientation = self.stateEstimator.getEstimatedOrientation()
        vehicleDirection = vehicleOrientation.getYaw(True)
        
        # Get the currentID
        pipeData.setdefault('currentID', event.id)

        if self._currentPipe(event):
            self.ai.data['lastPipeEvent'] = event

        # Check if this pipe exists
        if not pipeData['itemData'].has_key(pipeData['currentID']):
            # If it doesn't, set the currentID to event.id
            pipeData['currentID'] = event.id

        # Only do work if we are biasing the direction
        if self._biasDirection is not None:
            # If the pipe event is not the currently followed pipe
            if not self._currentPipe(event):
                # If the new pipe is closer to the biasDirection, switch
                currentPipeDifference = \
                    ext.math.Degree(self._biasDirection) - \
                    pipeData['absoluteDirection'][pipeData['currentID']]
                newPipeDifference = ext.math.Degree(self._biasDirection) - \
                    pipeData['absoluteDirection'][event.id]

                if math.fabs(newPipeDifference.valueDegrees()) < \
                        math.fabs(currentPipeDifference.valueDegrees()):
                    pipeData['currentID'] = event.id
                    self._pipe.setState(event.x, event.y, angle)
            else: # Otherwise continue normally
                # Check difference between actual and "biasDirection"
                difference = self._biasDirection - \
                    pipeData['absoluteDirection'][event.id]
                    
                if math.fabs(difference.valueDegrees()) > 90:
                    # We are pointing the wrong direction, so lets switch
                    # it around
                    if angle.valueDegrees() < 0:
                        angle = ext.math.Degree(180) + angle
                    else:
                        angle = ext.math.Degree(-180) + angle
                self._pipe.setState(event.x, event.y, angle)
        
        else: # If we are not biasing the direction
            # If the pipe event is not the currently followed pipe
            # And if the new pipe is closer to our current direction
            if not self._currentPipe(event):
                    # If the new pipe is closer to our current direction, switch
                    if math.fabs(angle.valueDegrees()) < \
                            math.fabs(pipeData['itemData'][pipeData['currentID']]
                                      .angle.valueDegrees()):
                        pipeData['currentID'] = event.id
                        self._pipe.setState(event.x, event.y, angle)
            else: # Otherwise continue normally
                self._pipe.setState(event.x, event.y)

    def enter(self):
        PipeTrackingState.enter(self)
        
        self._pipe = PipeInfo(0, 0, 0)

        speedGain = self._config.get('speedGain', 7)
        dSpeedGain = self._config.get('dSpeedGain', 1)
        iSpeedGain = self._config.get('iSpeedGain', 0.1)
        
        sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 3)
        iSidewaysSpeedGain = self._config.get('iSidewaysSpeedGain', 0.05)
        dSidewaysSpeedGain = self._config.get('dSidewaysSpeedGain', 0.5)
        
        yawGain = self._config.get('yawGain', 0.3)
        maxSpeed = self._config.get('forwardSpeed', 5)
        maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 3)
        
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, currentOrientation.getYaw().valueDegrees() + self._pipe.getAngle()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = yawGain)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = ext.math.Vector2(self._pipe.getX() * sidewaysSpeedGain,
                                          self._pipe.getY() * speedGain),
            initialRate = self.stateEstimator.getEstimatedVelocity())

        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)

        self.motionManager.setMotion(yawMotion)
        self.motionManager.setMotion(translateMotion)
        
class Start(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['speed', 'offset', 'minimumDepth'])
    
    def enter(self):
        orientation = self.stateEstimator.getEstimatedOrientation()
        self.ai.data['pipeStartOrientation'] = \
            orientation.getYaw().valueDegrees()

        offset = self._config.get('offset', 0)
        desiredDepth = self.ai.data['config'].get('pipeDepth', 6)

        # Calculate the real depth and check to that it's lower than the minimum
        desiredDepth -= offset
        minimumDepth = self._config.get('minimumDepth', 0.5)
        if desiredDepth < minimumDepth:
            desiredDepth = minimumDepth

        # Go to 5 feet
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = desiredDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._config.get('speed', 1.0/3.0))
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class FindAttempt(state.FindAttempt, PipeTrackingState):

    @staticmethod
    def transitions(foundState = None):
        if foundState is None:
            foundState = Seeking
        return state.FindAttempt.transitions(
            vision.EventType.PIPE_FOUND, foundState, Searching)

    @staticmethod
    def getattr():
        return set(['speed'])

    def enter(self):
        PipeTrackingState.enter(self)
        state.FindAttempt.enter(self)

        self.visionSystem.pipeLineDetectorOn()

        event = self.ai.data.get('lastPipeEvent', None)

        if event is not None:
            ahead = ext.math.Vector3(event.x, event.y, 0)
            quat = ext.math.Vector3.UNIT_Y.getRotationTo(ahead)
            self._direction = quat.getYaw(True)
            self._speed = self._config.get('speed', 0.5)

            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = yawVehicleHelper(currentOrientation, 
                                                      self._direction),
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = 0.3)
            translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                initialValue = self.stateEstimator.getEstimatedPosition(),
                finalValue = ext.math.Vector2(5,0),
                initialRate = self.stateEstimator.getEstimatedVelocity())

            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
            
            self.motionManager.setMotion(yawMotion)
            self.motionManager.setMotion(translateMotion)
        else:
            self.motionManager.stopCurrentMotion()

class Searching(PipeTrackingState):
    """When the vehicle is looking for a pipe"""
    
    @staticmethod
    def transitions():
        return PipeTrackingState.transitions(Searching,
            { vision.EventType.PIPE_FOUND : Seeking })

    @staticmethod
    def getattr():
        return set(['forwardSpeed'])

    def PIPE_FOUND(self, event):
        currentID = self.ai.data['pipeData'].setdefault('currentID', event.id)
        if event.id == currentID:
            self.ai.data['lastPipeEvent'] = event

    def enter(self):
        PipeTrackingState.enter(self)
        
        # Turn on the vision system
        self.visionSystem.pipeLineDetectorOn()

        # Set the start orientation if it isn't already set
        orientation = self.stateEstimator.getEstimatedOrientation()
        direction = self.ai.data.setdefault('pipeStartOrientation',
                                            orientation.getYaw().valueDegrees())


        #TODO: Change this back into a ZigZag motion
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, self.ai.data.setdefault('pipeStartOrientation',orientation.getYaw().valueDegrees())),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = 0.3)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = ext.math.Vector2(5,0),
            initialRate = self.stateEstimator.getEstimatedVelocity())
        
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(yawMotion)
        self.motionManager.setMotion(translateMotion)

class Seeking(PipeFollowingState):
    """When the vehicle is moving over the found pipe"""
    
    @staticmethod
    def transitions():
        return PipeFollowingState.transitions(Seeking,
            { vision.EventType.PIPE_CENTERED : Centering })

class Centering(PipeFollowingState):
    """
    When the vehicle is settling over the pipe
    
    @cvar SETTLED: Event fired when vehicle has settled over the pipe
    """
    SETTLED = core.declareEventType('SETTLED')
    
    @staticmethod
    def transitions():
        return PipeFollowingState.transitions(Centering,
            { Centering.SETTLED : AlongPipe })

    def _withinRange(self, values):
        pos = abs(values[0]) < self._planeThreshold and \
            abs(values[1]) < self._planeThreshold and \
            abs(values[2]) < self._angleThreshold
        vel = abs(values[0] - self._startX) < self._planeChange and \
            abs(values[1] - self._startY) < self._planeChange and \
            abs(values[2] - self._startAngle) < self._angleChange
        return pos and vel

    #def FOUND_PIPE(self, event):
    #    status = PipeFollowingState.FOUND_PIPE(self, event)

        # Wait for 'delay' pipe found events to happen first
        #self._numEvents += 1

        # Check to make sure it's not an outdated event
        #if status and self._delay < self._numEvents:
            # Check the change over time. If it's low enough,
            # declare it settled
        #    if self._withinRange((event.x, event.y,
        #                          event.angle.valueDegrees())):
        #        self.publish(Centering.SETTLED, core.Event())

        #self._startX, self._startY, self._startAngle = \
        #    event.x, event.y, event.angle.valueDegrees()

    def enter(self):
        self._planeThreshold = self._config.get('planeThreshold', 0.03)
        self._angleThreshold = self._config.get('angleThreshold', 0.03)
        #self._planeChange = self._config.get('planeChange', 0.03)
        #self._angleChange = self._config.get('angleChange', 0.03)
        #self._delay = self._config.get('delay', 20)

        #self._numEvents = 0

        self.timer = self.timerManager.newTimer(Centering.SETTLED, 5)
        self.timer.start()
        
        PipeFollowingState.enter(self)

    def exit(self):
        #print '"Exiting Seek, going to follow"'
        PipeFollowingState.exit(self)
        self.timer.stop()

class AlongPipe(PipeFollowingState):
    """
    When the vehicle is following along a visible pipe
    """
    FOUND_NEW_PIPE = core.declareEventType('FOUND_NEW_PIPE')
    
    @staticmethod
    def transitions():
        trans = { vision.EventType.PIPE_LOST : BetweenPipes, 
                 vision.EventType.PIPE_FOUND : AlongPipe,
                 vision.EventType.PIPE_DROPPED : AlongPipe,
                 AlongPipe.FOUND_NEW_PIPE : Seeking }
        return PipeTrackingState.transitions(AlongPipe, trans)

    @staticmethod
    def getattr():
        return set(['angleDistance', 'forwardSpeed']).union(
            PipeFollowingState.getattr())

    def PIPE_FOUND(self, event):
        """Update the state of the pipe, this moves the vehicle"""
        
        # Determine if a new pipe has appeared in the field of view
        newPipeLoc = ext.math.Vector3(event.x, event.y, 0)
        if self._lastPipeLoc is not None:
            if self._lastPipeLoc.distance(newPipeLoc) > 0.5:
                self.publish(AlongPipe.FOUND_NEW_PIPE, core.Event())
        self._lastPipeLoc = newPipeLoc
        
        # Update the targets state
        angle = ext.math.Degree(0)
        if event.x < self._angleDistance and event.y < self._angleDistance:
            angle = event.angle
        event.angle = angle

        # This must be called after the event is changed
        PipeFollowingState.PIPE_FOUND(self, event)

    def enter(self):
        """Makes the vehicle follow along line outlined by the pipe"""
        
        # Initial settings
        PipeFollowingState.enter(self)
        
        # Stop default motion created above
        self.motionManager.stopCurrentMotion()

        # Load our configuration settings
        self._lastPipeLoc = None
        self._angleDistance = self._config.get('angleDistance', 0.5)
        maxSpeed = self._config.get('forwardSpeed', 5)
        
        # Create our new motion to follow the pipe straight forward
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = ext.math.Vector2(15,0),
            initialRate = self.stateEstimator.getEstimatedVelocity())
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()
       
class BetweenPipes(PipeTrackingState):
    """
    When the vehicle is between two pipes, and can't see either.
    """
    LOST_PATH = core.declareEventType('LOST_PATH')
    
    @staticmethod
    def transitions():
        return PipeTrackingState.transitions(BetweenPipes,
                                             {vision.EventType.PIPE_FOUND : Seeking,
                                              BetweenPipes.LOST_PATH : End })

    @staticmethod
    def getattr():
        return set(['forwardTime', 'forwardSpeed'])
    
    def PIPE_FOUND(self, event):
        currentID = self.ai.data['pipeData'].setdefault('currentID', event.id)
        if event.id == currentID:
            self.ai.data['lastPipeEvent'] = event

    def enter(self):
        """We have driving off the 'end' of the pipe set a timeout"""
        PipeTrackingState.enter(self)
        
        forwardTime = self._config.get('forwardTime', 15)
        self.timer = self.timerManager.newTimer(BetweenPipes.LOST_PATH, 
						forwardTime)
        self.timer.start()
        
        self.controller.setSpeed(self._config.get('forwardSpeed', 5))
        
    def exit(self):
        PipeTrackingState.exit(self)
        
        self.controller.setSpeed(0)
        self.timer.stop()
        
class End(state.State):
    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        #print '"Pipe Follow"'
