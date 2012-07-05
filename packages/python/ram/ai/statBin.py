# Copyright (C) 2012 Maryland Robotics Club
# Copyright (C) 2012 Gary Sullivan <gsulliva@umd.edu>
# All rights reserved.
#
# Author: Gary Sullivan gsullivan@umd.edu>
# File:  packages/python/ram/ai/statBin.py

# Standard imports
import math as pmath
import string as pstring

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.ai.tracking as tracking
import ram.ai.state as state
import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

COMPLETE = core.declareEventType('COMPLETE')

def ensureBinTracking(qeventHub, ai):        
    tracking.ensureItemTracking(qeventHub, ai, 'binData',
                                vision.EventType.BIN_FOUND,
                                vision.EventType.BIN_DROPPED)
    ai.data['binData'].setdefault('histogram', {})

class Start(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search }

    @staticmethod
    def getattr():
        return { 'diveRate' : 0.3 , 'speed' : 0.3 }

    def enter(self):
        self.visionSystem.binDetectorOn()

        # Make sure we are tracking
        ensureBinTracking(self.queuedEventHub, self.ai)


        binDepth = self.ai.data['config'].get('binDepth', 10)

        self.ai.data['binData'] = {}
        self.ai.data['ignoreSymbol'] = {}
        for symbol in self.ai.data['symbolList']:
            self.ai.data['binData'][symbol.lower()] = []
            self.ai.data[symbol.lower() + 'FoundNum'] = 0
            self.ai.data['ignoreSymbol'][symbol.lower()] = False

        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = binDepth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = self._diveRate)

        # Dive yaw and translate
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        
        self.motionManager.setMotion(diveMotion)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class Search(state.ZigZag):
    DONE = core.declareEventType('DONE')

    foundBins = 0

    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : Search , 
                 Search.DONE : Strafe,
                 state.ZigZag.DONE : End }

    def BIN_FOUND(self, event):
        self.foundBins = self.foundBins + 1
        if self.foundBins >= 10:
            self.publish(Search.DONE, core.Event())
            


class Strafe(state.State):

    NONE_LEFT = core.declareEventType('NONE_LEFT')
    DONE = core.declareEventType('DONE')
    SYMBOL_FOUND = core.declareEventType('SYMBOL_FOUND')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Strafe ,
                 vision.EventType.BIN_FOUND : Strafe ,
                 Strafe.SYMBOL_FOUND : Center ,
                 Strafe.DONE : Align,
                 Strafe.NONE_LEFT : End}

    @staticmethod
    def getattr():
        return { 'distanceLeft' : 3 , 'distanceRight' : 3,  'speed' : 0.15,
                 'foundMin' : 10}

    def enter(self):
        
        self.ignoreCount = 0

        for symbol in self.ai.data['ignoreSymbol']:
            if self.ai.data['ignoreSymbol'][symbol] == True:
                self.ignoreCount += 1

        if self.ignoreCount == len(self.ai.data['ignoreSymbol']):
            self.publish(Strafe.NONE_LEFT, core.Event())
            

        self._orientation = self.ai.data['binOrientation']
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(
                math.Degree(self._orientation), math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.motionManager.setMotion(yawMotion)

        self.STEPNUM = 0

    def exit(self):
        self.motionManager.stopCurrentMotion()

    def FINISHED(self, event):
        self.nextStep()

    def BIN_FOUND(self, event):
        symbol = str(event.symbol).lower()
        if(symbol in self.ai.data['binData']):
            self.ai.data['binData'][symbol].append(
                self.stateEstimator.getEstimatedPosition())
            self.ai.data[symbol + 'FoundNum'] += 1
            if(self.ai.data[symbol + 'FoundNum'] >= self._foundMin and
               not self.ai.data['ignoreSymbol'][symbol]):
                self.ai.data['binSymbol'] = symbol
                self.publish(Strafe.SYMBOL_FOUND, core.Event())


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

    def nextStep(self):
        #print("Step #: " + str(self.STEPNUM))
        if ( self.STEPNUM == 0):
            self.move(self._distanceRight)

        elif ( self.STEPNUM == 1 ):
            self.move(-self._distanceRight - self._distanceLeft)
        elif ( self.STEPNUM == 2 ):
            self.move(self._distanceLeft)
        else: 
            self.publish(Strafe.DONE, core.Event())

        self.STEPNUM += 1
        
        
class Align(state.State):

    NONE_LEFT = core.declareEventType('NONE_LEFT')
    ALIGNED = core.declareEventType('ALIGNED')

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Align ,
                 Align.NONE_LEFT : End , 
                 Align.ALIGNED : Center} 

    @staticmethod
    def getattr():
        return { 'speed' : 0.15 }

    def enter(self):
        if( len(self.ai.data['symbolList']) == 0 ):
            #print('All buoys hit')
            self.publish(Align.NONE_LEFT, core.Event())
            return
        
        symbol = self.ai.data['symbolList'].pop(0).lower()
        ignoreSymbol = self.ai.data['ignoreSymbol'][symbol]
        while ignoreSymbol:
            if( len(self.ai.data['symbolList']) == 0 ):
                #print('All buoys hit')
                self.publish(Align.NONE_LEFT, core.Event())
                return
            symbol = self.ai.data['symbolList'].pop(0).lower()
            ignoreSymbol = self.ai.data['ignoreSymbol'][symbol]

        #print('Aligning with ' + color + ' buoy')
        self.ai.data['binSymbol'] = symbol;
        count = 0
        x_total = 0
        y_total = 0
        for pos in self.ai.data['binData'][symbol]:
            count += 1
            x_total += pos.x
            y_total += pos.y
            
        x_avg = x_total / count;
        y_avg = y_total / count;
            
        #print(str(count) + ' entries in data')
        #print('X Average: ' + str(x_avg))
        #print('Y Average: ' + str(y_avg))

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedPosition(),
            finalValue = math.Vector2(x_avg, y_avg),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.GLOBAL)
        
        self.motionManager.setMotion(translateMotion)


    def FINISHED(self, event):
        self.publish(Align.ALIGNED, core.Event())


