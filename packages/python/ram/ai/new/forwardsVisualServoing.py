import ram.motion as motion
import ext.estimation as estimation
import ext.math as math
import ram.ai.new.utilStates as utilStates
import ram.ai.new.stateMachine as stateMachine
import ram.ai.Utility as oldUtil
import ram.ai.new.searchPatterns as search
import ram.ai.new.state as state
import ram.ai.new.utilClasses as util
from ext.control import yawVehicleHelper
import math as m

class ForwardsVisualServoing(state.State):
    def __init__(self,visionObject, dx,dy,drange,configNode = None):
        super(ForwardsVisualServoing,self).__init__()
        self._obj = visionObject
        if(configNode != None):
            self._kx = configNode.get('kx',.15)
            self._ky = configNode.get('ky',.4)
            self._kz = configNode.get('kz',.4)
            self._x_bound = configNode.get('x_bound',.1)
            self._y_bound = configNode.get('y_bound',.1)
            self._r_bound = configNode.get('r_bound',.1)
            self._minvx = configNode.get('minvx',.05)
            self._minvy = configNode.get('minvy',.05)
            self._minvz = configNode.get('minvz',.05)
        else:
            self._kx = .15
            self._ky = .15
            self._kz = .15
            self._x_d = dx
            self._y_d = dy
            self._r_d = drange
            self._x_bound = .1
            self._y_bound = .1
            self._r_bound = .1
            self._minvx = .05
            self._minvy = .05
            self._minvz = .05

    def enter(self):
        self.motionManager = self.getStateMachine().getLegacyState().motionManager

    def update(self):
        super(ForwardsVisualServoing,self).update()
        self._obj.update()
        self.runMotion(self._obj)

    #this is copypasta, its bad for your health
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
