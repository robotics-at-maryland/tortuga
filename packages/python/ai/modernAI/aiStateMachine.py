from newStateMachine import stateMachine
import module.Module as Module
import module.ModuleManager as ModuleManager

class aiStateMachine(stateMachine):
    def __init__(self,args = None,interFuncs = None,interStates = None):
        self.controller = ModuleManager.get().get_module("Controller")
        self.vehicle = ModuleManager.get().get_module("Vehicle")

        stateMachine.__init__(self,args,interFuncs,interStates)
    
    def startState(self,args,interFuncs,interStates):
        raise Exception("start state needs to be defined")