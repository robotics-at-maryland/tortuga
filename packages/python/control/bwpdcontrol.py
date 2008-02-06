# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  control/bwpdcontrol.py

# Python Imports
import warnings

# Project Imports
from core import Component, implements
from ram.module import IModule, ModuleManager
import event

warnings.simplefilter('ignore', RuntimeWarning)
import ext.pattern
import ext.control
from ext.control import BWPDController as _BWPDController
from ext.core import ConfigNode as _ConfigNode
warnings.simplefilter('default', RuntimeWarning)

class BWPDController(_BWPDController, Component):
    implements(IModule)
    
    def __init__(self, vehicle, config):
        # Create C++ super class
        _BWPDController.__init__(self, vehicle, _ConfigNode.fromString(str(config)))
        
        # Module variabls
        self.name = config['name']
        self.type = type(self)
        self._depends = []
        self._running = False
        self._devices = {}
        
        self._config = config
    
        # Create module base class, send off events
        #print 'Initing'    
        event.send('MODULE_CREATED', self)
        ModuleManager.get().register(self)
        #print 'Done'
        
    def start(self):
        self.background(self._config['update_interval'])
        event.send('MODULE_START', self)    
        
    def pause(self):
        self.unbackground()
        self._running = False
        event.send('MODULE_PAUSE', self)
        
    def shutdown(self):
        self.pause()
        # Put more shutdown stuff here

        # Make sure to get rid of reference loops
        del self._devices
        
        event.send('MODULE_SHUTDOWN',self)