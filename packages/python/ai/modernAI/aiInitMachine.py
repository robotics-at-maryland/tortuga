from aiStateMachine import aiStateMachine
from gateMachine import gateMachine

class aiInitMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        print "Initializing the AI..."
        firstMachine = gateMachine()
        self.branch(firstMachine,self.end)
        
    def end(self,args,interFuncs,interStates):
        self.exit()