# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/ai/sonar.py
  
# STD Imports
import math
  
# Project Imports
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device
import ext.math
from ext.control import yawVehicleHelper
from ext.control import holdCurrentHeadingHelper

import ram.ai.state as state
import ram.motion as motion
#import ram.motion.search
import ram.motion.pipe
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

class PingerState(state.State):
    """
    Base state for the using the Sonar data to hover toward and over the Pinger.
    
    Base classes are required to update the actual target with new Sonar 
    information.
    """
    TIMEOUT = core.declareEventType('PINGER_TIMEOUT')

    @staticmethod
    def transitions(myState, timeoutState = None, trans = None):
        if timeoutState is None:
            timeoutState = PingerLost
        if trans is None:
            trans = {}
        trans.update({vehicle.device.ISonar.UPDATE : myState,
                      PingerState.TIMEOUT : timeoutState})
        return trans

    @staticmethod
    def getattr():
        return set(['closeZ', 'speedGain', 'yawGain', 'maxSpeed', 'timeout',
                    'maxSidewaysSpeed', 'sidewaysSpeedGain', 'forwardSpeed',
                    'motionRange', 'distance', 'sonarError'])

    def UPDATE(self, event):
        if self._timeout > 0:
            self._pingChecker.stop()
            self._pingChecker = self.timerManager.newTimer(PingerState.TIMEOUT,
                                                           self._timeout)
            self._pingChecker.start()
    
    def _isNewPing(self, event):
        #if self._lastTime != event.pingTimeUSec:
        #    self._lastTime = event.pingTimeUSec
        return True
        #return False
    
    def _pingerAngle(self, event):
        pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
            event.direction)
        return pingerOrientation.getYaw(True)
    
    def _loadSettings(self):
        self._closeZ = self._config.get('closeZ', 0.85)
        self._speedGain = self._config.get('speedGain', 5)
        self._yawGain = self._config.get('yawGain', 1)
        self._maxSpeed = self._config.get('maxSpeed', 1)
        self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 2)
        self._forwardSpeed = self._config.get('forwardSpeed', 0.15)
        self._motionRange = self._config.get('motionRange', .02)
        self._distance = self._config.get('distance', 1)
        self.ai.data['minSonarError'] = self._config.get('sonarError', 1)
     
    def enter(self, timeout = 2.5):
        self._pipe = ram.motion.pipe.Pipe(0, 0, 0, timeStamp = None)
        self._lastTime = 0
        self._recalculate = False

        self._loadSettings()

        self._currentDesiredPos = ext.math.Vector2(self._pipe.getY() *
                                                   self._distance, 
                                                   self._pipe.getX() *
                                                   self._distance)
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        
        orientation = yawVehicleHelper(currentOrientation, 
                                       self._pipe.getAngle())
        
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = orientation,
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = ext.math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = ext.math.Vector2.ZERO,
            finalValue = self._currentDesiredPos,
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._forwardSpeed)
            
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        self.motionManager.setMotion(yawMotion, translateMotion)

        # Set up the ping timer
        self._pingChecker = None
        self._timeout = self._config.get('timeout', timeout)
        if self._timeout > 0:
            self._pingChecker = self.timerManager.newTimer(
                PingerState.TIMEOUT, self._timeout)
            self._pingChecker.start()

    def exit(self):
        if self._pingChecker is not None:
            self._pingChecker.stop()
        self.motionManager.stopCurrentMotion()
        
