from aiStateMachine import aiStateMachine
import time as clock

class gateMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)

    def startState(self,args,interrupts):
        print "diving and goin!"
        diveDistance = 6
        args = {"diveDistance":diveDistance}
        self.changeState(self.dive,args)
    
    def dive(self,args,interrupts):
        diveDistance = args["diveDistance"]
        self.controller.setDepth(diveDistance)
        self.changeState(self.waitForDive)
        
    def waitForDive(self,args,interrupts):
        if self.controller.isAtDepth():
            driveSpeed = 5
            driveTime = 35
            args = {"driveSpeed":driveSpeed,"driveTime":driveTime}
            self.changeState(self.drive,args)
            
    def drive(self,args,interrupts):
        driveSpeed = args["driveSpeed"]
        driveTime = args["driveTime"]
        self.controller.setSpeed(driveSpeed)
        startTime = clock.time()
        args = {"startTime":startTime,"driveTime":driveTime,"afterState":self.end}
        self.changeState(self.waitForTime,args)
        
    def waitForTime(self,args,interrupts):
        startTime = args["startTime"]
        waitTime = args["waitTime"]
        afterState = args["afterState"]
        elapsed = clock.time() - startTime
        if elapsed >= waitTime:
            self.changeState(afterState)
            
    def end(self,args,interFuncs,interStates):
        self.exit()