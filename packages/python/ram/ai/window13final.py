
import math as m
import ext.control as control
import ram.ai.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
import ram.ai.Utility as util
from ram.ai.Approach import HyperApproach
from ram.ai.Approach import DONE
from ram.motion.basic import Frame

LCOLOR = vision.Color.RED
RCOLOR = vision.Color.YELLOW

COMPLETE = core.declareEventType('COMPLETE')

class Start(util.FiniteState):
    def run(self):
        self.visionSystem.cupidDetectorOn()
    @staticmethod
    def transitions():
        return {util.DONE : initialBuffer}

class initialBuffer(state.State):

    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {initialBuffer.FINISHED : TMApproach}

    def enter(self):
        self.publish(initialBuffer.FINISHED, core.Event())

class TMApproach(HyperApproach):
    def TARGET_FOUND(self,event):
        if(event.color == vision.Color.UNKNOWN):#unknown is main structure
            print '-------'
            print 'xFunc ' + str(self.xFunc(event))
            print 'yFunc ' + str(self.yFunc(event))
            print 'zFunc ' + str(self.zFunc(event))
            print 'X ' + str(event.x)
            print 'Y ' + str(event.y)
            print 'Range ' + str(event.range)
            self.run(event)
    @staticmethod
    def transitions():
        return {DONE : bufferOne, vision.EventType.TARGET_FOUND : TMApproach}

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .4 , 'kz' : .45, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.05 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

#I'm transition consumer RAWR
class bufferOne(state.State):
    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {bufferOne.FINISHED : TMApproachLL1}

    def enter(self):
        self.publish(bufferOne.FINISHED, core.Event())

#left handed variant
class TMApproachSL2(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 10 , 'ky' : .15 , 'kz' : .35, 'x_d' : -.15, 'r_d' : 200 , 'y_d' : -.07, 'x_bound': .05, 'r_bound': 10, 'y_bound':.05 ,'minvx': .05, 'minvy': .05 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if event.color == LCOLOR:
            if event.smallflag == True:
                print 'Centering on small hole...'
                event.x = event.smallx
                event.y = event.smally
                event.range = event.rangesmall
            elif event.largeflag == True:
                print '404 SMALL HOLE NOT FOUND'
                event.x = event.largex
                event.y = event.largey
                event.range = event.rangelarge

            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)
            
            self.run(event)

    def DONE(self,event):
        self.vehicle.fireTorpedoIndex(1)
        print('I fired')
    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.TARGET_FOUND : TMApproachSL2}

#right handed variant
class TMApproachSR2(HyperApproach):

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : .15, 'r_d' : 200 , 'y_d' : -.07, 'x_bound': .05, 'r_bound': 10, 'y_bound':.05 ,'minvx': .05, 'minvy': .05 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        print 'I did something'
        if event.color == RCOLOR:
            if event.smallflag == True:
                print 'Centering on small hole...'
                event.x = event.smallx
                event.y = event.smally
                event.range = event.rangesmall
            elif event.largeflag == True:
                print '404 SMALL HOLE NOT FOUND'
                event.x = event.largex
                event.y = event.largey
                event.range = event.rangelarge

            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)
            
            self.run(event)

    #i fire
    def DONE(self,event):
        vehicle.fireTorpedoIndex(0)
        print 'I fired'

    @staticmethod
    def transitions():
        return { DONE : state.State, vision.EventType.TARGET_FOUND : TMApproachSR2 }


class TMApproachLL1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if(event.color == LCOLOR and event.largeflag == True):

            event.x = event.largex
            event.y = event.largey
            event.range = event.rangelarge

            print 'xFunc ' + str(self.xFunc(event))
            print 'yFunc ' + str(self.yFunc(event))
            print 'zFunc ' + str(self.zFunc(event))
            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)

            self.run(event)
    @staticmethod
    def transitions():
        return {DONE : bufferTwo, vision.EventType.TARGET_FOUND : TMApproachLL1}

#I'm transition consumer RAWR
class bufferTwo(state.State):
    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {bufferTwo.FINISHED : TMApproachLL2}

    def enter(self):
        self.publish(bufferTwo.FINISHED, core.Event())

