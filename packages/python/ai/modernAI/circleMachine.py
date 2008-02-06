from aiStateMachine import aiStateMachine
from movementMachine import movementMachine
import time as clock

turnTime = 1
turnAngle = 5

class controlTestMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interrupts):
        self.controller.setSpeed(4)
        self.lastTime = clock.time()
        self.changeState(self.circle)
    
    def circle(self,args,interrupts):
        if clock.time() - self.lastTime:
            self.yawVehicle(turnAngle)
            self.lastTime = clock.time()