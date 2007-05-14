# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  module.py

"""
This encapsulates the module system.  It allows seperate parts of the program
to be aware of other portions starting and stopping without having to be 
explictly joined.
"""

# Move me into the core?

# Project Imports
import event
from core import Singleton, Interface, verifyClass, Attribute, implements, Component
from decorator import decorate, make_weak_signature

event.add_event_types(['MODULE_CREATED','MODULE_START', 'MODULE_PAUSE', 
                       'MODULE_SHUTDOWN'])

class IModule(Interface):
    """
    A module can be started and stopped.
    """
    def start():
        """
        If threaded this will cause the module to call there update function
        in a contious loop.  In synchonous mode this will enable the update
        function.
        
        Raises MODULE_START event.
        """
        pass
    
    def pause():
        """
        If threaded this will cause the background thread to cease calling of
        update.  If synchonous this will disable the update function.
        
        Raises MODULE_PAUSE event.
        """
        pass
    
    def backgrounded():
        """
        Returns true if this module is updating itself with a background thread.
        This means you don't have to, and shouldn't explicitly call its update
        function.
        """
        pass
    
    def update(time_since_last_update):
        """
        If the module is paused this should do nothing.
        This should not cause problems if the module is shutdown.
        """
        pass
    
    def shutdown():
        """
        Executes a pause then permantly releases all resouces.  In possible to
        restart.
        
        Raises MODULE_SHUTDOWN event.
        """
        pass
    
    name = Attribute(
         """
         A unique string identifier for the module.
         """)
        
    type = Attribute(
         """
         The type of the module, used in dependency tracking.
         """)
        
def register_module(module_name):
    """
    This decorator will automatically register you module after the init 
    function has been called.
    """
    def wrapper(func):
        def new_init(func, self, *args, **kwargs):
            func(self, *args, **kwargs)
            self.name = module_name
            
        return decorate(func, new_init, make_weak_signature(func))
    return wrapper


class Module(Component):
    implements(IModule)
    
    def __init__(self, _type, name):
        self.name = name
        self.type = _type
        self._running = False
        
        event.send('MODULE_CREATED', self)
        ModuleManager.get().register(self)
    
    def start(self):
        self._running = True
        event.send('MODULE_START', self)
        
    def backgrounded(self):
        return False
        
    def pause(self):
        self._running = False
        event.send('MODULE_PAUSE', self)
        
    def update(self, time_since_last_update):
        pass
        
    def shutdown(self):
        self.pause()
        event.send('MODULE_SHUTDOWN',self)

class ModuleManager(Singleton):
    
    def init(self, config = None):
        self._registry = {}
        self._mod_type_reg = {}
        #self._registry_mod = {}
        
        if config is not None:
            self.load_modules(config)
        
    def load_modules(self, config):
        """
        @type  config: dict like
        @param config: The section of the configuration containing the modules
        """
        print 'loading modules'
        # Current does not dependency checking, just loads the modules in a 
        # random order
        mod_nodes = config.get('Modules', None)
        if mod_nodes is not None:
            for name, node in mod_nodes.iteritems():
                class_name = node['type']
                print 'Loading Module of type',class_name
                # Note we don't register the object now becase the objects own
                # constructor does that
                Component.create(IModule, class_name, node)
                
        
    def register(self, mod):
        """
        Registers the module and tells is to start operation.
        """
        
        verifyClass(IModule, type(mod))
        print 'Registory',type(mod),type(mod.name)

        self._registry[mod.name] = mod
        #self._registry_mod[mod] = mod.name
        self._mod_type_reg.setdefault(mod.type,[]).append(mod)
        mod.start()
        
    def unregister(self, mod):
        """
        Unregisters the moduile and tells it to shudown.
        """       
        del self._registry[mod.name]
        #del self._registry_mod[mod]
        self._mod_type_reg[mod.type].remove(mod)
        
        mod.shutdown()
        
    def get_module(self, name):
        """
        Returns the module with the given name, or None
        """
        return self._registry.get(name, None)
        
    def itermodules(self):
        for mod in self._registry.itervalues():
            yield mod
        
        