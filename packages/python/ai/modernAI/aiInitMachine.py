from aiStateMachine import aiStateMachine
from controlTestMachine import controlTestMachine

class aiInitMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interrupts):
        print "Initializing the AI..."
        firstMachine = controlTestMachine()
        self.branch(firstMachine,self.end)
        
    def end(self,args,interrupts):
        self.exit()