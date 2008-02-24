# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/oci/src/frame.py

# STD Imports
import os

# Library Imports
import wx
import wx.aui
import yaml

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
        self.guiDataFile = os.path.abspath(os.path.join(wx.StandardPaths.Get().GetUserConfigDir(), 'guidata.yml'))
        
        # Instantiate super class based on configuration settings
        #gui_node = config.get('GUI', {})
        #position = gui_node.get('window_position',wx.DefaultPosition)
        #size = gui_node.get('window_size', wx.Size(800,600))

        position = wx.DefaultPosition
        size = wx.Size(800,600)
        title = 'OCI'
        wx.Frame.__init__(self, None, title = title, pos = position, 
                          size = size) 
        
        self._mgr = wx.aui.AuiManager(self)

        # Add panels for all the current subsystems
        self._addShell(subsystems)
        self._addSubsystemPanels(subsystems)
        self._mgr.Update()
        
        try:
            stream = file(self.guiDataFile, 'r')
            guiData = yaml.load(stream)    
            stream.close()
            self.SetSize(guiData["windowSize"])
            self.SetPosition(guiData["windowPos"])
            self._shell.history = guiData["shellHistory"]
            self._mgr.LoadPerspective(guiData["paneLayout"])
            all_panes = self._mgr.GetAllPanes()
            for pane in all_panes:
                pane.Show()
            self._mgr.Update()    
        except Exception, e:
            self._shell.write("Could not read/load yaml layout: "  + e) # Test this line
        
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
        shell.Bind(wx.EVT_KEY_DOWN, self._onShellKeyPress)
        locals["shell"] = shell # for testing

        paneInfo = wx.aui.AuiPaneInfo().Name("Shell")
        paneInfo = paneInfo.Caption("Shell").Left()
        self._addSubsystemPanel(paneInfo, shell, [])
    
    def _onShellKeyPress(self,event):
        if event.KeyCode ==  wx.WXK_UP:
            self._shell.OnHistoryReplace(step=+1)
            return
        elif event.KeyCode == wx.WXK_DOWN:
            self._shell.OnHistoryReplace(step=-1)
            return
        event.Skip()
    
    def _onClose(self, event):  
        allPanes = self._mgr.GetAllPanes()
        for pane in allPanes: 
            self._mgr.ClosePane(pane)    

        # Write YAML data
        layoutStream = file(self.guiDataFile, 'w+')
        guiData = {}
        guiData["windowSize"] = self.GetSize()
        guiData["windowPos"] = self.GetPosition()
        guiData["paneLayout"] = self._mgr.SavePerspective()
        guiData["shellHistory"] = self._shell.history
        
        yaml.dump(guiData,layoutStream) 
        layoutStream.close()
        
        self._mgr.UnInit()
     
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
        if paneInfo.caption == "Thrusters":
            paneInfo.Center()
        elif paneInfo.caption == "Depth":
            paneInfo.Right()
        elif paneInfo.caption == "Shell":
            self._shell = panel
        elif paneInfo.caption == "Orientation":
            paneInfo.Center()
        elif paneInfo.caption == "Demo Sonar":
            paneInfo.Left()
        elif paneInfo.caption == "Demo Power":
            paneInfo.Left()
        
        self._mgr.AddPane(panel, paneInfo, paneInfo.caption)
        self._panels.append(panel)
        