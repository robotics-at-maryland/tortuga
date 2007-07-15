from ai.modernAI.aiStateMachine import aiStateMachine
from controlTestMachine import controlTestMachine
from buoyMachine import buoyMachine
from ai.modernAI.visionTestMachine import visionTestMachine

class aiInitMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interrupts):
        print "Initializing the AI..."
        #firstMachine = controlTestMachine()
        #firstMachine = buoyMachine()
	firstMachine = visionTestMachine()
	self.branch(firstMachine,self.end)
        
    def end(self,args,interrupts):
        self.exit()