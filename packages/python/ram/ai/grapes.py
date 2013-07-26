# Copyright (C) 2012 Maryland Robotics Club
# Copyright (C) 2012 Stephen Christian <schrist2@umd.edu>
# All rights reserved.
#
# Author: Stephen Christian <schrist2@terpmail.umd.edu>
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

class Start(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search }

    @staticmethod
    def getattr():
        return { 'diveRate' : 0.3 }

    def enter(self):
        self.visionSystem.buoyDetectorOn()
        
        grapesDepth = self.ai.data['config'].get('grapesDepth', -1)

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = grapesDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        
        # Dive to specified depth
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Search(state.ZigZag):

    @staticmethod
    def transitions():
        return { vision.EventType.BUOY_FOUND : ReAlign , 
                 state.ZigZag.DONE : End }


class ReAlign(state.State):

    DONE = core.declareEventType('DONE')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return {  ReAlign.DONE : Strafe }

    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'distance' : 4 , 'delay' : 3 }

    def enter(self):
        windowOrientation = self.ai.data['config'].get('windowOrientation', 9001)

        if windowOrientation > 9000:
            raise LookupError, "windowOrientation not specified"

        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(math.Degree(windowOrientation), 
                                         math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
            
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.motionManager.setMotion(yawMotion)

        self.timer = self.timerManager.newTimer(ReAlign.DONE, self._delay)
        self.timer.start()
        
    def exit(self):
        self.motionManager.stopCurrentMotion()


class Strafe(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End ,
                 vision.EventType.BUOY_FOUND : UpCenter }

    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'distance' : 4 }

    def enter(self):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, self._distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class UpCenter(state.Center):
    
    @staticmethod
    def transitions():
        return { vision.EventType.BUOY_FOUND : UpCenter ,
                 state.Center.CENTERED : Approach , 
                 state.Center.TIMEOUT : End }

 
    def BUOY_FOUND(self, event):
        self.update(event)


class Approach(state.State):

    DONE = core.declareEventType('DONE')

    @staticmethod
    def transitions():
        return { Approach.DONE : Touch ,
                 vision.EventType.BUOY_FOUND : Approach ,
                 motion.basic.MotionManager.FINISHED : Approach }
        
    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'correct_factor' : 2 ,
                 'step_dist' : 0.5 , 'max_dist' : 4,
                 'timeout' : 30 }

    def enter(self):
        self.X = 0
        self.Y = 0
        self.DISTANCE = 0
        
        self.timer = self.timerManager.newTimer(Approach.DONE, self._timeout)
        self.timer.start()
    
    def exit(self):
        if self.timer is not None:
            self.timer.stop()

    def update(self):
        
        if(self.DISTANCE >= self._max_dist):
            self.publish(Approach.DONE, core.Event())

        else:
            translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                initialValue = math.Vector2.ZERO,
                finalValue = math.Vector2(self._step_dist, 
                                          self._correct_factor * self.X),
                initialRate = self.stateEstimator.getEstimatedVelocity(),
                avgRate = self._speed)
            translateMotion = motion.basic.Translate(
                trajectory = translateTrajectory,
                frame = Frame.LOCAL)
            
            self.motionManager.setMotion(translateMotion)
            self.DISTANCE += self._step_dist

    def BUOY_FOUND(self, event):
        
        self.X = event.x
        self.Y = event.y
        
        if(self.DISTANCE == 0):
            self.update()
        
    def FINISHED(self, event):
        self.update() 


class Touch(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Derpy ,
                 vision.EventType.BUOY_LOST : Derpy }

    @staticmethod
    def getattr():
        return { 'distance' : 4 , 'speed' : 0.15 }

    def enter(self):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)


class Derpy(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : End }

    @staticmethod
    def getattr():
        return { 'distance' : 1 , 'speed' : 0.3 }

    def enter(self):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-self._distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)


class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