class TMApproachLL2(HyperApproach):
    COMPLETE = core.declareEventType('COMPLETE')

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : -.158, 'r_d' : 200 , 'y_d' : -.07, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if(event.color == LCOLOR and event.largeflag == True):

            event.x = event.largex
            event.y = event.largey
            event.range = event.rangelarge

            print 'xFunc ' + str(self.xFunc(event))
            print 'yFunc ' + str(self.yFunc(event))
            print 'zFunc ' + str(self.zFunc(event))
            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)

            self.run(event)

    def DONE(self,event):
        print'IMA FIRING MY LASER'
        self.vehicle.fireTorpedoIndex(1)
        self.publish(TMApproachLL2.COMPLETE, core.Event())

    @staticmethod
    def transitions():
        return {DONE : TMApproachLL2, vision.EventType.TARGET_FOUND : TMApproachLL2, TMApproachLL2.COMPLETE : bufferThree}

#I'm transition consumer RAWR
class bufferThree(state.State):
    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {bufferThree.FINISHED : backup}

    def enter(self):
        self.publish(bufferThree.FINISHED, core.Event())

class TMApproachLR1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if(event.color == RCOLOR and event.largeflag == True):

            event.x = event.largex
            event.y = event.largey
            event.range = event.rangelarge

            print 'xFunc ' + str(self.xFunc(event))
            print 'yFunc ' + str(self.yFunc(event))
            print 'zFunc ' + str(self.zFunc(event))
            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)

            self.run(event)
    @staticmethod
    def transitions():
        return {DONE : bufferFive, vision.EventType.TARGET_FOUND : TMApproachLR1}

#I'm transition consumer RAWR
class bufferFive(state.State):
    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {bufferFive.FINISHED : TMApproachLR2}

    def enter(self):
        self.publish(bufferFive.FINISHED, core.Event())

class TMApproachLR2(HyperApproach):
    COMPLETE = core.declareEventType('COMPLETE')

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : .158, 'r_d' : 200 , 'y_d' : -.07, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if(event.color == RCOLOR and event.largeflag == True):

            event.x = event.largex
            event.y = event.largey
            event.range = event.rangelarge

            print 'xFunc ' + str(self.xFunc(event))
            print 'yFunc ' + str(self.yFunc(event))
            print 'zFunc ' + str(self.zFunc(event))
            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)

            self.run(event)

    def DONE(self,event):
        print'IMA FIRING MY LASER'
        self.vehicle.fireTorpedoIndex(0)
        self.publish(TMApproachLR2.COMPLETE, core.Event())

    @staticmethod
    def transitions():
        return {DONE : TMApproachLR2, vision.EventType.TARGET_FOUND : TMApproachLR2, TMApproachLR2.COMPLETE : End}


class backup(util.MotionState):
    def enter(self):
        self.translate(0,-1,0.3)
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : backup,
                 util.DONE : bufferFour }

    def exit(self):
        util.freeze(self)
        self.motionManager.stopCurrentMotion()

#I'm transition consumer RAWR
class bufferFour(state.State):
    FINISHED = core.declareEventType('FINISHED')

    @staticmethod
    def transitions():
        return {bufferFour.FINISHED : TMApproachLR1}

    def enter(self):
        self.publish(bufferFour.FINISHED, core.Event())

#NOTE: currently attached to right TMA approach
class TMApproachSL1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 15, 'y_bound':.075 ,'minvx': .05, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if event.color == LCOLOR:
            if event.smallflag == True:
                print 'Centering on small hole...'
                event.x = event.smallx
                event.y = event.smally
                event.range = event.rangesmall
            elif event.largeflag == True:
                print '404 SMALL HOLE NOT FOUND'
                event.x = event.largex
                event.y = event.largey
                event.range = event.rangelarge

            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)
            
            self.run(event)
            
    @staticmethod
    def transitions():
        return {DONE : TMApproachSL2, vision.EventType.TARGET_FOUND : TMApproachSL1}

class TMApproachSR1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 15, 'y_bound':.075 ,'minvx': .05, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if event.color == RCOLOR:
            if event.smallflag == True:
                print 'Centering on small hole...'
                event.x = event.smallx
                event.y = event.smally
                event.range = event.rangesmall
            elif event.largeflag == True:
                print '404 SMALL HOLE NOT FOUND'
                event.x = event.largex
                event.y = event.largey
                event.range = event.rangelarge

            print 'X: ' + str(event.x)
            print 'Y: ' + str(event.y)
            print 'Range: ' + str(event.range)
            
            self.run(event)
            
    @staticmethod
    def transitions():
        return {DONE : TMApproachSR2, vision.EventType.TARGET_FOUND : TMApproachSR1}

class End(state.State):
    def enter(self):
        self.visionSystem.cupidDetectorOff()
        self.publish(COMPLETE, core.Event())
