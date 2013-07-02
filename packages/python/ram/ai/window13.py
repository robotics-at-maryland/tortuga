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
import ram.ai.Utility as util
import ram.ai.Approach as vs

COMPLETE = core.declareEventType('COMPLETE')
REDT = core.declareEventType('REDT')
BLUET = core.declareEventType('BLUET')
GREENT = core.declareEventType('GREENT')
YELLOWT = core.declareEventType('YELLOWT')


#convention, window is the entire object, target is the individual cut-outs
#global variables are used here, but it should be noted that python's globals
#have some useful properties
#these will not be modified by any local assignments done and furthermore
#can only be read from if a local assignment hasn't been done

class Start(state.State):

    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : SearchWindow }

    @staticmethod
    def getattr():
        return { 'diveRate' : 0.3 , 'speed' : 0.3 }

    def enter(self):
        self.visionSystem.buoyDetectorOn()#change to torpedo once ready
        #initialize global variables for this set of states
        #set the two window target events(may require some additional work)
        #this is an appropriate use of global variables, these will only ever be read
        global WindowTarget1
        WindowTarget1 = util.getConfig(self,'Target1',-1)
        global WindowTarget2
        WindowTarget2 = util.getConfig(self,'Target2',-1)

        #some additional parsing likely required
        

        windowDepth = util.getConfig(self,'windowDepth', -1)

        if windowDepth == -1:
            raise LookupError, "windowDepth not specified"

        #dive to depth for this task
        util.dive(self,windowDepth,self._diveRate)
        
    def exit(self):
        self.motionManager.stopCurrentMotion()

#starting state for window search, handles initializations
class SearchWindow(util.FiniteState):

    def run(self):
        #define variables for counting how many windows have been seen
        #the default is 1 window(which is just seeing the window)
        global WindowSeenCount
        global totalWindowSeen
        totalWindowSeen = 0
        WindowSeenCount = util.getConfig(self,'WindowSeenCount',1)

    @staticmethod
    def transitions():
        return {util.FiniteState.DONE : SearchWindowZigZag}

class SearchWindowZigZag(state.ZigZag):

    #replace event when able to!
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : SearchWindowZigZag ,
                 vision.EventType.BUOY_FOUND : SearchWindowCountUpdate , 
                 state.ZigZag.DONE : End }


class SearchWindowCountUpdate(util.FiniteState):
    NEXT = core.declareEventType('NEXT')
    def run(self):
        global totalWindowSeen
        totalWindowSeen = totalWindowSeen + 1
        if totalWindowSeen >= WindowSeenCount:
            self.publish(NEXT,core.Event())
    @staticmethod
    def transitions():
        return{ SearchWindowCountUpdate.NEXT : SearchWindowDone, util.FiniteState.DONE : SearchWindowZigZag }

class SearchWindowDone(util.FiniteState):

    def run(self):
        global totalWindowSeen
        totalWindowSeen = 0
    @staticmethod
    def transitions():
        return{ util.FiniteState.DONE : ApproachWindow}

class ApproachWindow(vs.ForwardXYCenter):
    def WINDOW_FOUND(self,event):
        self.run(event)
    @staticmethod
    def transitions():
        {vs.DONE : AlignWindow, vision.EventType.WINDOW_FOUND : ApproachWindow}

class AlignWindow(util.MotionState):
    DONE = core.declareEventType('DONE')
    def enter():
        windowOrientation = self.ai.data['config'].get('windowOrientation', 9001)
        if windowOrientation > 9000:
            raise LookupError, "windowOrientation not specified"
        self.yawGlobal(windowOrientation,3)
    @staticmethod
    def transitions():
        {AlignWindow.DONE : SetupDecideTarget}

class SetupDecideTarget(util.FiniteState):
    def run(self):
        global NUMTARGS
        NUMTARGS = 2
        global T1
        T1 = util.getConfig(self,'Target 1', 'Swordfish')#naturally swordfish is not an actual color, but irregardless
        global T1
        T2 = util.getConfig(self,'Target 2', 'Swordfish')#naturally swordfish is not an actual color, but irregardless
        
        @staticmethod
    def transitions():
        {util.DONE : DecideTarget}

class DecideTarget(state.State):
    def enter(self):
        global T1
        global T2
        global NUMTARGS
        global LR
        LR = 'LEFT'
        if(NUMTARGS == 2):
            ctarg = T1
        if(NUMTARGS == 1):
            ctarg = T2
        #decide the color to go after, later on add appropriate vision system calls
        #to turn on the correct detector
        if(ctarg == 'Red'):
            self.publish(REDT,core.Event())
        if(ctarg == 'Blue'):
            self.publish(BLUET,core.Event()) 
        if(ctarg == 'Green'):
            self.publish(GREENT,core.Event())      
        if(ctarg == 'Yellow'):
            self.publish(YELLOWT,core.Event())    
    @staticmethod
    def transitions():
        {REDT : RedTarget, BLUET : BlueTarget, GREENT : GreenTarget}

class RedTarget(Approach.XZCenter):
    def enter(self):
        pass

    def RED_TARGET_FOUND(self, event):
        self.run(event)
    
    @staticmethod
    def transitions():
        return {vision.RED_TARGET_FOUND : RedTarget, vs.DONE : RedAim}

class RedAim(util.MotionState):
    def enter(self):
        pass#put motion here!
    
    @staticmethod
    def transitions:
        {util.DONE : FIRE}

#FIRE THE TORPEDOES!
class FIRE(util.FiniteState):
    def enter(self):
        global NUMTARGS
        self.vehicle.FireTorpedo(NUMTARGS)#HOW DO I SHOT TORPEDO
    
    

class CenterWindow(state.Center):
    #still working on this
    #may want to move forwards a bit before
    #going to this state
    #need to talk with steven about structure of these states
    @staticmethod
    def transitions():
        #replace with correct event type
        return{ vision.EventType.WINDOW_FOUND : CenterWindow, Center.CENTERED : ApproachWindow}

    def BUOY_FOUND(self,event):
        self.Update(event)

        

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

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
