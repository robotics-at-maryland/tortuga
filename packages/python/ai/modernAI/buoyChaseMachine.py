from aiStateMachine import aiStateMachine

startDepth = 0.5
driveSpeed = 3
buoyDepth = 8
peakAngle = 45

class buoyMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self,None,[self.seeingRed],[self.foundRed])
        self.vision.forward.redLightDetectOn()
    
    def startState(self):
        pass
        self.exit()