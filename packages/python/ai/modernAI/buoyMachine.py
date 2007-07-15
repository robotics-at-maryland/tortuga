from aiStateMachine import aiStateMachine

startDepth = 0.5
driveSpeed = 3
buoyDepth = 8
peakAngle = 45

class buoyMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        self.vision.forward.redLightDetectOn()
        self.redInterrupts = {
                            self.seeingRed:self.foundRed
                            }
    
    def startState(self,args,interrupts):
        self.changeState(self.go,None,self.redInterrupts)
        
    def go(self,args,interrupts):
        self.controller.setDepth(startDepth)
        self.changeState(self.waitAndDrive,None,self.redInterrupts)
        
    def waitAndDrive(self,args,interrupts):
        if self.controller.isOriented():
            self.controller.setSpeed(driveSpeed)
            self.movingDepth = startDepth
            self.changeState(self.diveSequence,None,self.redInterrupts)
            
    def diveSequence(self,args,interrupts):
        newDepth = depth + self.movingDepth
        if self.controller.isOriented():
            if self.movingDepth >= buoyDepth:
                self.changeState(self.firstLook,None,self.redInterrupts)
            else:
                self.controller.setDepth(newDepth)
                self.movingDepth = newDepth
    
    def firstLook(self,args,interrupts):
        self.controller.setSpeed(0)
        self.controller.yawVehicle(peakAngle/2)
        self.changeState(self.lookAround,None,self.redInterrupts)
        
    def lookLeft(self,args,interrupts):
        if self.controller.isOriented():
            self.controller.yawVehicle(peakAngle)
            self.changeState(self.lookRight,None,self.redInterrupts)
            
    def lookRight(self,args,interrupts):
        if self.controller.isOriented():
            self.controller.yawVehicle(-1 * peakAngle)
            self.changeState(self.lookBack,None,self.redInterrupts)
            
    def lookBack(self,args,interrupts):
        if self.controller.isOriented():
            self.controller.yawVehicle(peakAngle/2)
            self.changeState(self.creep,None,self.redInterrupts)
            
    def creep(self,args,interrupts):
        if self.controller.isOriented():
            self.controller.setSpeed(driveSpeed)
            self.changeState(self.firstLook,None,self.redInterrupts)
         
    def foundRed(self,args,interrupts):
        bouy = buoyChaseMachine()
        self.branch(bouy,self.end)
        
    def end(self,args,interrupts):
        self.setDepth(0)
        self.setSpeed(0)
        self.exit()
        
     def seeingRed(self):
        lightDetector = self.vision.forward.RedLightDetector
        if lightDetector.found():
            return True
        else:
            return False
            