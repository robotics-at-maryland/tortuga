from aiStateMachine import aiStateMachine
import time as clock

class controlTestMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        self.controller.yawVehicle(180)
        self.changeState(self.waitAndExit)
    
    def turnLeft(self,args,interFuncs,interStates):
	self.controller.yawVehicle(90)
	self.changeState(self.turnRight)

    def turnRight(self,args,interFuncs,interStates):
	if self.controller.isOriented():
	    self.controller.yawVehicle(-90)
	    self.changeState(waitAndExit)

    def waitAndExit(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.exit()