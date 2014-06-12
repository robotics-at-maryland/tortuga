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
#DROPPED = core.declareEventType('DROPPED')

class Start(util.FiniteState):
    def run(self):
        self.visionSystem.downwardSafeDetectorOn()
        print 'vision ON'
        
    @staticmethod
    def transitions():
        return {util.DONE : Diving}

class firstBuffer(state.State):

    def enter(self):
        self.publish(firstBuffer.DONE, core.Event())

    @staticmethod
    def transitions():
        return { firstBuffer.DONE : Diving }

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
        return { 'kx' : .15 ,  'ky' : .4 , 'kz' : 9, 'x_d' : 0, 'r_d' : 175 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.05 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}   
    @staticmethod
    def transitions():
        return {vs.DONE : Shift, vision.EventType.BUOY_FOUND : Positioning}

    def enter(self):
        self.timer = self.timerManager.newTimer(vs.DONE, 180)
        self.timer.start()

    def exit(self):
        self.timer.stop()

class Shift(util.MotionState):
    def enter(self):
        self.translate(-.18,0,.1)#FILL ME IN LATER WITH APPROPRIATE VALUES

    @staticmethod
    def transitions():
        return  {util.DONE : buffering, motion.basic.MotionManager.FINISHED : Shift}

class buffering(state.State):
    
    TRANS = core.declareEventType('TRANS')

    def enter(self):
        self.publish(buffering.TRANS, core.Event())

    @staticmethod
    def transitions():
        return { buffering.TRANS : Grabbing }

class Grabbing(util.MotionState):
    def enter(self):
        self.dive(self.stateEstimator.getEstimatedDepth()+5.5,.1)#FILL ME IN LATER
        self.timer = self.timerManager.newTimer(util.DONE, 90)
        self.timer.start()

    def exit(self):
        self.timer.stop()
        self.motionManager.stopCurrentMotion()
        
    @staticmethod
    def transitions():
        return {util.DONE : ROCK, motion.basic.MotionManager.FINISHED : Grabbing
}

#yes i know these states violate our conventions, but this was too good an excuse not to use these names
class ROCK(state.State):

    DROPPED = core.declareEventType('DROPPED')

    def enter(self):
        self.controller.rollVehicle(2,1);
        self.timer = self.timerManager.newTimer(ROCK.DROPPED, 2.5)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {ROCK.DROPPED : ROLL}

class ROLL(state.State):
    DROPPED = core.declareEventType('DROPPED')

    def enter(self):
        self.controller.rollVehicle(-4,1);
        self.timer = self.timerManager.newTimer(ROLL.DROPPED, 2.5)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {ROLL.DROPPED : Stabilize}

#there was an extreme temptation to give this state an unclear name(hint: the other two are synonyms for rotating about our X axis)
class Stabilize(state.State):
    DROPPED = core.declareEventType('DROPPED')

    def enter(self):
        self.controller.rollVehicle(2,1);
        self.timer = self.timerManager.newTimer(Stabilize.DROPPED, 7.5)#DO WE NEED A LONGER WAIT, sure why not(definitely here)
        self.timer.start()
    @staticmethod
    def transitions():
        return {Stabilize.DROPPED : Up}

#go a bit up away from the stand
class Up(util.MotionState):
    def enter(self):
        self.dive(self.stateEstimator.getEstimatedDepth()-5.5,.1)#FILL ME IN LATER,
        self.timer = self.timerManager.newTimer(util.DONE, 90)
        self.timer.start()

    def exit(self):
        self.timer.stop()
        self.motionManager.stopCurrentMotion()

    @staticmethod
    def transitions():
        return {util.DONE : anotherBuffer, motion.basic.MotionManager.FINISHED : Up}

class anotherBuffer(state.State):

    TRANS = core.declareEventType('TRANS')

    def enter(self):
        self.publish(anotherBuffer.TRANS, core.Event())

    @staticmethod
    def transitions():
        return { anotherBuffer.TRANS : Unshift }

class Unshift(util.MotionState):
    def enter(self):
        self.translate(.18,0,.1)#FILL ME IN LATER
    @staticmethod
    def transitions():
        return {util.DONE : YAB, motion.basic.MotionManager.FINISHED : Unshift}

class YAB(state.State):

    TRANS = core.declareEventType('TRANS')

    def enter(self):
        self.publish(YAB.TRANS, core.Event())

    @staticmethod
    def transitions():
        return { YAB.TRANS : Surface }

class Surface(util.MotionState):
    def enter(self):
        self.dive(0,.1)#THE SURFACE IS PROBABLY NOT 0! BE SURE TO CHANGE(eh, it more or less is)
        self.timer = self.timerManager.newTimer(util.DONE, 120)
        self.timer.start()

    def exit(self):
        self.timer.stop()
        self.motionManager.stopCurrentMotion()

    @staticmethod
    def transitions():
        return  {util.DONE : Redive, motion.basic.MotionManager.FINISHED : Surface}

class MAO(state.State):
    DONE = core.declareEventType('DONE')

    def enter(self):
        self.publish(MAO.DONE, core.Event())

    def exit(self):
        pass

    @staticmethod
    def transitions():
        return { MAO.DONE : Redive }

#go a bit lower in order to release the object
class Redive(util.MotionState):
    def enter(self):
        self.dive(3.5,.1)#FILL ME IN LATER, (it should be about 3.5)
 
    @staticmethod
    def transitions():
        return  {util.DONE : Dropping, motion.basic.MotionManager.FINISHED : Redive}

class Dropping(state.State):
    DROPPED = core.declareEventType('DROPPED')

    def enter(self):
        #HOW DO I DROP OBJECT?(excellent question!)
        self.vehicle.releaseGrabber()
        self.timer = self.timerManager.newTimer(Dropping.DROPPED, 15)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {Dropping.DROPPED : Dropping2}

class Dropping2(state.State):
    DROPPED = core.declareEventType('DROPPED')

    def enter(self):
        #HOW DO I DROP OBJECT?(excellent question!)
        self.vehicle.releaseGrabber()
        self.timer = self.timerManager.newTimer(Dropping2.DROPPED, 15)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {Dropping2.DROPPED : Dropping3}

class Dropping3(state.State):
    DROPPED = core.declareEventType('DROPPED')
    
    def enter(self):
        #HOW DO I DROP OBJECT?(excellent question!)
        self.vehicle.releaseGrabber()
        self.timer = self.timerManager.newTimer(Dropping3.DROPPED, 15)#DO WE NEED A LONGER WAIT, sure why not
        self.timer.start()
    @staticmethod
    def transitions():
        return {Dropping3.DROPPED : End}
        
#Segment complete
class End(state.State):
    def enter(self):
        self.visionSystem.downwardSafeDetectorOff()
        self.publish(COMPLETE, core.Event())



    
    

        
