# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Gary Sullivan <gsulliva@umd.edu>
# All rights reserved.
#
# Author: Gary Sullivan <gsulliva@umd.edu>
# File:  packages/python/ram/ai/vase.py
  
# STD Imports
import math as pmath
  
# Project Imports
import ext.core as core
import ext.vehicle as vehicle
import ext.vehicle.device
import ext.vision as vision
import ext.math as math
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

class VaseInfo(object):
    """
    A class to hold information about the vase for tracking it
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

class VaseTrackingState(state.State):
    """
    A tracking state based off of the PipeTrackingState. This continuously 
    centers over a vase at a specified relative angle to the vase.
    """
    
    @staticmethod
    def transitions(myState = None, trans = None):
        if myState is None:
            myState = VaseTrackingState
        if trans is None:
            trans = {}

        newTrans = {vision.EventType.PIPE_FOUND : myState}
        newTrans.update(trans)
        return newTrans

    @staticmethod
    def getattr():
        return {'forwardRate' : 1, 'sidewaysRate' : 1, 'motionRange' : .1,
                'forwardSpeed' : .15}

    def PIPE_FOUND(self, event):
        """Update the information and move the vehicle"""
        
        angle = math.Degree(self._angleOffset) + event.angle
        self._vase.setState(event.x, event.y, angle.valueDegrees())
        self.changedVase()
        
    def enter(self, angle = 0):
        self._vase = VaseInfo(0,0,0)
        self._angleOffset = angle
       
        self._currentDesiredPos = ext.math.Vector2(self._vase.getY() * 
                                                   self._forwardRate,
                                                   self._vase.getX() * 
                                                   self._sidewaysRate)

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          self._vase.getAngle()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = ext.math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = ext.math.Vector2.ZERO,
            finalValue = self._currentDesiredPos,
            initialRate = ext.math.Vector2.ZERO,
            avgRate = self._forwardSpeed)
        
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(yawMotion, translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

    def changedVase(self):
        if not self._changeMotion(self._currentDesiredPos, 
                                  ext.math.Vector2(self._vase.getY() *
                                                   self._forwardRate,
                                                   self._vase.getX() * 
                                                   self._sidewaysRate)):
            return

        self._currentDesiredPos = ext.math.Vector2(self._vase.getY() * 
                                                   self._forwardRate,
                                                   self._vase.getX() * 
                                                   self._sidewaysRate)
        self.motionManager.stopCurrentMotion()

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          self._vase.getAngle()),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = ext.math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = ext.math.Vector2.ZERO,
            finalValue = self._currentDesiredPos,
            initialRate = ext.math.Vector2.ZERO,
            avgRate = self._forwardSpeed)
        
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(yawMotion, translateMotion)


    def _changeMotion(self, vector1, vector2):
        # Another messy solution, compares two vectors to see if there
        # is much of a difference between them. Returns True if there is
        # is significant difference
        if pmath.sqrt((vector2.x - vector1.x) * (vector2.x - vector1.x) + 
                     (vector2.y - vector1.y) * 
                     (vector2.y - vector1.y)) < self._motionRange:
            return False
        return True


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
            timeoutState = SlowDive
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
            timeoutState = SlowDive
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

            if pmath.fabs(event.direction.z) > pmath.fabs(self._closeZ):
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

        if pmath.sqrt((vector2.x - vector1.x) * (vector2.x - vector1.x) + 
                     (vector2.y - vector1.y) * 
                     (vector2.y - vector1.y)) < self._motionRange:
            return False
        elif error.length() < self.ai.data['minSonarError']:
            return False
        return True
            

class Start(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Aligning }

    @staticmethod
    def getattr():
        return {'depth' : 5, 'depthRate' : 0.2}

    def enter(self):
        # Go to 5 feet
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self.ai.data['vaseOrientation']), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.motionManager.setMotion(yawMotion, diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Aligning(VaseTrackingState):

    CENTERED = core.declareEventType('CENTERED')
    
    NONE_FOUND = core.declareEventType('NONE_FOUND')
    
    @staticmethod
    def transitions():
        return VaseTrackingState.transitions(Aligning,
            { Aligning.CENTERED : CloseSeeking })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(VaseTrackingState.getattr())
        attrs.update({'delay' : 20})
        return attrs

    def enter(self):

        # Turn on the vision system
        self.visionSystem.pipeLineDetectorOn()

        VaseTrackingState.enter(self, 90)

        self.timer = self.timerManager.newTimer(Aligning.CENTERED, 
                                                self._delay)
        self.timer.start()

    def exit(self):
        VaseTrackingState.exit(self)
        self.timer.stop()
        
class CloseSeeking(TranslationSeeking):
    """
    For when we are close to the pinger, moves slow enough that it should not 
    miss any pings or overshoot but is too slow to use from far away.
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(CloseSeeking, FastDive,
            { TranslationSeeking.CLOSE : FastDive } ) 

    @staticmethod
    def getattr():
        return set(['closeZ', 'distance', 'motionRange']).union(TranslationSeeking.getattr())

    def _loadSettings(self):
        TranslationSeeking._loadSettings(self)
        self._closeZ = self._config.get('closeZ', 0.8)
        self._distance = self._config.get('distance', 1)
        self._motionRange = self._config.get('motionRange', .05)
    
