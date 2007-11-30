import ext.core as core
import ext.vehicle_device as vdev
from math import sin, pi

class Thruster(vdev.IThruster,core.EventPublisher):
    def __init__(self,forceIn=0,nameIn="AftThruster"):
        vdev.IThruster.__init__(self)
        core.EventPublisher.__init__(self)
        self.force = forceIn
        self.name = nameIn
        self.stepVal = 0
        self.PYFORCEUPDATE = str(self.FORCE_UPDATE)
        self.bar = None 
        
    def getName(self):
        return self.name
    
    def getForce(self):
        return self.force
        
    def setForce(self, val):
        self.force = val
        
    def step(self):
        self.setForce(sin(self.stepVal))
        self.stepVal += (pi/6)
        
    def update(self):
        self.step()     
        pyEvent = core.Event()
        pyEvent.force = self.force
        self.publish(self.PYFORCEUPDATE, pyEvent)
        
    def handlerForceUpdate(self, event):
        self.etype = event.type
        self.sender = event.sender
        self.args = event
        self.forceVal = event.force
        self.called = True
        tmp =int(self.forceVal*100)
        self.bar.setVal(tmp)
        #self.Refresh()
