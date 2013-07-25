
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

class Start(util.FiniteState):
    def run(self):
        self.visionSystem.cupidDetectorOn()
    @staticmethod
    def transitions():
        return {util.DONE : TMApproach}

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
        return {DONE : TMApproachLL1, vision.EventType.TARGET_FOUND : TMApproach}

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .4 , 'kz' : .45, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.05 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}


#left handed variant
class TMApproachSL2(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 10 , 'ky' : .15 , 'kz' : .35, 'x_d' : -.15, 'r_d' : 85 , 'y_d' : -.07, 'x_bound': .05, 'r_bound': 10, 'y_bound':.05 ,'minvx': .05, 'minvy': .05 ,'minvz' : .1}

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
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : .15, 'r_d' : 85 , 'y_d' : -.07, 'x_bound': .05, 'r_bound': 10, 'y_bound':.05 ,'minvx': .05, 'minvy': .05 ,'minvz' : .1}

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
        vehicle.fireTorpedoIndex(2)
        print 'I fired'

    @staticmethod
    def transitions():
        return { DONE : state.State, vision.EventType.TARGET_FOUND : TMApproachSR2 }


class TMApproachLL1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : 0, 'r_d' : 160 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

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
        return {DONE : TMApproachLL2, vision.EventType.TARGET_FOUND : TMApproachLL1}

class TMApproachLL2(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : -.158, 'r_d' : 160 , 'y_d' : -.07, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

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

    @staticmethod
    def transitions():
        return {DONE : backup, vision.EventType.TARGET_FOUND : TMApproachLL2}



class TMApproachLR1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : 0, 'r_d' : 160 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

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
        return {DONE : TMApproachLR2, vision.EventType.TARGET_FOUND : TMApproachLR1}

class TMApproachLR2(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : .158, 'r_d' : 160 , 'y_d' : -.07, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

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
        self.vehicle.fireTorpedoIndex(2)

    @staticmethod
    def transitions():
        return {DONE : End, vision.EventType.TARGET_FOUND : TMApproachLR2}


class backup(util.MotionState):
    def enter(self):
        self.translate(0,-.85,.3)
    @staticmethod
    def transitions():
        return { motion.basic.MotionManager.FINISHED : backup,
                 util.DONE : TMApproachLR1 }

    def exit(self):
        util.freeze(self)
        self.motionManager.stopCurrentMotion()


#NOTE: currently attached to right TMA approach
class TMApproachSL1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : 0, 'r_d' : 85 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 15, 'y_bound':.075 ,'minvx': .05, 'minvy': .1 ,'minvz' : .1}

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
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : 0, 'r_d' : 85 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 15, 'y_bound':.075 ,'minvx': .05, 'minvy': .1 ,'minvz' : .1}

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