class FastDive(state.State):

    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : ReAlign }

    @staticmethod
    def getattr():
        return {'distanceFromBottom' : 6.5, 'intervals' : 1, 
                'depthRate' : 0.2, 'depth' : 8}

    def enter(self):
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    # def FINISHED(self, event):
    #     depth = self.stateEstimator.getEstimatedDepth()
    #     bottomRange = self.stateEstimator.getEstimatedBottomRange()
        
    #     if self._distanceFromBottom <= depth:
    #         self.publish(FastDive.IN_RANGE, core.Event())
    #     else:            
    #         diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
    #             initialValue = depth,
    #             finalValue = depth + self._intervals,
    #             initialRate = self.stateEstimator.getEstimatedDepthRate(),
    #             avgRate = self._depthRate)
    #         diveMotion = motion.basic.ChangeDepth(
    #             trajectory = diveTrajectory)
    #         self.motionManager.setMotion(diveMotion)

            
    def exit(self):
        self.motionManager.stopCurrentMotion()

# class ReAlign(VaseTrackingState):


#     CENTERED = core.declareEventType('CENTERED')
    
#     @staticmethod
#     def transitions():
#         return VaseTrackingState.transitions(ReAlign,
#             { ReAlign.CENTERED : SlowDive })

#     @staticmethod
#     def getattr():
#         attrs = {}
#         attrs.update(VaseTrackingState.getattr())
#         attrs.update({'delay' : 10})
#         return attrs

#     def enter(self):
#         VaseTrackingState.enter(self, 90)

#         self.timer = self.timerManager.newTimer(ReAlign.CENTERED, 
#                                                 self._delay)
#         self.timer.start()

#     def exit(self):
#         VaseTrackingState.exit(self)
#         self.timer.stop()


class ReAlign(TranslationSeeking):
    """
    For when we are close to the pinger, moves slow enough that it should not 
    miss any pings or overshoot but is too slow to use from far away.
    """
    @staticmethod
    def transitions():
        return TranslationSeeking.transitions(ReAlign, SlowDive,
            { TranslationSeeking.CLOSE : SlowDive } ) 

    @staticmethod
    def getattr():
        return set(['closeZ', 'distance', 'motionRange']).union(TranslationSeeking.getattr())

    def _loadSettings(self):
        TranslationSeeking._loadSettings(self)
        self._closeZ = self._config.get('closeZ', 0.8)
        self._distance = self._config.get('distance', 1)
        self._motionRange = self._config.get('motionRange', .05)

class SlowDive(state.State):

    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Grabbing,
                 SlowDive.IN_RANGE : Grabbing }

    @staticmethod
    def getattr():
        return {'distanceFromBottom' : 4.5, 'intervals' : .25, 
                'depthRate' : 0.2, 'depth' : 10, 'delay' : 15}

    def enter(self):

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        self._timer = self.timerManager.newTimer(SlowDive.IN_RANGE, self._delay)
        self._timer.start()

    # def FINISHED(self, event):
    #     depth = self.stateEstimator.getEstimatedDepth()
    #     bottomRange = self.stateEstimator.getEstimatedBottomRange()
        
    #     if self._distanceFromBottom <= depth:
    #         self.publish(SlowDive.IN_RANGE, core.Event())
    #     else:            
    #         diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
    #             initialValue = depth,
    #             finalValue = depth + self._intervals,
    #             initialRate = self.stateEstimator.getEstimatedDepthRate(),
    #             avgRate = self._depthRate)
    #         diveMotion = motion.basic.ChangeDepth(
    #             trajectory = diveTrajectory)
    #         self.motionManager.setMotion(diveMotion)

            
    def exit(self):
        if self._timer is not None:
            self._timer.stop()
        self.motionManager.stopCurrentMotion()

class Grabbing(state.State):

    GRABBED = core.declareEventType('GRABBED')
    
    @staticmethod
    def transitions():
        return { Grabbing.GRABBED : Rise }

    @staticmethod
    def getattr():
        return {'delay' : 7}

    def enter(self):
        self.timer = self.timerManager.newTimer(Grabbing.GRABBED, 
                                                self._delay)
        self.timer.start()

        self.vehicle.closeGrabber()

    def exit(self):
        self.timer.stop()

class Rise(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End  }

    @staticmethod
    def getattr():
        return {'depth' : 7, 'depthRate' : 0.2}

    def enter(self):
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(state.State):
    def enter(self):
        self.visionSystem.pipeLineDetectorOff()
        self.publish(COMPLETE, core.Event())
