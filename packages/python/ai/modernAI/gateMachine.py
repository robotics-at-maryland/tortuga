from aiStateMachine import aiStateMachine
import time as clock

class gateMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)

    def startState(self,args,interFuncs,interStates):
        print "diving and goin!"
        diveDistance = 6
        args = [diveDistance]
        self.changeState(self.dive,args)
    
    def dive(self,args,interFuncs,interStates):
        diveDistance = args[0]
        self.controller.setDepth(diveDistance)
        self.changeState(self.waitForDive)
        
    def waitForDive(self,args,interFuncs,interStates):
        if self.controller.isAtDepth():
            driveSpeed = 5
            driveTime = 35
            args = [driveSpeed,driveTime]
            self.changeState(self.drive)
            
    def drive(self,args,interFuncs,interStates):
        driveSpeed = args[0]
        driveTime = args[1]
        self.controller.setSpeed(driveSpeed)
        startTime = clock.time()
        args = [startTime,driveTime,self.end]
        self.changeState(self.waitForTime,args)
        
    def waitForTime(self,args,interFuncs,interStates):
        startTime = args[0]
        waitTime = args[1]
        afterState = args[2]
        elapsed = clock.time() - startTime
        if elapsed >= waitTime:
            self.changeState(afterState)
            
    def end(self,args,interFuncs,interStates):
        self.exit()