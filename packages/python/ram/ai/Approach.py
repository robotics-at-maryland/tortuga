import math as m
import ext.control as control
import ram.ai.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
import ram.ai.Utility as util
from ram.motion.basic import Frame

DONE = core.declareEventType('DONE')

#general visual servoing motion class
class VSMotion(state.State):

    def xFunc(self, event):
        return 0

    def yFunc(self,event):
        return 0
    
    def zFunc(self,event):
        return 0

    def moveX(self,dirct,event):
        pass

    def moveY(self,dirct,event):
        pass

    def moveZ(self,dirct,event):
        pass

    def runMotion(self,event=0):
        pass

    def decideX(self,event):
        return 0

    def decideY(self,event):
        return 0

    def decideZ(self,event):
        return 0

    def stop(self):
        pass
    
    def end_cond(self,event):
        return True

    def run(self,event):
        if(self.end_cond(event) == True):
            self.stop()
            self.publish(DONE,core.Event())
        else:
            self.moveX(self.decideX(event),event)
            self.moveY(self.decideY(event),event)
            self.moveZ(self.decideZ(event),event)
            self.runMotion(event)
    
class VSDirect2Control(VSMotion):

    
    def runMotion(self,event):
        d = self.motionManager._estimator.getEstimatedDepth()
        pos = self.motionManager._estimator.getEstimatedPosition()
        if(self.decideZ(event) != 0):
            self.motionManager._controller.changeDepth(self._zDisp+d)
        #print pos
        self.motionManager._controller.translate(math.Vector2(self._yDisp+pos.x,self._xDisp+pos.y))
        
        
class VSDirect2ControlConst(VSDirect2Control):
    @staticmethod
 
    
    def xFunc(self,event):
        return self._cX
    def yFunc(self,event):
        return self._cY
    def zFunc(self,event):
        return self._cZ

