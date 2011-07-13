# Copyright (C) 2011 Maryland Robotics Club
# Copyright (C) 2011 Gary Sullivan <gsulliva@umd.edu>
# All rights reserved.
#
# Author: Gary Sullivan <gsulliva@umd.edu>
# File:  packages/python/ram/ai/cupid.py

# STD Imports
import math as pmath
import string as pstring

# Project Imports
import ext.core as core
import ext.math as math
import ext.vision as vision
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

def getObstacleType(heart):
    heart = pstring.lower(heart)
    if heart == 'large':
        return estimation.Obstacle.ObstacleType.LARGE_HEART
    elif buoy == 'small':
        return estimation.Obstacle.ObstacleType.SMALL_HEART

class Start(state.State):
    """
    Calculates and moves to a starting position and orientation
    specified by configuration values, and starts the detector.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Aim }

    @staticmethod
    def getattr():
        return {'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self.visionSystem.cupidDetectorOn()

        windowX = self.ai.data['config'].get('windowX', -1)
        windowY = self.ai.data['config'].get('windowY', -1)
        windowDepth = self.ai.data['config'].get('windowDepth', -1)
        self.ai.data['windowOrientation'] = self.ai.data['config'].get('windowOrientation', -1)

        self._orientation = self.ai.data['windowOrientation']
        
        self.ai.data['heartSize'] = self.ai.data['config'].get('heartSize', 'large')

        if windowX == -1 or windowY == -1 or windowDepth == -1  or \
                self._orientation == -1:
            raise LookupError, "windowX, windowY, windowDepth or windowOrientation not specified"


        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = windowDepth,
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
            finalValue = math.Vector2(windowY,windowX),
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

class Aim(state.State)
    """
    Moves in front of the window.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Fire }

    @staticmethod
    def getattr():
        return {'distance' : 2, 'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['windowOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        heartSize = self.ai.data['heartSize']

        windowObstacle = getObstacleType(heartSize)
        
        windowLocation = self.stateEstimator.getObstacleLocation(windowObstacle)

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
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Fire(state.State):
    """
    Launch a torpedo
    """
    FIRED = core.declareEventType('FIRED')
    NONE_FOUND = core.declareEventType('NONE_FOUND')

    @staticmethod
    def _transitions():
        return { Fire.FIRED : Rise,
                 Fire.NONE_FOUND : Fire}

    @staticmethod
    def getattr():
        return {'timout': 20}

    def CUPID_FOUND(self, event):
        # grab the color, fire the appropriate torpedo
        if event.color == vision.Color.ColorType.BLUE:
            self.vehicle.fireTorpedo(0)
            self.publish(Fire.FIRED, core.Event())
        elif event.color == vision.Color.ColorType.RED:
            self.vehicle.fireTorpedo(1)
            self.publish(Fire.FIRED, core.Event())
        # Ignore other colors 

    def NONE_FOUND(self,event):
        # We didn't find a window in time, just fire and move on.
        self.vehicle.fireTorpedo(0)
        self.publish(Fire.FIRED, core.Event())

    def enter(self):
        self._timer = self.timerManager.newTimer(Fire.NONE_FOUND, self._timeout)
        self._timer.start()
        
    def exit(self):
        if self._timer is not None:
            self._timer.stop()

class Rise(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : SwitchWindow }

    @staticmethod
    def getattr():
        return {'depthRate' : 0.2, 'height' : 3.5}

    def enter(self):
        depth = self.stateEstimator.getEstimatedDepth() + self._height
        if depth < 2:
            depth = 2
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class SwitchWindow(state.State):
    """
    Moves in front of the window and above it.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Aim2 }

    @staticmethod
    def getattr():
        return {'distance' : 3, 'speed' : 0.15}

    def enter(self):
        self.ai.data['windowOrientation']  = \
            self.ai.data['windowOrientation'] + 180

        if self.ai.data['windowOrientation'] > 180:
            self.ai.data['windowOrientation'] = -180 + \
                self.ai.data['windowOrientation'] - 180

        self._orientation = self.ai.data['windowOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        heartSize = self.ai.data['heartSize']

        windowObstacle = getObstacleType(heartSize)
        
        windowLocation = self.stateEstimator.getObstacleLocation(windowObstacle)

        xDesired = buoyLocation.x + xOffset
        yDesired = buoyLocation.y + yOffset

        # Compute trajectories
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(yDesired, xDesired),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self._orientation), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)

        # Dive and translate
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(yawMotion)
        self.motionManager.setMotion(translateMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Aim2(state.State)
    """
    Moves in front of the window.
    """
    @staticmethod
    def transitions(foundState = None, lostState = None):
        return { motion.basic.MotionManager.FINISHED : Fire2 }

    @staticmethod
    def getattr():
        return {'distance' : 2, 'diveRate' : 0.3, 'speed' : 0.15}

    def enter(self):
        self._orientation = self.ai.data['windowOrientation']

        # Compute where we want to be
        xOffset = -self._distance * pmath.sin(self._orientation)
        yOffset = self._distance * pmath.cos(self._orientation)
        
        heartSize = self.ai.data['heartSize']

        windowObstacle = getObstacleType(heartSize)
        
        windowLocation = self.stateEstimator.getObstacleLocation(windowObstacle)

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
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion)
        self.motionManager.setMotion(translateMotion)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

class Fire2(state.State):
    """
    Launch a torpedo
    """
    FIRED = core.declareEventType('FIRED')
    NONE_FOUND = core.declareEventType('NONE_FOUND')

    @staticmethod
    def _transitions():
        return { Fire2.FIRED : Rise2,
                 Fire2.NONE_FOUND : Fire2}

    @staticmethod
    def getattr():
        return {'timout': 20}

    def CUPID_FOUND(self, event):
        # grab the color, fire the appropriate torpedo
        if event.color == vision.Color.ColorType.BLUE:
            self.vehicle.fireTorpedo(0)
            self.publish(Fire2.FIRED, core.Event())
        elif event.color == vision.Color.ColorType.RED:
            self.vehicle.fireTorpedo(1)
            self.publish(Fire2.FIRED, core.Event())
        # Ignore other colors 

    def NONE_FOUND(self,event):
        # We didn't find a window in time, just fire and move on.
        self.vehicle.fireTorpedo(1)
        self.publish(Fire2.FIRED, core.Event())

    def enter(self):
        self._timer = self.timerManager.newTimer(Fire2.NONE_FOUND, self._timeout)
        self._timer.start()
        
    def exit(self):
        if self._timer is not None:
            self._timer.stop()

class Rise2(state.State):
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return {'depthRate' : 0.2, 'height' : 4}

    def enter(self):
        depth = self.stateEstimator.getEstimatedDepth() + self._height
        if depth < 2:
            depth = 2
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._depthRate)
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class End(state.State):
    def enter(self):
        self.visionSystem.cupidDetectorOff()
        self.publish(COMPLETE, core.Event())
