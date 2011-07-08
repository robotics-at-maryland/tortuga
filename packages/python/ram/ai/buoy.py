# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Gary Sullivan <gsulliva@umd.edu>
# All rights reserved.
#
# Author: Gary Sullivan <gsulliva@umd.edu>
# File:  packages/python/ram/ai/buoy.py

# Standard imports
import math as pmath
import string as pstring

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

def getObstacleType(buoy):
    buoy = pstring.lower(buoy)
    if buoy == 'red':
        return estimation.Obstacle.ObstacleType.RED_BUOY
    elif buoy == 'yellow':
        return estimation.Obstacle.ObstacleType.YELLOW_BUOY
    elif buoy == 'green':
        return estimation.Obstacle.ObstacleType.GREEN_BUOY
    else:
        raise LookupError("buoy name '%s' is not a recognized Obtacle type %" % buoy)

class Start(state.State):
    """
    Calculates and moves to a starting position and orientation
    specified by configuration values, and starts the detector.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Identification }

    @staticmethod
    def getattr():
        return {'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self.visionSystem.buoyDetectorOn()
        
        ##########################
        # Move this to course.py #
        ##########################

        ##########################
        # Move this to course.py #
        ##########################


        buoyX = self.ai.data['config'].get('buoyX', -1)
        buoyY = self.ai.data['config'].get('buoyY', -1)
        buoyDepth = self.ai.data['config'].get('buoyDepth', -1)

        if buoyX == -1 or buoyY == -1 or buoyDepth == -1:
            raise LookupError, "buoyX, buoyY or buoyDepth not specified"

        self._orientation = self.ai.data['buoyOrientation']


        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self._orientation), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(buoyY,buoyX),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive yaw and translate
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(yawMotion)
        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

        ##########################
        # Move this to course.py #
        ##########################


        ##########################
        # Move this to course.py #
        ##########################

class Identification(state.State):
    """
    Moves in front of the leftmost Buoy. Holds orientation.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Identification2 }

    @staticmethod
    def getattr():
        return {'distance' : 2, 'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['buoyOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        leftmostBuoy = self.ai.data['buoyOrder'][0]

        buoyObstacle = getObstacleType(leftmostBuoy)
        
        buoyLocation = self.stateEstimator.getObstacleLocation(buoyObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyLocation.z,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(yDesired, xDesired),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive and translate
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Identification2(state.State):
    """
    Moves in front of the rightmost Buoy. Holds orientation.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Approach }

    @staticmethod
    def getattr():
        return {'distance' : 2, 'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['buoyOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        rightmostBuoy = self.ai.data['buoyOrder'][-1]

        buoyObstacle = getObstacleType(rightmostBuoy)
        
        buoyLocation = self.stateEstimator.getObstacleLocation(buoyObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyLocation.z,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(yDesired, xDesired),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive and translate
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Approach(state.State):
    """
    Moves in front of the Buoy we want to hit, to a close position.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Hit }

    @staticmethod
    def getattr():
        return {'distance' : 1, 'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['buoyOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        buoy = self.ai.data['buoyList'][0]

        buoyObstacle = getObstacleType(buoy)
        
        buoyLocation = self.stateEstimator.getObstacleLocation(buoyObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyLocation.z,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(yDesired, xDesired),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive and translate
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Hit(state.State):
    """
    Moves in front of the Buoy we want to hit, to a close position.
    """
    
    DONE = core.declareEventType('DONE')
    NOT_DONE = core.declareEventType('NOT_DONE')

    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { Hit.DONE : End,
                 Hit.NOT_DONE : Approach,
                 motion.basic.MotionManager.FINISHED : Hit }

    @staticmethod
    def getattr():
        return {'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['buoyOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        buoy = self.ai.data['buoyList'].pop(0)

        buoyObstacle = getObstacleType(buoy)
        
        buoyLocation = self.stateEstimator.getObstacleLocation(buoyObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = buoyLocation.z,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(buoyLocation.y, buoyLocation.x),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive and translate
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)

    def FINISHED(self, event):
        if len(self.ai.data['buoyList']) > 0:
            self.publish(Hit.NOT_DONE, core.Event())
        else:
            self.publish(Hit.DONE, core.Event())

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Leave(state.State):
    """
    Backs away and moves upward
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { Hit.DONE : End,
                 Hit.NOT_DONE : Approach,
                 motion.basic.MotionManager.FINISHED : Hit }

    @staticmethod
    def getattr():
        return {'distance' : 2, 'diveRate' : 0.3, 'depth' : 6, 
                'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['buoyOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        centerBuoy = self.ai.data['buoyList'][1]

        buoyObstacle = getObstacleType(centerBuoy)
        
        buoyLocation = self.stateEstimator.getObstacleLocation(buoyObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self._depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(yDesired, xDesired),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        self._orientation = self.ai.data['buoyOrientation']
        centerBuoy = self.ai.data['buoyOrder'][1]

        buoyObstacle = getObstacleType(centerBuoy)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(state.State):
    def enter(self):
        self.visionSystem.buoyDetectorOff()
        self.motionManager.stopCurrentMotion()
        self.publish(COMPLETE, core.Event())