class Center(state.State):
    
    CENTERED = core.declareEventType('CENTERED')
    TIMEOUT = core.declareEventType('TIMEOUT')

    STEPNUM = 0

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Center,
                 vision.EventType.BIN_FOUND : Center ,
                 Center.CENTERED : Drop , 
                 Center.TIMEOUT : Drop }

    @staticmethod
    def getattr():
        return { 'speed' : 0.2 , 'diveRate' : 0.2 , 'distance' : 1 ,
                 'xmin' : -0.05 , 'xmax' : 0.05 , 
                 'ymin' : -0.05 , 'ymax' : 0.05,
                 'timeout' : 20}

    def movex(self, distance):
        if not self.move_again:
            return

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(0, distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

        self.move_again = False

    def movey(self, distance):
        if not self.move_again:
            return

        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(-distance,0),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = self._speed)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)

        self.motionManager.setMotion(translateMotion)

        self.move_again = False

    def enter(self):
        self.STEPNUM = 0

        self.move_again = True

        self.timer = self.timerManager.newTimer(Center.TIMEOUT, self._timeout)
        self.timer.start()

    def exit(self):
        if self.timer is not None:
            self.timer.stop()
        self.motionManager.stopCurrentMotion()

    def FINISHED(self, event):
        self.move_again = True

    def BIN_FOUND(self, event):
        if(str(event.symbol).lower() != self.ai.data['binSymbol']):
            #print 'returng ' + str(event.symbol).lower() + 'dumbfuck ' + self.ai.data['binSymbol']
            return

        if(self.STEPNUM == 0):
            #print("Buoy X: " + str(event.x))
            if(event.x <= self._xmin):
                #print("Moving left to compensate")
                self.movex(-self._distance)
            elif(event.x >= self._xmax):
                #print("Moving right to compensate")
                self.movex(self._distance)

            self.STEPNUM += 1
        
        elif(self.STEPNUM == 1):
            if(event.x > self._xmin and event.x < self._xmax):
                #print("X Axis Aligned")
                self.motionManager.stopCurrentMotion()
                self.move_again = True
                self.STEPNUM += 1
            else:
                self.STEPNUM -= 1

        elif(self.STEPNUM == 2):
            if(event.y <= self._ymin):
                self.movey(self._distance)
            elif(event.y >= self._ymax):
                self.movey(-self._distance)

            self.STEPNUM += 1

        elif(self.STEPNUM == 3):
            if(event.y > self._ymin and event.y < self._ymax):
                #print("Y Axis Aligned, All Done")
                self.motionManager.stopCurrentMotion()
                self.publish(Center.CENTERED, core.Event())
            else:
                self.STEPNUM -= 1


class Drop(state.State):

    DROPPED = core.declareEventType('DROPPED')

    @staticmethod
    def transitions():
        return { Drop.DROPPED : Strafe }

    def enter(self):
        # Release the marker
        self.vehicle.dropMarker();
        self.ai.data['ignoreSymbol'][self.ai.data['binSymbol']] = True
        self.publish(Drop.DROPPED, core.Event())

class End(state.State):
    def enter(self):
        #self.vehicle.dropMarkerIndex(1)
        #self.vehicle.dropMarkerIndex(0)
        self.publish(COMPLETE, core.Event())

