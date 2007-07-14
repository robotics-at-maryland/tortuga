from aiStateMachine import aiStateMachine

class aiInitMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        
    def startState(self,args,interFuncs,interStates):
        print "Initializing the AI..."
        self.exit()