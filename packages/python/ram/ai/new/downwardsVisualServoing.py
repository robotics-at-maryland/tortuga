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


#NOTE: I've mostly just based this off the old downwards visual servoing code, please use caution when reusing this!
#This code should be rewritten in the future because to be utterly honest, its terrible
#Before you ask, yes z is unused, this code was copy pasted from the old stuff
class DownVisualServoing(state.State):
    def __init__(self,visionObject, dx,dy,configNode = None):
        super(DownVisualServoing,self).__init__()
        self._obj = visionObject
        if(configNode != None):
            self._kx = configNode.get('kx',.15)
            self._ky = configNode.get('kx',.4)
            self._kz = configNode.get('kz',0)
            self._x_bound = configNode.get('x_bound',.1)
            self._y_bound = configNode.get('y_bound',.1)
            self._r_bound = configNode.get('r_bound',0)
            self._minvx = configNode.get('minvx',.2)
            self._minvy = configNode.get('minvx',.2)
            self._minvz = configNode.get('minvx',.1)
        else:
            self._kx = .15
            self._ky = .4
            self._kz = 0
            self._x_d = dx
            self._y_d = dy
            self._r_d = 1
            self._x_bound = .05
            self._y_bound = .05
            self._r_bound = 0
            self._minvx = .2
            self._minvy = .2
            self._minvz = .1
    
    def update(self):
        self._obj.update()
        self.runMotion(self._obj)    

    def runMotion(self,event):
        #this variant moves the minimal speed possible unless
        #a speed is zero, allowing for theoretically very fast behaviour
        #while maintaining the image within the frame
        #to clarify, this chooses the smallest possible speed to move at and uses it
        #this is done to ensure general stability
        vx = self.xFunc(event)*self.decideX(event)
        vy = self.yFunc(event)*self.decideY(event)
        vz = self.zFunc(event)*self.decideZ(event)
        if(abs(vx) > abs(vy) and vy != 0):
            vx = m.copysign(vy,vx)
        if(abs(vy) > abs(vx) and vx != 0):
            vy = m.copysign(vx,vy)
        self.getStateMachine().getLegacyState().motionManager._controller.moveVel(vx, vy, vz)

    def yFunc(self,event):
        a = self._ky*(event.x - self._x_d)
        if(abs(a)<self._minvy):
            a = m.copysign(self._minvy,a)
        return a

    def zFunc(self,event):
        event.range = 1
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

#the goal angle difference is the final relative orientation with what you're aligning with
#0 means you orient with the object
#this state is a controller it will never transition out of itself
class YawVisualServoing(state.State):
    def __init__(self, visionObj,goalAngleDifference = 0):
        super(YawVisualServoing, self).__init__()
        self._obj = visionObj
        self._goal = goalAngleDifference


    def getMotionManager(self):
        return self.getStateMachine().getLegacyState().motionManager

    def getStateEstimator(self):
        return self.getStateMachine().getLegacyState().stateEstimator

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
    def update(self):
        self._obj.update()
        currentOrientation = self.getStateEstimator().getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          -self._obj.angle),
            initialRate = self.getStateEstimator().getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.getMotionManager().setMotion(yawMotion) 