class ForwardXYCenter(VSDirect2ControlConst):
    @staticmethod
    def getattr():
        return { 'fDisp' : .01 , 'sDisp' : .02 ,
                 'xmin' : -0.05 , 'xmax' : 0.05,'rmin' : 0.4 , 'rmax' : 0.5, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, }

    def decideX(self,event):
        if(event.x<self._xmin):
            return -1#go the other way
        else:
            if(event.x>self._xmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0
    def decideY(self,event):
        if(event.range<self._rmin):
            return -1#go the other way
        else:
            if(event.range>self._rmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0
    def xFunc(self,event):
        return self._sDisp

    def yFunc(self,event):
        return self._fDisp

    def moveX(self,dirct,event):
        self._xDisp = dirct*self.xFunc(event)

    def moveY(self,dirct,event):
        self._yDisp = dirct*self.yFunc(event)

    def DONE(self,event):
        util.freeze(self)
    





#import ram.ai.Approach as a
class genApproach(ForwardXYCenter):
    def enter(self):
        pass

    def BUOY_FOUND(self,event):
        self.run(event)

    @staticmethod
    def getattr():
        return { 'fDisp' : .1 , 'sDisp' : .2 ,
                 'xmin' : -0.035 , 'xmax' : 0.035,'rmin' : 4 , 'rmax' : 4.25, }

    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.BUOY_FOUND : genApproach}

    def end_cond(self,event):
        return ((self.decideY(event) == 0) and (self.decideX(event) == 0) )

    def DONE(self,event):
        util.freeze(self)
                                                



class XZCenter(VSDirect2ControlConst):
    @staticmethod
    def getattr():
        return { 'fDisp' : .1 , 'sDisp' : .2 ,
                 'xmin' : -0.05 , 'xmax' : 0.05, 'zmin' : -0.05 , 'zmax' : 0.05, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, }

    def decideX(self,event):
        if(event.x<self._xmin):
            return -1#go the other way
        else:
            if(event.x>self._xmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0
    def decideZ(self,event):
        if(event.y<self._zmin):
            return 1#go the other way
        else:
            if(event.y>self._zmax):
                return -1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def xFunc(self,event):
        return self._sDisp

    def zFunc(self,event):
        return self._fDisp

    def moveX(self,dirct,event):
        self._xDisp = dirct*self.xFunc(event)

    def moveZ(self,dirct,event):
        self._zDisp = dirct*self.zFunc(event)
    
    def DONE(self,event):
        util.freeze(self)

#A 3DOF approach
class SuperApproach(VSDirect2ControlConst):

    @staticmethod
    def getattr():
        return { 'dy' : .1 , 'dx' : .1 , 'dz' : .1,
                 'xmin' : -0.05 , 'xmax' : 0.05, 'zmin' : -0.05 , 'zmax' : 0.05, 'rmin' : 1.5 , 'rmax' : 1.75, 'xDisp' : 0, 'yDisp' : 0, 'zDisp' : 0, }
    
    def xFunc(self,event):
        return self._dx

    def yFunc(self,event):
        return self._dy

    def zFunc(self,event):
        return self._dz

    def moveX(self,dirct,event):
        self._xDisp = dirct*self.xFunc(event)

    def moveY(self,dirct,event):
        self._yDisp = dirct*self.yFunc(event)

    def moveZ(self,dirct,event):
        self._zDisp = dirct*self.zFunc(event)

    def decideX(self,event):
        if(event.x<self._xmin):
            return -1#go the other way
        else:
            if(event.x>self._xmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def decideY(self,event):
        if(event.range<self._rmin):
            return -1#go the other way
        else:
            if(event.range>self._rmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0


    def decideZ(self,event):
        if(event.y<self._zmin):
            return 1#go the other way
        else:
            if(event.y>self._zmax):
                return -1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def end_cond(self,event):
        return ((self.decideZ(event) == 0) and (self.decideX(event) == 0) and (self.decideY(event) == 0))

#A 3DOF Visual Servoing controller
class HyperApproach(VSMotion):
    @staticmethod
    def getattr():
        return { 'kx' : .05 , 'ky' : .4 , 'kz' : .45, 'x_d' : 0, 'r_d' : 1.75 , 'y_d' : 0, 'x_bound': .05, 'r_bound': .25, 'y_bound':.025 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}
#define error methods to return the current error
    def yFunc(self,event):
        a = self._ky*(event.x - self._x_d)
        if(abs(a)<self._minvy):
            a = m.copysign(self._minvy,a)
        return a

    def xFunc(self,event):
        a = self._kx*(1.0/event.range - 1.0/self._r_d)
        if(abs(a)<self._minvx):
            a = m.copysign(self._minvx,a)
        return a

    def zFunc(self,event):
        a = -self._kz*(event.y - self._y_d)
        if(abs(a)<self._minvz):
            a = m.copysign(self._minvz,a)
        return a

    def runMotion(self,event):
        self.motionManager._controller.moveVel(self.xFunc(event)*self.decideX(event), self.yFunc(event)*self.decideY(event), self.zFunc(event)*self.decideZ(event))
    
    def stop(self):
        print 'I should be stopping'
        self.motionManager._controller.moveVel(0,0,0)
        util.freeze(self)

    def decideY(self,event):
        if(event.x<(self._x_d - self._x_bound)):
            return 1#go the other way
        else:
            if(event.x>(self._x_d + self._x_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def decideX(self,event):
        if(event.range<(self._r_d - self._r_bound)):
            return 1#go the other way
        else:
            if(event.range>(self._r_d + self._r_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def decideZ(self,event):
        if(event.y<(self._y_d - self._y_bound)):
            return 1#go the other way
        else:
            if(event.y>(self._y_d + self._y_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def end_cond(self,event):
        return ((self.decideZ(event) == 0) and (self.decideX(event) == 0) and (self.decideY(event) == 0))
    #where are the move function definitions?
    #missing, because this variant has no need for such things, thus they aren't defined


                                                

   
class HyperApproachVConstrict(HyperApproach):
    def runMotion(self,event):
        #this variant moves the minimal speed possible unless
        #a speed is zero, allowing for theoretically very fast behaviour
        #while maintaining the image within the frame
        vx = self.xFunc(event)*self.decideX(event)
        vy = self.yFunc(event)*self.decideY(event)
        vz = self.zFunc(event)*self.decideZ(event)
        if(abs(vx) > abs(vy) and vy != 0):
            vx = m.copysign(vy,vx)
        if(abs(vy) > abs(vx) and vx != 0):
            vy = m.copysign(vx,vy)
        self.motionManager._controller.moveVel(vx, vy, vz)




class genHyperApproach(HyperApproachVConstrict):
    @staticmethod
    def getattr():
        return { 'kx' : .15 , 'ky' : .4 , 'kz' : .45, 'x_d' : 0, 'r_d' : 1.75 , 'y_d' : 0, 'x_bound': .05, 'r_bound': .25, 'y_bound':.025 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def enter(self):
        pass

    #redefined because the simulator cheats at ranges
    def xFunc(self,event):
        a = self._kx*(event.range - self._r_d)
        if(abs(a)<self._minvx):
            a = m.copysign(self._minvx,a)
        return a

    def BUOY_FOUND(self,event):
        if(event.color == vision.Color.YELLOW):
            self.run(event)

    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.BUOY_FOUND : genHyperApproach}

# def DONE(self,event):
# util.freeze(self)

class DHyperApproach(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : .25 , 'ky' : .25 , 'kz' : 7, 'x_d' : 0, 'r_d' : 115 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 20, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .3}
    
    def enter(self):
        pass

    def BUOY_FOUND(self,event):
        if(event.color == vision.Color.RED):
            #get rotation matrix from robot frame to intertial frame, then apply it to the x,y measurements
            #this rotation matrix must exclude yaw, this ends up looking a bit silly because of quaternion math
            #q1 = self.stateEstimator.getEstimatedOrientation()
            #v1 = math.Vector3(event.x,event.y,0)
            #now get the quaternion to unrotate our yaw, by unrotating by our level quaternion
            #qp = control.holdCurrentHeadingHelper(q1)
            #by rotating to the inertial frame, then rotating to the body yaw frame we acheive our goal
            #rotate coordinates from body frame to inertial frame
            #v1p = qp*(q1.UnitInverse()*v1)
            #event.x = v1p.x
            #event.y = v1p.y
            print "x:"+str(event.x)
            print "y:"+str(event.y)
            print "r:"+str(event.range)
            #print self.xFunc(event)
            #print self.yFunc(event)
            #print self.zFunc(event)
            print '----------'
            self.run(event)

    def yFunc(self,event):
        a = self._ky*(event.x - self._x_d)
        if(abs(a)<self._minvy):
            a = m.copysign(self._minvy,a)
        return a

    def zFunc(self,event):
        a = self._kz*(1.0/event.range - 1.0/self._r_d)
        if(abs(a)<self._minvz):
            a = m.copysign(self._minvz,a)
        return a

    def xFunc(self,event):
        a = self._kx*(event.y - self._y_d)
        if(abs(a)<self._minvy):
            a = m.copysign(self._minvx,a)
        return a


    def decideY(self,event):
        if(event.x<(self._x_d - self._x_bound)):
            return 1#go the other way
        else:
            if(event.x>(self._x_d + self._x_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def decideZ(self,event):
        if(event.range<(self._r_d - self._r_bound)):
            return 1#go the other way
        else:
            if(event.range>(self._r_d + self._r_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def decideX(self,event):
        if(event.y<(self._y_d - self._y_bound)):
            return 1#go the other way
        else:
            if(event.y>(self._y_d + self._y_bound)):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0


    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.BUOY_FOUND : DHyperApproach}

class IMaTarget(state.State):
    def TARGET_FOUND(self,event):
            print "x:"+str(event.x)
            print "y:"+str(event.y)
            print "lx:"+str(event.largex)
            print "ly:"+str(event.largey)
            print "sx:"+str(event.smallx)
            print "sy:"+str(event.smally)
            print "px:"+str(event.panelx)
            print "py:"+str(event.panely)
            print "sq:"+str(event.squareNess)
            print "r:"+str(event.range)
            #print "lf:"+str(event.largeflag)
            #print "sf:"+str(event.smallflag)
            print "lr:"+str(event.rangelarge)
            print "sr:"+str(event.rangesmall)
            print "color:"+str(event.color)
    
    @staticmethod
    def transitions():
        return { vision.EventType.TARGET_FOUND : IMaTarget }

class TMApproach(HyperApproach):
    def TARGET_FOUND(self,event):
        if(event.color == vision.Color.RED):
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
        return {DONE : state.State, vision.EventType.TARGET_FOUND : TMApproach}

    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .4 , 'kz' : .45, 'x_d' : 0, 'r_d' : 200 , 'y_d' : 0, 'x_bound': .05, 'r_bound': 20, 'y_bound':.05 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

class TMApproachL(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .25 , 'kz' : .25, 'x_d' : 0, 'r_d' : 160 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 10, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if(event.color == vision.Color.RED and event.largeflag == True):

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
        return {DONE : state.State, vision.EventType.TARGET_FOUND : TMApproachL}

#NOTE: currently attached to right TMA approach
class TMApproachSL1(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 7 , 'ky' : .15 , 'kz' : .35, 'x_d' : 0, 'r_d' : 85 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 15, 'y_bound':.075 ,'minvx': .05, 'minvy': .1 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if event.color == vision.Color.RED:
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
        if event.color == vision.Color.YELLOW:
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

class ImABin(state.State):
    def BIN_FOUND(self,event):
            print "TYPE: "+str(event.type)
            print "id: "+str(event.id)
            print "x: "+str(event.x)
            print "y: "+str(event.y)
            print "range: "+str(event.range)
    
    @staticmethod
    def transitions():
        return { vision.EventType.BIN_FOUND : ImABin }

class PizzaBoxCenter(DHyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : .25 , 'ky' : .25 , 'kz' : 7, 'x_d' : 0, 'r_d' : 190 , 'y_d' : 0, 'x_bound': .075, 'r_bound': 20, 'y_bound':.075 ,'minvx': .1, 'minvy': .1 ,'minvz' : .3}

    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.BUOY_FOUND : PizzaBoxCenter }

    def decideZ(self,event):
        return 0

class Side(util.MotionState):
    def enter(self):
        self.translate(.15,0,.1)
    @staticmethod
    def transitions():
        return {motion.basic.MotionManager.FINISHED : Side, util.DONE : state.State}

class SideV2(XZCenter):
    def enter(self):
        pass

    @staticmethod
    def transitions():
        return { approach.DONE : SideV2}

#left handed variant
class TMApproachSL2(HyperApproach):
    @staticmethod
    def getattr():
        return { 'kx' : 10 , 'ky' : .15 , 'kz' : .35, 'x_d' : -.15, 'r_d' : 85 , 'y_d' : -.07, 'x_bound': .05, 'r_bound': 10, 'y_bound':.05 ,'minvx': .05, 'minvy': .05 ,'minvz' : .1}

    def TARGET_FOUND(self,event):
        if event.color == vision.Color.RED:
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
        if event.color == vision.Color.YELLOW:
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
        if(event.color == vision.Color.RED and event.largeflag == True):

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
        if(event.color == vision.Color.RED and event.largeflag == True):

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
        if(event.color == vision.Color.RED and event.largeflag == True):

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
        if(event.color == vision.Color.RED and event.largeflag == True):

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
        return {DONE : state.State, vision.EventType.TARGET_FOUND : TMApproachLR2}


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

class IMaHedge(state.State):

    @staticmethod
    def transitions():
        return { vision.EventType.GATE_FOUND : IMaHedge }


    def enter(self):
        pass

    def GATE_FOUND(self, event):
        print '-------------'
        print 'Event X: ' + str(event.x)
        print 'Event Y: ' + str(event.y)
        print 'Event Size: ' + str(event.range)

    def exit(self):
        pass
