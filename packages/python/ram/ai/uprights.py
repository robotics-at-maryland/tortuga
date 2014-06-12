# Copyright (C) 2012 Maryland Robotics Club
# Copyright (C) 2012 Stephen Christian <schrist2@umd.edu>
# 
# All rights reserved.
#
# Author: Stephen Christian <schrist2@terpmail.umd.edu>
# Edited 2013 Johnny Mao <jmao@umd.edu>
# File:  packages/python/ram/ai/uprights.py

# Standard imports
import math as pmath
import string as pstring

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math
import ext.estimation as estimation
from ext.control import yawVehicleHelper

import ram.motion as motion
import ram.timer
from ram.motion.basic import Frame

import ram.ai.state as state
import ram.ai.Utility as utility
import ram.ai.Approach as approach

COMPLETE = core.declareEventType('COMPLETE')
CALLBACK1 = core.declareEventType('CALLBACK1')

# global vars ------
global startDepth # task starting depth
global desiredOrientation # desired direction to be pointed in for passing through the gate
global timeLimit # time limit for yaw motion
global strafeTimeLimit # time limit before strafe start
global lastLocation # probably will be used to reacquire object if a lost event was given
global approachFinished # used to determine if past Approach or not
global strafeDistance
global strafeRate
global redStrafeTimeLimit
global greenStrafeTimeLimit
global reverseDistance
global reverseTimeLimit

redStrafeTimeLimit = 20
greenStrafeTimeLimit = 25
approachFinished = False
startDepth = 7
timeLimit = 10
desiredOrientation = 0
strafeDistance = 1.5
strafeRate = 0.15
# end of global vars
        

class Start(utility.MotionState):

    """ 
    Dive down to the starting depth for the task
    """

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Start,
                 utility.DONE : Search }

    @staticmethod
    def getattr():
        global startDepth
        return { 'depth' : startDepth, 'diveSpeed' : 0.15 }

    def enter(self):
        self.dive( self._depth, self._diveSpeed )

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

class Search(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Search,
                 utility.DONE : End,
                 vision.EventType.GATE_FOUND : UprightsApproach }

    @staticmethod
    def getattr():
        return { 'distance' : 8.5, 'speed' : 0.15 }

    def enter(self):
        self.translate(0, self._distance, self._speed)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

# Focus on the traffic light

class UprightsApproach(approach.HyperApproach):

    @staticmethod
    def transitions():
        return { vision.EventType.GATE_FOUND : UprightsApproach,
                 approach.DONE : Ready }

    @staticmethod
    def getattr():
         return { 'kx' : .05 ,  'ky' : .3 , 'kz' : .45, 
                  'x_d' : 0, 'r_d' : 40 , 'y_d' : 0, 
                  'x_bound': .05, 'r_bound': 10, 'y_bound':.025 ,
                  'minvx': .1, 'minvy': .1 ,'minvz' : .1 } 

    def enter(self):
        pass

    def end_cond(self, event):
        return ((self.decideX(event) == 0) and \
                    (self.decideY(event) == 0) and \
                    (self.decideZ(event) == 0))

    def GATE_FOUND(self, event):
        global lastLocation
        lastLocation = self.stateEstimator.getEstimatedPosition()
        #print '-----------'
        #print 'X pos: ' + str(event.x)
        #print 'Y pos: ' + str(event.y)
        #print 'Range: ' + str(event.range)
        self.run(event)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()


class Offset(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Offset,
                 utility.DONE : StartYaw }

    @staticmethod
    def getattr():
        return { 'distance' : -1.5, 'speed' : 0.15 }

    def enter(self):
        currentDepth = self.stateEstimator.getEstimatedDepth()
        desiredDepth = currentDepth + self._distance
        self.dive(desiredDepth, self._speed)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class StartYaw(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : StartYaw,
                 utility.YAWED : StartYaw,
                 utility.DONE : Strafe }

    @staticmethod
    def getattr():
        return { 'yawAngle' : 90, 'time' : 15 }

    def enter(self):
        self.yaw(self._yawAngle, self._time)

    def exit(self):
        self.motionManager.stopCurrentMotion()

# this is the part that waits for the light to change ----

# class Ready(state.State):
    
