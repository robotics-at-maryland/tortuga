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
DROPPED = core.declareEventType('DROPPED')

class Start(util.FiniteState):
    def run(self):
        self.visionSystem.downwardSafeDetectorOn()
    @staticmethod
    def transitions():
        return {util.DONE : Diving}

class Diving(util.MotionState):
    def enter(self):
        self.dive(self.stateEstimator.getEstimatedDepth(),.1)
    @staticmethod
    def transitions():
        return {util.DONE : Positioning, motion.basic.MotionManager.FINISHED : Diving}

#default variant of this approach is for the buoy
class Positioning(vs.DHyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : .15 ,  'ky' : .4 , 'kz' : 9, 'x_d' : 0, 'r_d' : 50 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.025 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}   
    @staticmethod
    def transitions():
        return {vs.DONE : Shift, vision.EventType.BUOY_FOUND : Positioning}

class Shift(util.MotionState):
    def enter(self):
        self.translate(0,.18,.1)#FILL ME IN LATER WITH APPROPRIATE VALUES
    @staticmethod
    def transitions():
        return  {util.DONE : Grabbing, motion.basic.MotionManager.FINISHED : Shift}

class Grabbing(util.MotionState):
    def enter(self):
        self.dive(self.stateEstimator.getEstimatedDepth()+4.75,.1)#FILL ME IN LATER

    @staticmethod
    def transitions():
        return {util.DONE : Up, motion.basic.MotionManager.FINISHED : ROCK}

#yes i know these states violate our conventions, but this was too good an excuse not to use these names
class ROCK(state.State):
    def enter(self):
        self.controller.rollVehicle(2,.1);
        self.timer = self.timerManager.newTimer(DROPPED, 2.5)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {DROPPED : ROLL}

class ROLL(state.State):
    def enter(self):
        self.controller.rollVehicle(-2,.1);
        self.timer = self.timerManager.newTimer(DROPPED, 2.5)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {DROPPED : Stabilize}

#there was an extreme temptation to give this state an unclear name(hint: the other two are synonyms for rotating about our X axis)
class Stabilize(state.State):
    def enter(self):
        self.controller.rollVehicle(0,.1);
        self.timer = self.timerManager.newTimer(DROPPED, 7.5)#DO WE NEED A LONGER WAIT, sure why not(definitely here)
        self.timer.start()
    @staticmethod
    def transitions():
        return {DROPPED : Up}

#go a bit up away from the stand
class Up(util.MotionState):
    def enter(self):
        self.dive(self.stateEstimator.getEstimatedDepth()-4.75,.1)#FILL ME IN LATER, 

    @staticmethod
    def transitions():
        return {util.DONE : Unshift, motion.basic.MotionManager.FINISHED : Up}

class Unshift(util.MotionState):
    def enter(self):
        self.translate(0,-.18,.1)#FILL ME IN LATER
    @staticmethod
    def transitions():
        return  {util.DONE : Surface, motion.basic.MotionManager.FINISHED : Unshift}

class Surface(util.MotionState):
    def enter(self):
        self.dive(0,.1)#THE SURFACE IS PROBABLY NOT 0! BE SURE TO CHANGE(eh, it more or less is)
    @staticmethod
    def transitions():
        return  {util.DONE : Redive, motion.basic.MotionManager.FINISHED : Surface}

#go a bit lower in order to release the object
class Redive(util.MotionState):
    def enter(self):
        self.dive(3.5,.1)#FILL ME IN LATER, (it should be about 3.5)
    @staticmethod
    def transitions():
        return  {util.DONE : Dropping, motion.basic.MotionManager.FINISHED : Redive}

class Dropping(state.State):
    def enter(self):
        #HOW DO I DROP OBJECT?(excellent question!)
        self.vehicle.releaseGrabber()
        self.timer = self.timerManager.newTimer(DROPPED, 20)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {DROPPED : End}
        
#Segment complete
class End(state.State):
    def enter(self):
        self.visionSystem.downwardSafeDetectorOff()
        self.publish(COMPLETE, core.Event())



    
    

        
