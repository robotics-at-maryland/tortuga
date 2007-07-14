import module.Module as Module
import module.ModuleManager as ModuleManager
from aiInitMachine impot initMachine

class AI(module):
    implements(IModule)
    
    def __init__(self,veh, config):
        self.vehicle = veh
        self.controller = ModuleManager.get().get_module("Controller")
        
        self.stateMachine = initMachine()
        
        Module.__init__(self,config)
        
    def update(self,timeStep):
        complete = self.stateMachine.operate()
        if complete:
            print "AI module is complete"
            self.shutdown()