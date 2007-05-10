# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/frame.py

# Library Imports
import wx
import wx.aui

# Project Imports
import event
from gui.wxogre import wxOgreFrame
from gui.view import IPanelProvider
from core import ExtensionPoint
from module import ModuleManager

import gui.simview

class MainFrame(wx.Frame):
    """
    @type panel_providers: iterable
    @cvar panel_providers: A list of all classes which can provide panels for 
                           the gui.
                           
    @type _panels: {IModule : [wx.Window]}
    @ivar _panels: Keeps track of which modules own which panel so they can be
                   cleaned up module shutdown.
    """
    
    
    panel_providers = ExtensionPoint(IPanelProvider)
    
    def __init__(self, config):
        self._panels = {}
    
    
        # Instantiate super class based on configuration settings
        gui_node = config.get('GUI', {})
        position = gui_node.get('window_position',wx.DefaultPosition)
        size = gui_node.get('window_size', wx.Size(800,600))
        wx.Frame.__init__(self, None, -1, 'AVU Sim', position, size)

        # Set wxAUI
        self._mgr = wx.aui.AuiManager()
        self._mgr.SetManagedWindow(self)
       
        for mod in ModuleManager.get().itermodules():
            self._add_module(mod)
        
        self.SetMinSize(self.GetSize())
        self._mgr.Update()
        
        # Register module event handlers so we are aware of any future changes
        event.register_handlers({'MODULE_SHUTDOWN' : self._remove_module,
                                 'MODULE_CREATED' : self._add_module} )
        
    def _remove_module(self, mod):    
        """
        Close all panes attached to a module
        """
        for panel in self._panels[mod]:
            self._mgr.DetachPane(panel)
            panel.Close()
        
        del self._panels[mod]
            
    def _add_module(self, mod):
        # Check every provider to see if its supports this module
        for provider_type in self.panel_providers:
            provider = provider_type()
            if provider.supports_module(mod):
                
                # Grab all the panels from the provider and store them for later
                # cleanup on module shutdown
                panels = []
                
                for pane_info, panel in provider.get_panels(mod, self):
                    self._mgr.AddPane(panel, pane_info)
                    panels.append(panel)
                    
                self._panels[mod] = panels