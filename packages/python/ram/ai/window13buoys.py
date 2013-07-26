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
        global T2
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
        {REDT : RedTarget, BLUET : BlueTarget, GREENT : GreenTarget, YELLOWT : YellowTarget}

class RedTarget(vs.SuperApproach):#vs.XZCenter):

    #@staticmethod
    #def getattr():
    #    return { 'fDisp' : .1 ,  'sDisp' : .2 ,
    #             'xmin' : -0.05 , 'xmax' : 0.05, 'zmin' : -0.05 , 'zmax' : 0.05, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, } 
    @staticmethod
    def getattr():
        return { 'dy' : .1 ,  'dx' : .1 , 'dz' : .1,
                 'xmin' : -0.05 , 'xmax' : 0.05, 'zmin' : -0.05 , 'zmax' : 0.05, 'rmin' : 1.5 , 'rmax' : 1.75, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, } 
    
    def enter(self):
        pass

    #def RED_TARGET_FOUND(self, event):
    def BUOY_FOUND(self,event):
        if(event.color == vision.Color.YELLOW):
            self.run(event)
    
    @staticmethod
    def transitions():
        return {vision.EventType.BUOY_FOUND : RedTarget, vs.DONE : RedApproach}

#    def end_cond(self,event):
#        return ((self.decideZ(event) == 0) and (self.decideX(event) == 0) )

class RedApproach(vs.ForwardXYCenter):
    def enter(self):
        pass

    @staticmethod
    def getattr():
        return { 'fDisp' : .05 ,  'sDisp' : .1 ,
                 'xmin' : -0.05 , 'xmax' : 0.05, 'rmin' : 1.5 , 'rmax' : 1.75, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, } 

    #def RED_TARGET_FOUND(self, event):
    def BUOY_FOUND(self,event):
        if(event.color == vision.Color.YELLOW):
            self.run(event)
    
    @staticmethod
    def transitions():
        return {vision.EventType.BUOY_FOUND : RedApproach, vs.DONE : RedLockOn}

    def end_cond(self,event):
        return ((self.decideY(event) == 0) and (self.decideX(event) == 0) )

class RedLockOn(vs.XZCenter):

    @staticmethod
    def getattr():
        return { 'fDisp' : .05 ,  'sDisp' : .05 ,
                 'xmin' : -0.025 , 'xmax' : 0.025, 'zmin' : -0.025 , 'zmax' : 0.025, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, } 

    def enter(self):
        pass

    #def RED_TARGET_FOUND(self, event):
    def BUOY_FOUND(self,event):
        if(event.color == vision.Color.YELLOW):
            self.run(event)
    
    @staticmethod
    def transitions():
        return {vision.EventType.BUOY_FOUND : RedLockOn, vs.DONE : FIRE}

    def end_cond(self,event):
        return ((self.decideZ(event) == 0) and (self.decideX(event) == 0) )



class RedAim(util.MotionState):
    def enter(self):
        self.translate(0,0,.1)#put motion here!
    
    @staticmethod
    def transitions():
        return {util.DONE : FIRE}

#FIRE THE TORPEDOES!
class FIRE(util.FiniteState):
    def run(self):
        global NUMTARGS
        self.vehicle.fireTorpedoIndex(2)#HOW DO I SHOT TORPEDO

    @staticmethod
    def transitions():
        return {util.DONE : End}
    
    

class End(state.State):
    def enter(self):
        self.publish(COMPLETE, core.Event())