class Start(state.State):
    """
    Changes to the depth for proper sonar operation, then starts the search.
    """
    @staticmethod
    def transitions():
        return { motion.basic.Motion.FINISHED : Searching }

    @staticmethod
    def getattr():
        return set(['diveSpeed', 'offset', 'minimumDepth'])

    def enter(self):
        offset = self._config.get('offset', 0)
        desiredDepth = self.ai.data['config'].get('sonarDepth', 2)

        # Check that it's not lower than the minimum depth
        desiredDepth -= offset
        minimumDepth = self._config.get('minimumDepth', 0.5)
        if desiredDepth < minimumDepth:
            desiredDepth = minimumDepth

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = desiredDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._config.get('diveSpeed', 0.2))
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Searching(state.State):
    """
    Waits for a ping to happen, and when it gets one, turns toward the source.
    Then it waits for 4 seconds, to allow the any disturbances in pinger data 
    from the vehicles rotation to die down, and then moves on to the next state. 
    """
    CHANGE = core.declareEventType("CHANGE")

    @staticmethod
    def transitions():
        return { vehicle.device.ISonar.UPDATE : Searching,
                 Searching.CHANGE : FarSeeking }
        
    def UPDATE(self, event):
        if self._first:
            pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
                event.direction)
            self.controller.yawVehicle(pingerOrientation.getYaw(True).valueDegrees())
            
            self.timer = self.timerManager.newTimer(Searching.CHANGE, 4)
            self.timer.start()
            self._first = False

    def enter(self):
        self._first = True
        self.timer = None
        
    def exit(self):
        if self.timer is not None:
            self.timer.stop()

class TranslationSeeking(PingerState):
    """
    Feeds the pinger data into the target and causes the vehicle to translate
    toward the pinger.
    """
    CLOSE = core.declareEventType('CLOSE')

    @staticmethod
    def transitions(myState, timeoutState = None, trans = None):
        if myState is None:
            myState = TranslationSeeking
        if timeoutState is None:
            timeoutState = PingerLost
        if trans is None:
            trans = {}
        trans.update({vehicle.device.ISonar.UPDATE : myState,
                      PingerState.TIMEOUT : timeoutState,
                      motion.basic.MotionManager.FINISHED : myState})
        return trans
    
    def _loadSettings(self):
        PingerState._loadSettings(self)
        #self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        #self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 2)
    
    def FINISHED(self, event):
        self._recalculate = True
        
    def UPDATE(self, event):
        PingerState.UPDATE(self, event)
        if self._isNewPing(event):
            # Converting from the vehicle reference frame, to the image space
            # reference frame used by the pipe motion
            self._pipe.setState(-event.direction.y, event.direction.x, 
                                 ext.math.Degree(0), event.timeStamp)
            
            if not self._changeMotion(self._currentDesiredPos, 
                                      ext.math.Vector2(self._pipe.getY() *
                                                       self._distance,
                                                       self._pipe.getX() *
                                                       self._distance)) or self._recalculate:
                self._recalculate = False
                return
            
            self._currentDesiredPos = ext.math.Vector2(self._pipe.getY() *
                                                       self._distance, 
                                                       self._pipe.getX() * 
                                                       self._distance)
            
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            
            orientation = yawVehicleHelper(currentOrientation, 
                                           self._pipe.getAngle())
            
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = orientation,
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = ext.math.Vector3.ZERO)
            translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                initialValue = ext.math.Vector2.ZERO,
                finalValue = self._currentDesiredPos,
                initialRate = self.stateEstimator.getEstimatedVelocity(),
                avgRate = 0.15)

            #self.motionManager.stopCurrentMotion()
            
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
            translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                         frame = Frame.LOCAL)
            self.motionManager.setMotion(yawMotion, translateMotion)

            if math.fabs(event.direction.z) > math.fabs(self._closeZ):
                 self.publish(TranslationSeeking.CLOSE, core.Event())

    def _changeMotion(self, vector1, vector2):
        # Another messy solution, compares two vectors to see if there
        # is much of a difference between them. Returns True if there is
        # is significant difference

        position = self.stateEstimator.getEstimatedPosition()
        depth = self.stateEstimator.getEstimatedDepth()
        obstacle = ext.math.Vector3(vector2.x, vector2.y, depth)
        real = ext.math.Vector3(position.x, position.y, depth)
        error = real - obstacle

        if math.sqrt((vector2.x - vector1.x) * (vector2.x - vector1.x) + 
                     (vector2.y - vector1.y) * 
                     (vector2.y - vector1.y)) < self._motionRange:
            return False
        elif error.length() < self.ai.data['minSonarError']:
            return False
        return True
            
