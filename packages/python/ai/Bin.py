class GoBin():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        binStates={
                   "initialize":self.initialize,
                   "done":self.done,
                   "pollVision":self.pollVision,
                  }
        self.ai.stateMachine.set_states(binStates)
        self.ai.stateMachine.change_state("initialize")
        self.countDownToLost=20
        
    def initialize(self):
        #We should only enter the bin states once we have already
        #found the bin (at least for bin1).  All this needs to do
        #is navigate to the bin.  If we lose it, well... unfortunate
        #but also too bad.  Best we can do is drop marker and move on
        self.controller.setSpeed(0)
        self.controller.setDepth(10)
        self.vision.downward.binDetectOn()
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
            if (self.countDownToLost<=0):#if we lose the bin for 10 tries frames, start heading upwards
                depth=self.controller.getDepth()
                if (depth>6):
                    self.controller.setDepth(depth-.2)
                else:
                    #we're as high as I'm willing to risk, the bins just gone, lets drop marker and move on
                    self.controller.setDepth(10)
                    #self.vehicle.dropMarker()#TODO FIXME WRITE THIS METHOD JOE!
                    self.ai.stateMachine.change_state("done")
                    
    def done(self):
        self.vision.downward.binDetectOff()
        self.ai.returnControl()