#     RED = core.declareEventType('RED')
#     GREEN = core.declareEventType('GREEN')

#     @staticmethod
#     def transitions():
#         return { vision.EventType.BUOY_FOUND : Ready,
#                  Ready.RED : RedFirst,
#                  Ready.GREEN : GreenFirst }

#     def enter(self):
#         pass

#     def BUOY_FOUND(self, event):
#         if event.color == vision.Color.RED:
#             self.publish(Ready.RED, core.Event())
#         else:
#             self.publish(Ready.GREEN, core.Event())

#     def exit(self):
#         utility.freeze(self)
#         self.motionManager.stopCurrentMotion()

# # When it sees red first it waits for green

# class RedFirst(state.State):

#     YAW = core.declareEventType('YAW')

#     @staticmethod
#     def transitions():
#         return { vision.EventType.BUOY_FOUND : RedFirst,
#                  vision.EventType.BUOY_LOST : RedFirst,
#                  RedFirst.YAW : startYaw }

#     def enter(self):
#         global strafeTimeLimit
#         global redStrafeTimeLimit
#         strafeTimeLimit = redStrafeTimeLimit
#         pass

#     def BUOY_FOUND(self, event):
#         self.timer = self.timerManager.newTimer(RedFirst.YAW, 5)
#         self.timer.start()
#         if event.color == vision.Color.GREEN:
#             self.publish(YAW, core.Event())

#     def exit(self):
#         self.motionManager.stopCurrentMotion()

# # When it sees green first it waits for red

# class GreenFirst(state.State):

#     YAW = core.declareEventType('YAW')

#     @staticmethod
#     def transitions():
#         return { vision.EventType.BUOY_FOUND : GreenFirst,
#                  vision.EventType.BUOY_LOST : GreenFirst,
#                  RedFirst.YAW : startYaw }

#     def enter(self):
#         global strafeTimeLimit
#         global greenStrafeTimeLimit
#         strafeTimeLimit = greenStrafeTimeLimit
#         pass

#     def BUOY_FOUND(self, event):
#         if event.color == vision.Color.RED:
#             self.publish(YAW, core.Event())

#     def exit(self):
#         self.motionManager.stopCurrentMotion()

# # begin the yaw and timer

# class startYaw(utility.MotionState):

#     DONE = core.declareEventType('DONE')

#     @staticmethod
#     def transitions():
#        return { CALLBACK1 : startYaw,
#                  startYaw.DONE : strafe }

#     @staticmethod
#     def getattr():
#         global strafeTimeLimit
#         global reverseDistance
#         global reverseTimeLimit
#         global strafe
#         return { 'time1' : reverseTimeLimit, 'time2' : strafeTimeLimit, 'distance' : reverseDistance, 'speed' : strafeRate }

#     def enter(self):
#         global desiredOrientation
#         self.visionSystem.buoyDetectorOff()
#         self.yawGlobal((desiredOrientation + 90), self._time1)
#         self.timer2 = self.timerManager.newTimer(startYaw.DONE, self._time2)
#         self.timer1 = self.timerManager.newTimer(CALLBACK1, self._time1)
#         self.timer1.start()
#         self.timer2.start()

#     def CALLBACK1(self, event):
#         self.translate(0, self._distance, self._speed)

#     def exit(self):
#         utility.freeze(self)
#         self.motionManager.stopCurrentMotion()

# strafing motion

class strafe(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : strafe,
                 utility.DONE : straighten }

    @staticmethod
    def getattr():
        global strafeDistance
        global strafeRate
        return { 'strafeDist' : strafeDistance, 'speed' : strafeRate }


    def enter(self):
        self.translate(self._strafeDist, 0, self._speed)

    def exit(self):
        utility.freeze(self)
        self.motionManager.stopCurrentMotion()

class straighten(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : straighten,
                 utility.YAWED : straighten,
                 utility.DONE : Rise }

    def enter(self):
        self.yaw(-90, 15)

    def exit(self):
        self.motionManager.stopCurrentMotion()

class Rise(utility.MotionState):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : Rise,
                 utility.DONE : End}

    def enter(self):
        self.dive(4, 0.15)

    def exit(self):
        self.motionManager.stopCurrentMotion()


class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
