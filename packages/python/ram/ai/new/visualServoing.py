import math as m
import ext.control as control
import ram.ai.new.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
import ram.ai.new.Utility as util
import ram.ai.new.approach
from ram.motion.basic import Frame

class VisualServoing(state.State):
    def __init__(self, visionObject, desiredX, desiredY, desiredRange):
       self.kx = .05
       self.ky = .4
       self.kz = .45
       
       self.minvx = .1
       self.minvy = .1
       self.minvz = .1
       
       self.visionObject = visionObject
       self.desiredX = desiredX
       self.desiredY = desiredY
       self.desiredRange = desiredRange

    def update(self):
        pass

    def decideX(self,event):
        return 0

    def decideY(self,event):
        return 0

    def decideZ(self,event):
        return 0

class ForwardVisualServoing(VisualServoing):
    def update(self):
        

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

class BottomVisualServoing(VisualServoing):
    def update(self):
        error_x = self.desiredX - self.visionObject.x
        error_y = self.desiredY - self.visionObject.y
        
        
        self.visionObject.update();

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
