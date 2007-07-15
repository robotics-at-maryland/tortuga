from aiStateMachine import aiStateMachine
from controlTestMachine import controlTestMachine

class aiInitMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        print "Initializing the AI..."
        firstMachine = controlTestMachine()
        self.branch(firstMachine,self.end)
        
    def end(self,args,interFuncs,interStates):
        self.exit()