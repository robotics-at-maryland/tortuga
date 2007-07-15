from aiStateMachine import aiStateMachine
import time as clock

class gateMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        self.controller.yawVehicle(180)
        self.changeState(self.waitAndExit)
        
    def waitAndExit(self,args,interFuncs,interStates):
        if self.controller.isOriented():
            self.exit()