class FarSeeking(state.State):

    CLOSE = core.declareEventType('CLOSE')

    @staticmethod
    def transitions():
        return { vehicle.device.ISonar.UPDATE : FarSeeking,
          FarSeeking.CLOSE : CloseSeeking }
    
    @staticmethod
    def getattr():
        return set(['closeZ', 'speedGain', 'yawGain', 'maxSpeed', 'timeout',
                    'maxSidewaysSpeed', 'sidewaysSpeedGain', 'forwardSpeed',
                    'motionRange', 'distance', 'sonarError'])

    def UPDATE(self, event):
        if math.fabs(event.direction.z) > math.fabs(self._closeZ):
            self.publish(FarSeeking.CLOSE, core.Event())
            return

        #if not self._first:
        #    return

        direction = event.direction
        
        direction.normalise()

        translateTrajectory = motion.trajectories.Vector2VelocityTrajectory(
            velocity = ext.math.Vector2(direction.x * 0.3, - direction.y * 0.3),
            initialPosition = ext.math.Vector2.ZERO,
            maxDistance = 2)

        translateMotion = motion.basic.Translate(translateTrajectory,
                                           frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)
        self._first = False
        

    def _loadSettings(self):
        self._closeZ = self._config.get('closeZ', 0.85)
        self._speedGain = self._config.get('speedGain', 5)
        self._yawGain = self._config.get('yawGain', 1)
        self._maxSpeed = self._config.get('maxSpeed', 1)
        self._maxSidewaysSpeed = self._config.get('maxSidewaysSpeed', 2)
        self._sidewaysSpeedGain = self._config.get('sidewaysSpeedGain', 2)
        self._forwardSpeed = self._config.get('forwardSpeed', 0.15)
        self._motionRange = self._config.get('motionRange', .02)
        self._distance = self._config.get('distance', 1)
        self.ai.data['minSonarError'] = self._config.get('sonarError', 1)

    def enter(self):
        self._loadSettings()

    def exit(self):
        self.motionManager.stopCurrentMotion()
        

# # 0.65
# class FarSeeking(TranslationSeeking):
#     """
#     Approaches the pinger from a far away range, it moves quicker, and can 
#     possibly lose pings.  If this is used close to a pinger, it can and *will*
#     overshoot.
#     """
#     @staticmethod
#     def transitions():
#         return TranslationSeeking.transitions(FarSeeking, PingerLost,
#             { TranslationSeeking.CLOSE : CloseSeeking } ) 

#     @staticmethod
#     def getattr():
#         return set(['midRangeZ', 'distance', 'motionRange']).union(TranslationSeeking.getattr())
                 
#     def _loadSettings(self):
#         TranslationSeeking._loadSettings(self)
#         self._closeZ = self._config.get('midRangeZ', 0.65)
#         self._distance = self._config.get('distance', 10)
#         self._motionRange = self._config.get('motionRange', .5)
                 
class CloseSeeking(TranslationSeeking):
    """
    For when we are close to the pinger, moves slow enough that it should not 
    miss any pings or overshoot but is too slow to use from far away.
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(CloseSeeking, PingerLostClose,
            { TranslationSeeking.CLOSE : End } ) 

    @staticmethod
    def getattr():
        return set(['closeZ', 'distance', 'motionRange']).union(TranslationSeeking.getattr())

    def _loadSettings(self):
        TranslationSeeking._loadSettings(self)
        self._closeZ = self._config.get('closeZ', 0.8)
        self._distance = self._config.get('distance', 2)
        self._motionRange = self._config.get('motionRange', .05)
    
class Hovering(TranslationSeeking):
    """
    Just hovers over the pinger
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(Hovering, PingerLostHovering)

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())

class PingerLost(state.FindAttempt):
    @staticmethod
    def transitions(foundState = FarSeeking, timeoutState = Searching):
        return state.FindAttempt.transitions(
            vehicle.device.ISonar.UPDATE, foundState, timeoutState)

    def enter(self):
        state.FindAttempt.enter(self)

        self.motionManager.stopCurrentMotion()

class PingerLostClose(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(CloseSeeking)

class PingerLostHovering(PingerLost):
    @staticmethod
    def transitions():
        return PingerLost.transitions(Hovering)
