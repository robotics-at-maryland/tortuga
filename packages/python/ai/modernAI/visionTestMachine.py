from aiStateMachine import aiStateMachine
import time as clock

class buoyMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        self.vision.forward.redLightDetectOn()
        self.lightDetector = self.vision.forward.RedLightDetector
        
    def startState(self,args,interFuncs,interStates):
        self.vehicle.printLine(0,"Starting vision")
        self.vehicle.printLine(1,"test routine")
        self.changeState(self.detect)
        
    def detect(self,args,interFuncs,interStates):
        if self.lightDetector.found():
            x = lightDetector.getX()
            y = lightDetector.getY() 
            self.vehicle.printLine(0,"Red Light!")
            message = "(" + str(x) + "," + str(y) + ")"
            self.vehicle.printLine(1,message)
            self.changeSate(self.redOn)
            self.writeTime = clock.time()
    
    def redOn(self,args,interFuncs,interStates):
        elapsed = clock.time() - self.writeTime
        if self.lightDetector.found()
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
    
    def redOff(self,args,interFuncs,interStates):
        if self.lightDetector.found():
            self.changeState(self.redOn)
            
    def initPointAtRed(self,args,interFuncs,interStates):
        x = self.lightDetector.getX()
        self.lastX = x
        self.direction = 1
        self.changeState(self.pointAtRed)
        self.controller.setSpeed(1)
        
    def pointAtRed(self,args,interFuncs,interStates):
        x = self.lightDetector.getX()
        if x < 0.4:
            self.controller.yawVehicle(3)
        elif x > 0.6:
            self.controller.yawVehicle(-1 * 3)
        else:
            self.vehicle.printLine(0,"Holding Position!")
            