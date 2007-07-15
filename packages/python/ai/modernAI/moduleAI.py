from module import Module,ModuleManager,IModule
from core import implements
from aiInitMachine import aiInitMachine

class AI(Module):
    implements(IModule)
    
    def __init__(self,veh, config):
        self.vehicle = veh
        self.controller = ModuleManager.get().get_module("Controller")
        
        self.stateMachine = aiInitMachine()
        self.complete = False
        Module.__init__(self,config)
        
    def update(self,timeStep):
        if not self.complete:
            self.complete = self.stateMachine.operate()
            if self.complete:
                "Shutting down AI module"
                self.shutdown()