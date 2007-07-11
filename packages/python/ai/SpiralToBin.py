class GoCoveredBin():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        binStates={
                   "initialize":self.initialize,
                   "spiral":self.spiral,
                   "done":self.done,
                   "pollVision":self.pollVision
                  }
        self.ai.stateMachine.set_states(binStates)
        self.ai.stateMachine.change_state("initialize")
        self.countDownToLost=20
        self.ai.spiralSpeed=0
        
    def initialize(self):
        #In theory, we have just smashed into the second red light.  Now our job is to find the bin, which should be
        #quite close to us.
        self.vision.downward.binDetectOn()
        self.controller.setSpeed(0)
        self.controller.setDepth(10)
        self.ai.stateMachine.change_state("pollVision")
                
    def pollVision(self):
        binDetector=self.vision.downward.BinDetector
        if (binDetector.found()):
            x=binDetector.getX()
            y=binDetector.getY()
            if (x>.48 and x<.52 and y>.48 and y<.52):
                self.vehicle.dropMarker()
                self.ai.stateMachine.change_state("done")
            else:
                self.countDownToLost=20
                navigateAboveBin(x,y)
        else:
            self.countDownToLost-=1
            if (self.countDownToLost<=0):#if we lose the bin for 20 tries, start heading upwards
                depth=self.controller.getDepth()
                if (depth>6):
                    self.controller.setDepth(depth-.2)
                else:
                    #we're as high as I'm willing to risk, the bins just gone, since the next step is to find the treasure
                    #we're going to spiral out from our current location and pray we find the bin that way
                    #Remember that we're now at depth 6
                    self.ai.stateMachine.change_state("spiral")
    
    def spiral(self):
        #we're at depth 6, near the second red buoy hopefully, the bin has to be around here somewheres.
        binDetector=self.vision.downward.BinDetector
        if (binDetector.found()):
            self.ai.stateMachine.change_state("pollVision")
            self.spiralSpeed=0
        else:
            self.spiralSpeed+=1
            self.controller.setSpeed(self.spiralSpeed/25)
            self.controller.yawVehicle(4)
    
    def done(self):
        self.vision.downward.binDetectOff()
        self.ai.returnControl()