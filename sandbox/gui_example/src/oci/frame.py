# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/oci/src/frame.py

# Library Imports
import wx
import wx.aui

# Project Imports
from gui.view import IPanelProvider
from core import ExtensionPoint
import ext.core as core

import oci.view.panels  # Import needed for registration of IPanelProviders

class MainFrame(wx.Frame):
    """
    @type panel_providers: iterable
    @cvar panel_providers: A list of all classes which can provide panels for 
                           the gui.
                           
    @type _panels: {core.Subsystem : [wx.Window]}
    @ivar _panels: Keeps track of which modules own which panel so they can be
                   cleaned up module shutdown.
    """
    
    
    panelProviders = ExtensionPoint(IPanelProvider)
    
    def __init__(self, config, subsystems):
        self._panels = {}
    
        # Instantiate super class based on configuration settings
        #gui_node = config.get('GUI', {})
        #position = gui_node.get('window_position',wx.DefaultPosition)
        #size = gui_node.get('window_size', wx.Size(800,600))

        position = wx.DefaultPosition
        size = wx.Size(800,600)
        title = 'OCI'
        wx.Frame.__init__(self, None, wx.ID_ANY, title, position, size)

        # Set wxAUI
        self._mgr = wx.aui.AuiManager()
        self._mgr.SetManagedWindow(self)

        # Add panels for all the current subsystems
        self._addSubsystemPanels(subsystems)
        #for mod in ModuleManager.get().itermodules():
        #    self._add_module(mod, True)
        
        self.SetMinSize(self.GetSize())
        self._mgr.Update()
        
        # Register module event handlers so we are aware of any future changes
        #event.register_handlers({'MODULE_SHUTDOWN' : self._remove_module,
        #                         'MODULE_CREATED' : self._add_module} )
        
    def _remove_module(self, mod):    
        """
        Close all panes attached to a module
        """
        for panel in self._panels[mod]:
            self._mgr.DetachPane(panel)
            panel.Close()
        
        del self._panels[mod]

    def _addSubsystemPanels(self, subsystems):
        for provider in self.panelProviders:
            for paneInfo, panel, sys in provider.getPanels(subsystems, self):
                self._addSubsystemPanel(paneInfo, panel, sys, True)

    def _addSubsystemPanel(self, paneInfo, panel, usedSubsystems,
                           batch = False):
        
        self._mgr.AddPane(panel, paneInfo)

        if not batch:
            self._mgr.Update()
