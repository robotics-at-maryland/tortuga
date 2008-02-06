from aim.module import Module, ModuleManager

class model:
    def __init__(self):
        self.vehicle = ModuleManager.get().get_module("Vehicle")
        self.controller = ModuleManager.get().get_module("Controller")
        #self.vision = ModuleManager.get().get_module("Vision")
        
        