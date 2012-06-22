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
        self.visionSystem.windowDetectorOn()

        windowX = self.ai.data['config'].get('windowX', -1)
        windowY = self.ai.data['config'].get('windowY', -1)
        windowDepth = self.ai.data['config'].get('windowDepth', -1)

        if windowX == -1 or windowY == -1 or windowDepth == -1:
            raise LookupError, "windowX, windowY or windowDepth not specified"

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = windowDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)
        
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(windowX, windowY),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)

        # Dive yaw and translate
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        translateMotion = ram.motion.basic.Translate(translateTrajectory,
                                                     frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(diveMotion, translateMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class Search(state.ZigZag):

    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : ReAlign , 
                 state.ZigZag.DONE : End }


class ReAlign(state.State):

    FOUND = core.declareEventType('FOUND')
    DONE = core.declareEventType('DONE')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : ReAlign , 
                 ReAlign.FOUND : Center , 
                 ReAlign.DONE : End }

    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'distance' : 4 }

    def enter(self):
        self.STEPNUM = 0
        self.nextStep()
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

    def FINISHED(self):
        self.nextStep()

    def WINDOW_FOUND(self):
        if(STEPNUM > 0):
            self.publish(ReAlign.FOUND, core.Event())

    def nextStep(self):
        if(self.STEPNUM == 0):

            windowOrientation = self.ai.data['config'].get(
                'windowOrientation', 9001)
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

        elif(step.STEPNUM == 1):
            translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
                initialValue = math.Vector2.ZERO,
                finalValue = math.Vector2(0, self._distance),
                initialRate = self.stateEstimator.getEstimatedVelocity(),
                avgRate = self._speed)
            translateMotion = motion.basic.Translate(
                trajectory = translateTrajectory,
                frame = Frame.LOCAL)
            
            self.motionManager.setMotion(translateMotion)
            
        else:
            self.publish(ReAlign.DONE, core.Event())

        self.STEPNUM += 1

class Center(state.State):
    
    CENTERED = core.declareEventType('CENTERED')
    TIMEOUT = core.declareEventType('TIMEOUT')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : Center ,
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

    def WINDOW_FOUND(self, event):
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
    DONE = core.declareEventType('NONE_LEFT')
    
    @staticmethod
    def transitions():
        return { vision.EventType.WINDOW_FOUND : Fire ,
                 Fire.FIRED : MoveOver ,
                 Fire.DONE : End }

    def WINDOW_FOUND(self, event):
        if event.color == vision.Color.ColorType.BLUE:
            self.vehicle.fireTorpedo(1)
            self.publish(Fire.FIRED, core.Event())
        elif event.color == vision.Color.ColorType.RED:
            self.vehicle.fireTorpedo(2)
            self.publish(Fire.DONE, core.Event())

class MoveOver(state.State):

    DONE = core.declareEventType('DONE')
    
    STEPNUM = 0

    @staticmethod
    def getattr():
        return { 'speed' : 0.3 , 'diveRate' : 0.3 , 
                 'height' : 3 , 'distance' : 3 }

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
            self.publish(MoveOver.DONE, core.Event())

        self.STEPNUM += 1

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
