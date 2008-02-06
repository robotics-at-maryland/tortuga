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
from oci.view.shell import ShellPanel

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
        self._panels = []
    
        # Instantiate super class based on configuration settings
        #gui_node = config.get('GUI', {})
        #position = gui_node.get('window_position',wx.DefaultPosition)
        #size = gui_node.get('window_size', wx.Size(800,600))

        position = wx.DefaultPosition
        size = wx.Size(800,600)
        title = 'OCI'
        wx.Frame.__init__(self, None, title = title, pos = position, 
                          size = size) 
        
        self._notebook = wx.aui.AuiNotebook(self)
        sizer = wx.BoxSizer()
        sizer.Add(self._notebook, 1, wx.EXPAND)
        self.SetSizer(sizer)

        # Add panels for all the current subsystems
        self._addShell(subsystems)
        self._addSubsystemPanels(subsystems)
 
        self.Bind(wx.EVT_CLOSE,self._onClose)            
    
    def _addShell(self, subsystems):
        introText = 'Current Subsystems:\n'
        # Build locals
        locals = {}
        for subsystem in subsystems:
            # Make first letter lower case
            name = subsystem.getName()
            name = name[0].lower() + name[1:]
            locals[name] = subsystem
            introText += '%s: %s\n' % (name, type(subsystem))
        
        shell = ShellPanel(self, locals = locals, introText = introText)
        paneInfo = wx.aui.AuiPaneInfo().Name("Shell")
        paneInfo = paneInfo.Caption("Shell").Left()
        self._addSubsystemPanel(paneInfo, shell, [])
    
    def _onClose(self, event):        
        for i in range(0, self._notebook.GetPageCount()):
            self._notebook.RemovePage(i)
            
        # Close all panels, if I don't do this the EVT_CLOSE handlers will 
        # never be called
        for panel in self._panels:
            panel.Close()
            
        # Let the normal handler of this event take over
        event.Skip()
        
#    def _remove_module(self, mod):    
#        """
#        Close all panes attached to a module
#        """
#        for panel in self._panels[mod]:
#            self._mgr.DetachPane(panel)
#            panel.Close()
#        
#        del self._panels[mod]
        
    def _addSubsystemPanels(self, subsystems):
        for provider in self.panelProviders:
            panelInfos = provider.getPanels(subsystems, self)
            for paneInfo, panel, sys in panelInfos:
                self._addSubsystemPanel(paneInfo, panel, sys)

    def _addSubsystemPanel(self, paneInfo, panel, usedSubsystems):
        self._notebook.AddPage(panel, paneInfo.caption)
        self._panels.append(panel)
