from aiStateMachine import aiStateMachine

startDepth = 0.5
driveSpeed = 3
buoyDepth = 8
peakAngle = 45

class buoyMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        self.vision.forward.redLightDetectOn()
    
    def startState(self,args,interFuncs,interStates):
        self.changeState(self.simpleGo,None,[self.seeingRed],[self.foundRed])
        
    def go(self,args,interFuncs,interStates):
        self.controller.setDepth(startDepth)
        self.changeState(self.waitAndDrive,None,[self.seeingRed],[self.foundRed])
        
    def simpleGo(self,args,interFuncs,interStates):
        self.controller.setDepth(startDepth)
        self.controller.setSpeed(2)
        self.changeState(self.drive,[clock.time()],[self.seeingRed],[self.foundRed])
        
    def drive(self,args,interFuncs,interStates):
        pass
        
    def waitAndDrive(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.controller.setSpeed(driveSpeed)
            self.movingDepth = startDepth
            self.changeState(self.diveSequence,None,[self.seeingRed],[self.foundRed])
            
    def diveSequence(self,args,interFuncs,interStates):
        newDepth = depth + self.movingDepth
        if self.controller.isOriented():
            if self.movingDepth >= buoyDepth:
                self.changeState(self.firstLook,None,[self.seeingRed],[self.foundRed])
            else:
                self.controller.setDepth(newDepth)
                self.movingDepth = newDepth
    
    def firstLook(self,args,interFuncs,interStates):
        self.controller.setSpeed(0)
        self.controller.yawVehicle(peakAngle/2)
        self.changeState(self.lookAround,None,[self.seeingRed],[self.foundRed])
        
    def lookLeft(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.controller.yawVehicle(peakAngle)
            self.changeState(self.lookRight,None,[self.seeingRed],[self.foundRed])
            
    def lookRight(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.controller.yawVehicle(-1 * peakAngle)
            self.changeState(self.lookBack,None,[self.seeingRed],[self.foundRed])
            
    def lookBack(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.controller.yawVehicle(peakAngle/2)
            self.changeState(self.creep,None,[self.seeingRed],[self.foundRed])
            
    def creep(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.controller.setSpeed(driveSpeed)
            self.changeState(self.firstLook,None,[self.seeingRed],[self.foundRed])
         
    def foundRed(self,args,interFuncs,interStates):
        bouy = buoyChaseMachine()
        self.branch(bouy,self.end)
        
    def end(self,args,interFuncs,interStates):
        self.setDepth(0)
        self.setSpeed(0)
        self.exit()
        
     def seeingRed(self):
        lightDetector = self.vision.forward.RedLightDetector
        if lightDetector.found():
            return True
        else:
            return False
            