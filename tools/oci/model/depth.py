import ext.core as core
#import ext._vehicle_device as vdev

class Depth(core.EventPublisher):
    def __init__(self,forceIn=0,nameIn="AftThruster"):
        #vdev.IThruster.__init__(self)
        core.EventPublisher.__init__(self)
        self.depth = forceIn
        self.name = nameIn
        self.PYFORCEUPDATE = "UPDATE_DEPTH"
        
    def getName(self):
        return self.name
    
    def getDepth(self):
        return self.depth
        
    def setDepth(self, val):
        self.depth = val
        
    def step(self):
        if self.depth >= 100:
            self.depth = 0
        self.depth += 10

        
    def update(self):
        self.step()     
        pyEvent = core.Event()
        pyEvent.depth = self.depth
        self.publish(self.PYFORCEUPDATE, pyEvent)