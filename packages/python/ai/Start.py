class GoGate():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        gateStates={"initialize":self.initialize,
                     "done":self.done,
                     "pollVision":self.pollVision,
                     "changeHeading":self.changeHeading
                     }
        self.stateMachine.set_states(gateStates)
        self.gatex=-1.0
        self.gatey=-1.0
        self.gateCount=0
        self.gateOffCount=0
        self.ai.stateMachine.change_state("initialize")
        
    def initialize(self):
        self.vision.forward.gateDetectOn()
        self.controller.setSpeed(5)
        self.ai.stateMachine.change_state("pollVision")
        
    def pollVision(self):
        #WRITE THIS CODE HURRY HURRY!
        gateDetector=self.vision.forward.gateDetector
        if (gateDetector.found()):
            self.gateCount=self.gateCount+1
            self.gateOffCount=0
            gatex=gateDetector.getX()
            gatey=gateDetector.getY()
            self.ai.stateMachine.change_state("changeHeading")
        else:
            if (self.gateCount>3):
                self.gateOffCount=self.gateOffCount+1
            gatex=-1.0
            gatey=-1.0
            self.ai.stateMachine.change_state("pollVision")
        
        if (self.gateOffCount > 7):
            self.ai.stateMachine.change_state("done")
            
    def changeHeading(self):
        self.ai.navigateToward(self.gatex,self.gatey)
        self.change_state("pollVision")
    
    def done(self):
        self.vision.forward.gateDetectOff()
        self.ai.returnControl()
