# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Gary Sullivan <gsulliva@umd.edu>
# All rights reserved.
#
# Author: Gary Sullivan <gsulliva@umd.edu>
# File:  packages/python/ram/ai/lane.py

# Standard imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

class LaneInfo(object):
    """
    A class to hold information about the lane for tracking it
    """
    def __init__(self, x, y):
        self._x = x
        self._y = y

    def getX(self):
        return self._x
    
    def getY(self):
        return self._y

    def setState(self, x, y):
        self._x = x
        self._y = y

class Start(state.State):
    
    """
    Moves to the correct depth and orientation
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Search }

    @staticmethod
    def getattr():
        return {'diveRate' : 0.3}

    def enter(self):
        self.visionSystem.loversLaneDetectorOn()

        depth = self.ai.data['laneDepth']
        orientation = self.ai.data['laneOrientation']

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, orientation),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)

        # Dive and translate
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        
        self.motionManager.setMotion(yawMotion, diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Search(state.State):
    """When the vehicle is looking for the Lovers Lane"""

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End,
                 vision.EventType.LOVERSLANE_FOUND : Aligning }

    @staticmethod
    def getattr():
        return {'speed' : 0.15, 'distance' : 6}

    def enter(self):

        #TODO: Change this into a ZigZag motion
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Aligning(state.State):
    """When the vehicle is aligning to the Lovers Lane"""
    
    CENTERED = core.declareEventType('CENTERED')
    
    @staticmethod
    def transitions():
        return { vision.EventType.LOVERSLANE_FOUND : Aligning,
                 Aligning.CENTERED : Forward }

    @staticmethod
    def getattr():
        return {'speed' : 0.15, 'diveRate' : .2, 'centerBoundary' : 0.1,
                'diveSpeed' : 3, 'sidewaysSpeed' : 1, 'motionRange' : 0.3}

    def LOVERSLANE_FOUND(self, event):
        if pmath.fabs(event.centerX) < self._centerBoundary and \
                pmath.fabs(event.centerY) < self._centerBoundary:
            self.publish(Aligning.CENTERED, core.Event())
            return

        self._lane.setState(event.centerX * self._sidewaysSpeed,
                            event.centerY * self._diveSpeed)
        self.changedLane()


    def changedLane(self):
        if not self._changeMotion(self._currentDesiredPos, math.Vector2( 
                self._lane.getX(), self._lane.getY())):
            return
        
        
        self._currentDesiredPos = math.Vector2(self._lane.getX(), 
                                                   self._lane.getY())
        self.motionManager.stopCurrentMotion()

        depth = self.stateEstimator.getEstimatedDepth()


        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = depth,
            finalValue = depth - self._lane.getY(),
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, self._lane.getX()),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.LOCAL)
        
        self.motionManager.setMotion(diveMotion, translateMotion)


    def _changeMotion(self, vector1, vector2):
        if pmath.sqrt((vector2.x - vector1.x) * (vector2.x - vector1.x) + 
                     (vector2.y - vector1.y) * 
                     (vector2.y - vector1.y)) < self._motionRange:
            return False
        return True

    def enter(self):
        self._lane = LaneInfo(0, 0)
        self._currentDesiredPos = math.Vector2(self._lane.getX(), 
                                                   self._lane.getY())

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Forward(state.State):
    """
    Moves directly forward a specified distance.
    """

    @staticmethod
    def transitions():
        return {motion.basic.MotionManager.FINISHED : End}

    @staticmethod
    def getattr():
        return { 'distance' : 5 , 'avgRate' : 0.15, 'angle' : 0}

    def enter(self):
        forwardTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._avgRate)

        forwardMotion = motion.basic.Translate(
            trajectory = forwardTrajectory,
            frame = Frame.LOCAL)

        # Full speed ahead!!
        self.motionManager.setMotion(forwardMotion)
    
    def exit(self):
        self.motionManager.stopCurrentMotion()
        
class End(state.State):
    def enter(self):
        self.visionSystem.loversLaneDetectorOff()
        self.publish(COMPLETE, core.Event())
