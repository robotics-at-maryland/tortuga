class GoPipeLight():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        orangeStates={"initialize":self.initialize,
                      "scan":self.scan,
                      "done":self.done,
                      "makeSureBinFound":self.makeSureBinFound,
                      "chaseLight": self.chaseLight,
                      "pollVision":self.pollVision,
                      "waitForReadyThenScan":self.waitForReadyThenScan,
                     }
        self.ai.stateMachine.set_states(orangeStates)
        self.orangex=-1.0
        self.orangey=-1.0
        self.scanCount=0
        self.scanDegrees=5
        self.binCount=0
        self.orangeCount=0
        self.ai.stateMachine.change_state("initialize")
        self.countDown=15
        self.lostPipeCountDown=40
        self.countOfTimesPipeLost=0
        self.lightFoundCount=0
        self.lightLostCount=0
        self.chaseCount=0
        
    def initialize(self):
        self.vision.downward.orangeDetectOn()
        self.vision.forward.redLightDetectOn()
        self.controller.setSpeed(4)
        self.controller.yawVehicle(-60)
        self.ai.stateMachine.change_state("waitForReadyThenScan")
    
    def waitForReadyThenScan(self):
        if (self.controller.isReady()):
            self.ai.stateMachine.change_state("scan")

    def scan(self):
        if (self.controller.isReady()):
            self.controller.yawVehicle(scanDegrees)
            scanCount=scanCount+1
            if (scanCount==24):
                scanDegrees=-scanDegrees
                scanCount=0
        
        self.ai.stateMachine.change_state("pollVision")
    
    def chaseLight(self):
        self.chaseCount+=1#start slow, then speed up
        if (self.chaseCount>80):
            self.chaseCount=80 
        redDetector=self.vision.forward.RedLightDetector
        self.controller.setSpeed(chaseCount/10)#smash hard.... but not TOO hard...
        if (redDetector.found()):
            self.lightLostCount=0
            self.ai.navigateTowards(redDetector.getX(),redDetector.getY())
        else:
            self.lightLostCount+=1
            if (lightLostCount==20):
                #Its gone, lets hope we hit it and find that bin
                self.controller.setSpeed(0)
                self.ai.change_state("done")
            
    def pollVision(self):
        orangeDetector=self.vision.downward.OrangeDetector
        redDetector=self.vision.forward.RedLightDetector
        
        if (redDetector.found()):
            self.lightFoundCount+=1
            self.ai.navigateTowards(redDetector.getX(),redDetector.getY())
            if (self.lightFoundCount==5):
                self.lightFoundCount=0
                self.ai.stateMachine.change_state("chaseLight")
        elif (orangeDetector.found()):
            self.orangeCount+=1
            if (orangeCount>15):
                orangeCount=15
            self.ai.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
        elif (orangeCount<=5):
            self.ai.stateMachine.change_state("scan")
        else:
            self.countDown-=1
            if (self.countDown==0):
                self.countDown=15
                self.controller.setSpeed(-5)
                self.ai.stateMachine.change_state("lostPipe")
                
    def lostPipe(self):
        orangeDetector=self.vision.downward.OrangeDetector
        self.lostPipeCountDown-=1
        if (orangeDetector.found() and (not orangeDetector.getAngle()==-10)):
            self.controller.setSpeed(0)
            self.ai.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
            self.ai.stateMachine.change_state("pollVision")
        elif (self.lostPipeCountDown==0):
            self.countOfTimesPipeLost+=1
            if (countOfTimesPipeLost==5):#Its like we're just running off the edge over and over, maybe we got turned around?
                countOfTimesPipeLost=0
                self.controller.yawVehicle(180)
                self.ai.stateMachine.change_state("initialize")
            else:
                self.lostPipeCountDown=40
                self.controller.setSpeed(2)
                self.ai.stateMachine.change_state("scan")
        
    def done(self):
        self.vision.downward.orangeDetectOff()
        self.vision.forward.redLightDetectOff()
        self.ai.returnControl()
