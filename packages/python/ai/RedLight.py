class GoRedLight():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        lightStates={"initialize":self.initializing,
                     "scan":self.scan,
                     "done":self.done,
                     "pollVision":self.pollVision,
                     "waitForReadyThenScan":self.waitForReadyThenScan,
                     "changeHeading":self.changeHeading
                     }
        self.ai.stateMachine.set_states(lightStates)
        self.lightx=-1.0
        self.lighty=-1.0
        self.ai.stateMachine.change_state("initialize")
        self.scanDegrees=5
        self.scanCount=0
        self.seenLight=0
        
    def initialize(self):
        self.vision.forward.redLightDetectOn()
        self.vision.downward.orangeDetectOn()
        self.controller.setSpeed(2)
        self.controller.yawVehicle(-45)#45 degrees to the right
        self.ai.stateMachine.change_state("waitForReadyThenScan")
    
    def waitForReadyThenScan(self):
        if (self.controller.isReady()):
            self.ai.stateMachine.change_state("scan")

    def scan(self):
        if (self.controller.isReady()):
            self.controller.yawVehicle(scanDegrees)
            scanCount=scanCount+1
            if (scanCount==18):
                scanDegrees=-scanDegrees
                scanCount=0
        
        self.ai.stateMachine.change_state("pollVision")
        
    def pollVision(self):
        lightDetector=self.vision.forward.RedLightDetector
        orangeDetector=self.vision.downward.OrangeDetector
        
        if (lightDetector.found()):
            self.seenLight+=1
            if (self.seenLight>10):
                self.seenLight=10
            self.lightx=lightDetector.getX()
            self.lighty=lightDetector.getY()
            self.ai.stateMachine.change_state("changeHeading")
        elif (orangeDetector.found()):
            if (self.seenLight<5):
                self.seenLight=self.seenLight-1
                self.ai.stateMachine.change_state("done")
        else:
            self.ai.statemachine.change_state("scan")
            
            
    def changeHeading(self):
        self.ai.navigateToward(self.lightx,self.lighty)
        self.change_state("pollVision")
    
    def done(self):
        self.vision.forward.redLightDetectOff()
        self.vision.downward.orangeDetectOff()
        self.ai.returnControl()
