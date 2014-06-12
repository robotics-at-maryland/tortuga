# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/module.py

"""
This encapsulates the module system.  It allows seperate parts of the program
to be aware of other portions starting and stopping without having to be 
explictly joined.
"""

# Move me into the core?

# Project Imports
import ram.event as event
from ram.core import Singleton, Interface, verifyClass, Attribute, implements, Component
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
    
    def __init__(self, config):
        self.name = config['name']
        self.type = type(self)
        self._depends = []
        self._running = False
        
        event.send('MODULE_CREATED', self)
        ModuleManager.get().register(self)
    
    def start(self):
        self._running = True
        event.send('MODULE_START', self)
        
    def backgrounded(self):
        self._running
        
    def pause(self):
        self._running = False
        event.send('MODULE_PAUSE', self)
        
    def update(self, time_since_last_update):
        pass
        
    def shutdown(self):
        self.pause()
        event.send('MODULE_SHUTDOWN',self)

class ModuleManager(Singleton):
    """
    @ivar _registry: A dict which maps module name to module instances
    """
    
    def init(self, config = None, load_func = None):
        """
        @type  config: dict like
        @param config: Contains a module section, with information on how to
                       create the different modules of the system.
                       
        @type  load_func: function
        @param load_func: Called to load every module, first arg mod class,
                          second is the config section
        """
        self._registry = {}
        self._mod_type_reg = {}
        
        if load_func is None:
            self._load_func = self._load_module
        else:
            self._load_func = load_func
            
        if config is not None:
            self.load_modules(config)
        
    def load_modules(self, config):
        """
        @type  config: dict like
        @param config: The section of the configuration containing the modules
        """
        #print 'loading modules'
        
        mod_nodes = config.get('Modules', None)
        # Maps module -> modules it dependens on
        mod_depends = {}
        # Maps module -> module which depend on it
        mod_dependees = {}
        
        # Iterate over nodes and build dependency list
        if mod_nodes is not None:
            for name, config_node in mod_nodes.iteritems():
                class_name = config_node['type']
                config_node['name'] = name
                
                # TODO: some kind of error checking
                mod_type = Component.get_class(IModule, class_name)
        
                # Do not load module if we have already loaded it
                if not self._registry.has_key(name):
                    # Add this module to the list of dependees for each each module
                    # type it depends on
                    depends = config_node.get('depends_on', [])
                    if type(depends) is not list:
                        raise TypeError, "'depends_on' must be a list not %s" % type(depends)
                    for dep in depends:
                        # TODO: Put some error handling in here
                        if not mod_nodes.has_key(dep):
                            raise KeyError, 'No module called "%s" could be found' % dep
                        mod_dependees.setdefault(dep, []).append(name)
                    # Set dependents for this module
                    mod_depends[name] = (depends, config_node, mod_type)
        
        
        while len(mod_depends) != 0:
            # Assume we have a cycle
            cycle = True
        
            for name, (depends, config_node, mod_type) in mod_depends.items():
                # If its a free standing module load it
                if len(depends) == 0:
                    # Create module (registration done by Module constructor)
                    self._load_func(mod_type, config_node)
                    
                    # Update all the dependents lists to remove this module
                    for dependent in mod_dependees.get(name, []):
                        deps, conf_node, mod_type = mod_depends[dependent]
                        deps.remove(name)
                        mod_depends[dependent] = (deps, conf_node, mod_type)

                    # Remove module from list
                    del mod_depends[name]
                    
                    cycle = False

            # Raise error on cycle
            if cycle:
                raise "Cycle found in module dependencies"
                
    
    def _load_module(self, mod_type, mod_config):
        """
        Default module loading function
        """
        mod_type(mod_config)
        
        
    def register(self, mod):
        """
        Registers the module.
        """
        
        verifyClass(IModule, type(mod))

        self._registry[mod.name] = mod
        self._mod_type_reg.setdefault(type(mod),[]).append(mod)
        
    def unregister(self, mod):
        """
        Unregisters the moduile and tells it to shudown.
        """       
        del self._registry[mod.name]
        self._mod_type_reg[type(mod)].remove(mod)
        
        mod.shutdown()
        
    def get_module(self, name):
        """
        Returns the module with the given name/type, or None
        
        @type  name: str or type
        @param name: The given name of the module, or its type (ie class)
        
        @rtype:  IModule or [IModule]
        @return: If given a name it will be a single module or none, if given
                 a type it will be a (possible empty) list.
        """
        if type is type(name):
            return self._mod_type_reg.get(name, [])
        else:
            return self._registry.get(name, None)
        
    def itermodules(self):
        for mod in self._registry.itervalues():
            yield mod
        
        
