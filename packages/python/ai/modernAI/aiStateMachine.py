from ai.modernAI.newStateMachine import stateMachine
from module import Module,ModuleManager

class aiStateMachine(stateMachine):
    def __init__(self,args = None,interrupts = None):
        self.controller = ModuleManager.get().get_module("Controller")
        self.vehicle = ModuleManager.get().get_module("Vehicle")
        self.vision = ModuleManager.get().get_module("Vision")

        stateMachine.__init__(self,args,interrupts)
    
    def startState(self,args,interrupts):
        raise Exception("start state needs to be defined")