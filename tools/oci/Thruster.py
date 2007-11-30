import ext.core as core
import ext.vehicle_device as vdev
from math import sin, pi
import thread,time
class Thruster(vdev.IThruster,core.EventPublisher):
    def __init__(self,forceIn=0,nameIn="AftThruster"):
        vdev.IThruster.__init__(self)
        core.EventPublisher.__init__(self)
        self.force = forceIn
        self.name = nameIn
        self.stepVal = 0
        self.PYFORCEUPDATE = str(self.FORCE_UPDATE)
        
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
        
class Reciever(object):
    def __init__(self,panel):
        self.etype = None
        self.sender = None
        self.args = None
        self.calls = 0
        self.called = False
        self.forceVal = 0
        self.panel=panel
    
    def __call__(self, event):
        self.etype = event.type
        self.sender = event.sender
        self.args = event
        self.calls += 1
        self.forceVal = event.force
        self.called = True
        tmp =int(self.forceVal*100)
        self.panel.setVals(tmp)
        print "pong",tmp