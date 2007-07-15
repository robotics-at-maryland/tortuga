from aiStateMachine import aiStateMachine
import time as clock
import math

class movementMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        entryFunc = args[0]
        self.changeState(entryFunc)
        
    def sleep(self,args,interFuncs,interStates):
        sleepTime = args[0]
        entryTime = args[1]
        elapsed = clock.time() - entryTime
        if elapsed >= sleepTime:
            self.exit()
            
    def driveForTime(self,args,interFuncs,interStates):
        driveSpeed = args[0]
        driveTime = args[1]
        entryTime = args[2]
        self.controller.setSpeed(driveSpeed)
        self.changeState(self.sleep,[driveTime,clock.time()])
        