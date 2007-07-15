from aiStateMachine import aiStateMachine
import time as clock
import math

class movementMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
                
    def startState(self,args,interrupts):
        entryFunc = args["entryFunc"]
        self.startTime = clock.time()
        self.changeState(entryFunc)
        
    def sleep(self,args,interrupts):
        sleepTime = args["sleepTime"]
        elapsed = clock.time() - self.startTime
        if elapsed >= sleepTime:
            self.exit()
            
    def diveInTime(self,args,interrupts):
        startDepth = args["startDepth"]
        diveDepth = args["diveDepth"]
        diveTime = args["diveTime"]
        elapsed = clock.time() - self.startTime
        neededChange = diveDepth - startDepth
        timeFraction = (elapsed / diveTime)
        currentNeededDepth = neeedChange * timeFraction
        self.setDepth(currentNeededDepth)
        if currentNeededDepth >= diveDepth:
            self.exit()
                        
    def driveForTime(self,args,interrupts):
        driveSpeed = args["driveSpeed"]
        driveTime = args["driveTime"]
        self.controller.setSpeed(driveSpeed)
        self.changeState(self.sleep,{"driveTime":driveTime})
        