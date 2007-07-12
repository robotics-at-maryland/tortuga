class GoPipeline():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        orangeStates={"initialize":self.initialize,
                      "scan":self.scan,
                      "done":self.done,
                      "makeSureBinFound":self.makeSureBinFound,
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
        
    def initialize(self):
        self.vision.downward.orangeDetectOn()
        self.vision.downward.binDetectOn()
        self.controller.setSpeed(2)
        self.controller.yawVehicle(-30)
        self.ai.stateMachine.change_state("waitForReadyThenScan")
    
    def waitForReadyThenScan(self):
        if (self.controller.isReady()):
            self.ai.stateMachine.change_state("scan")

    def scan(self):
        if (self.controller.isReady()):
            self.controller.yawVehicle(scanDegrees)
            scanCount=scanCount+1
            if (scanCount==12):
                scanDegrees=-scanDegrees
                scanCount=0
        
        self.ai.stateMachine.change_state("pollVision")
        
    def makeSureBinFound(self):
        self.controller.setSpeed(0) 
        binDetector=self.vision.downward.BinDetector
        if (binDetector.found()):
            binCount+=1
        else:
            binCount-=1
            
        if (binCount<=0):
            binCount=0
            self.ai.stateMachine.change_state("pollVision")
        elif (binCount>=5):
            self.ai.stateMachine.change_state("done")
        else:
            pass #remain in this state stopped and looking for a bin
        
    def pollVision(self):
        orangeDetector=self.vision.downward.OrangeDetector
        binDetector=self.vision.downward.BinDetector
        
        if (binDetector.found()):
            self.binCount+=1
            self.ai.stateMachine.change_state("makeSureBinFound")
        elif (orangeDetector.found()):
            self.orangeCount+=1
            if (self.orangeCount>15):
                self.orangeCount=15
            self.ai.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
        elif (self.orangeCount<=5):
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
        #self.vision.downward.binDetectOff() #not turning it off because binDetection is next anyway
        self.ai.returnControl()
