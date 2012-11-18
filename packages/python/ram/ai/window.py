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
        return { 'diveRate' : 0.3 , 'speed' : 0.3 }

    def enter(self):
        self.visionSystem.buoyDetectorOn()

        windowDepth = self.ai.data['config'].get('windowDepth', -1)

        if windowDepth == -1:
            raise LookupError, "windowDepth not specified"

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = windowDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        
        # Dive
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class Search(state.ZigZag):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search ,
                 vision.EventType.BUOY_FOUND : ReAlign , 
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
                 vision.EventType.BUOY_FOUND : Center }

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


class Center(state.State):
    
    CENTERED = core.declareEventType('CENTERED')
    TIMEOUT = core.declareEventType('TIMEOUT')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { vision.EventType.BUOY_FOUND : Center ,
                 Center.CENTERED : Fire }

    @staticmethod
    def getattr():
        return { 'speed' : 0.2 , 'diveRate' : 0.2 , 'distance' : 10 ,
                 'xmin' : -0.05 , 'xmax' : 0.05 , 
                 'ymin' : -0.05 , 'ymax' : 0.05 }

    def move(self, distance):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

    def dive(self, distance):
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.stateEstimator.getEstimatedDepth() + distance,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)

        self.motionManager.setMotion(diveMotion)

    def enter(self):
        self.STEPNUM = 0

    def exit(self):
        self.motionManager.stopCurrentMotion()

    def BUOY_FOUND(self, event):
        if(self.STEPNUM == 0):
            print("Window X: " + str(event.x))
            if(event.x <= self._xmin):
                print("Moving left to compensate")
                self.move(-self._distance)
            elif(event.x >= self._xmax):
                print("Moving right to compensate")
                self.move(self._distance)

            self.STEPNUM += 1
        
        elif(self.STEPNUM == 1):
            if(event.x > self._xmin and event.x < self._xmax):
                print("X Axis Aligned")
                self.motionManager.stopCurrentMotion()
                self.STEPNUM += 1

        elif(self.STEPNUM == 2):
            print("Window Y: " + str(event.y))
            if(event.y <= self._ymin):
                print("Moving down to compensate")
                self.dive(self._distance)
            elif(event.y >= self._ymax):
                print("Moving up to compensate")
                self.dive(-self._distance)

            self.STEPNUM += 1

        elif(self.STEPNUM == 3):
            if(event.y > self._ymin and event.y < self._ymax):
                print("Y Axis Aligned, All Done")
                self.motionManager.stopCurrentMotion()
                self.publish(Center.CENTERED, core.Event())


class Fire(state.State):
    
    FIRED = core.declareEventType('FIRED')
    DONE = core.declareEventType('DONE')
    
    @staticmethod
    def transitions():
        return { vision.EventType.BUOY_FOUND : Fire ,
                 Fire.FIRED : MoveOver ,
                 Fire.DONE : End }

    def BUOY_FOUND(self, event):
        self.vehicle.fireTorpedo()
        self.publish(Fire.FIRED, core.Event())


class MoveOver(state.State):

    DONE = core.declareEventType('DONE')
    
    STEPNUM = 0

    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'diveRate' : 0.3 , 
                 'height' : 3 , 'distance' : 5 , 'delay' : 5 }

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : MoveOver , 
                 MoveOver.DONE : Center }

    def enter(self):
        self.STEPNUM = 0
        self.nextStep()

    def exit(self):
        self.motionManager.stopCurrentMotion()

    def FINISHED(self, event):
        self.nextStep()

    def move(self, distance):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(distance, 0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

    def dive(self, distance):
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = self.stateEstimator.getEstimatedDepth() + distance,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)

        self.motionManager.setMotion(diveMotion)

    def nextStep(self):
        if( self.STEPNUM == 0 ):
            self.dive(-self._height)

        elif( self.STEPNUM == 1 ):
            self.move(self._distance)

        elif( self.STEPNUM == 2 ):
            self.dive(self._height)

        elif( self.STEPNUM == 3 ):
            currentOrientation = self.stateEstimator.getEstimatedOrientation()
            yawTrajectory = motion.trajectories.StepTrajectory(
                initialValue = currentOrientation,
                finalValue = currentOrientation * math.Quaternion(
                    math.Degree(180), math.Vector3.UNIT_Z),
                initialRate = self.stateEstimator.getEstimatedAngularRate(),
                finalRate = math.Vector3.ZERO)
            yawMotion = motion.basic.ChangeOrientation(yawTrajectory)

            self.motionManager.setMotion(yawMotion)

        else:
            self.timer = self.timerManager.newTimer(MoveOver.DONE, self._delay)
            self.timer.start()

        self.STEPNUM += 1

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
