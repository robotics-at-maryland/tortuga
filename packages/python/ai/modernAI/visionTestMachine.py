from ai.modernAI.aiStateMachine import aiStateMachine
import time as clock

class visionTestMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
	self.vision.forward.lightDetectOn()
        self.lightDetector = self.vision.forward.getRedDetector()
    
    def startState(self,args,interrupts):
        self.vehicle.printLine(0,"Starting vision")
        self.vehicle.printLine(1,"test routine")
        self.changeState(self.detect)
        
    def detect(self,args,interrupts):
        if self.lightDetector.found():
            x = lightDetector.getX()
            y = lightDetector.getY() 
            self.vehicle.printLine(0,"Red Light!")
            message = "(" + str(x) + "," + str(y) + ")"
            self.vehicle.printLine(1,message)
            self.changeSate(self.redOn)
            self.writeTime = clock.time()
    
    def redOn(self,args,interrupts):
        elapsed = clock.time() - self.writeTime
        if self.lightDetector.found():
            if elapsed >= 0.5:
                x = lightDetector.getX()
                y = lightDetector.getY() 
                self.vehicle.printLine(0,"Red found!")
                message = "(" + str(x) + "," + str(y) + ")"
                self.vehicle.printLine(1,message)
                self.writeTime = clock.time()
        else:
            self.vehicle.printLine(0,"Lost red...")
            self.vehicle.printLine(1," ")
            self.changeState(self.redOff)
    
    def redOff(self,args,interrupts):
        if self.lightDetector.found():
            self.changeState(self.redOn)
            
    ########### active seek code ###########
            
    def initPointAtRed(self,args,interrupts):
        x = self.lightDetector.getX()
        self.lastX = x
        self.direction = 1
        self.changeState(self.pointAtRed)
        self.controller.setSpeed(1)
        
    def pointAtRed(self,args,interrupts):
        x = self.lightDetector.getX()
        if x < 0.4:
            self.controller.yawVehicle(3)
        elif x > 0.6:
            self.controller.yawVehicle(-1 * 3)
        else:
            self.vehicle.printLine(0,"Holding Position!")
            