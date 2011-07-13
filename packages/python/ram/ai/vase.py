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
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Aligning(VaseTrackingState):

    CENTERED = core.declareEventType('CENTERED')
    
    @staticmethod
    def transitions():
        return VaseTrackingState.transitions(Aligning,
            { Aligning.CENTERED : FastDive })

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

class FastDive(state.State):

    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions():
        return { FastDive.IN_RANGE : ReAlign,
                 motion.basic.MotionManager.FINISHED : FastDive }

    @staticmethod
    def getattr():
        return {'distanceFromBottom' : 6.5, 'intervals' : 1, 
                'depthRate' : 0.2}

    def enter(self):
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.stateEstimator.getEstimatedDepth() + \
                self._intervals,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    def FINISHED(self, event):
        depth = self.stateEstimator.getEstimatedDepth()
        bottomRange = self.stateEstimator.getEstimatedBottomRange()
        
        if self._distanceFromBottom <= depth:
            self.publish(FastDive.IN_RANGE, core.Event())
        else:            
            diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                initialValue = depth,
                finalValue = depth + self._intervals,
                initialRate = self.stateEstimator.getEstimatedDepthRate(),
                avgRate = self._depthRate)
            diveMotion = motion.basic.ChangeDepth(
                trajectory = diveTrajectory)
            self.motionManager.setMotion(diveMotion)

            
    def exit(self):
        self.motionManager.stopCurrentMotion()

class ReAlign(VaseTrackingState):


    CENTERED = core.declareEventType('CENTERED')
    
    @staticmethod
    def transitions():
        return VaseTrackingState.transitions(ReAlign,
            { ReAlign.CENTERED : FastDive })

    @staticmethod
    def getattr():
        attrs = {}
        attrs.update(VaseTrackingState.getattr())
        attrs.update({'delay' : 10})
        return attrs

    def enter(self):
        VaseTrackingState.enter(self, 90)

        self.timer = self.timerManager.newTimer(ReAlign.CENTERED, 
                                                self._delay)
        self.timer.start()

    def exit(self):
        VaseTrackingState.exit(self)
        self.timer.stop()

class SlowDive(state.State):

    IN_RANGE = core.declareEventType('IN_RANGE')
    
    @staticmethod
    def transitions():
        return VaseTrackingState.transitions(SlowDive,
            { SlowDive.IN_RANGE : SlowDive,
              motion.basic.MotionManager.FINISHED : FastDive })

    @staticmethod
    def getattr():
        return {'distanceFromBottom' : 4.5, 'intervals' : .25, 
                'depthRate' : 0.2}

    def enter(self):

        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.stateEstimator.getEstimatedDepth() + \
                self._intervals,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    def FINISHED(self, event):
        depth = self.stateEstimator.getEstimatedDepth()
        bottomRange = self.stateEstimator.getEstimatedBottomRange()
        
        if self._distanceFromBottom <= depth:
            self.publish(SlowDive.IN_RANGE, core.Event())
        else:            
            diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
                initialValue = depth,
                finalValue = depth + self._intervals,
                initialRate = self.stateEstimator.getEstimatedDepthRate(),
                avgRate = self._depthRate)
            diveMotion = motion.basic.ChangeDepth(
                trajectory = diveTrajectory)
            self.motionManager.setMotion(diveMotion)

            
    def exit(self):
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

        #TODO: ACTIVATE GRABBER

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
