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
from ram.core import ExtensionPoint
import ext.core as core

import oci.view.panels  # Import needed for registration of IPanelProviders
from oci.view.shell import ShellPanel

import oci.view.vision

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
        wx.Frame.__init__(self, None, title = 'OCI')
        self._panels = []
        self._shell = None
        self._mgr = None
        
        # Determine config file location
        guiBasePath = wx.StandardPaths.Get().GetUserConfigDir()
        guiFileName = 'ramoci.yml'
        if wx.Platform == '__WXGTK__':
            guiFileName = '.' + guiFileName
        self.guiDataFile = os.path.abspath(os.path.join(guiBasePath, guiFileName))
        
        # Add panels for all the current subsystems
        self._mgr = wx.aui.AuiManager(self)
        self._addShell(subsystems)
        self._addSubsystemPanels(subsystems)
        self._mgr.Update()
        
        # Load configuration settings from disk
        try:
            # Load data file
            stream = file(self.guiDataFile, 'r')
            guiData = yaml.load(stream)    
            stream.close()
            
            # Load settings
            self.SetSize(guiData.get("windowSize", wx.Size(800, 600)))
            self.SetPosition(guiData.get("windowPos", wx.DefaultPosition))
            self._shell.history = guiData.get("shellHistory", [])
            
            if guiData.has_key("paneLayout"):
                self._mgr.LoadPerspective(guiData["paneLayout"])

            # Apply changes
            self._mgr.Update()    
        except IOError:
            pass # No history availible, continue 
        mb = self._buildMenuBar()
        self.SetMenuBar(mb)
        
        self.Bind(wx.aui.EVT_AUI_PANE_CLOSE, self._paneHide)
        self.Bind(wx.EVT_CLOSE,self._onClose)            


    def _buildMenuBar(self):
        menuBar = wx.MenuBar()
        menu = wx.Menu()
        for pane in self._mgr.GetAllPanes():
            menuItem = menu.AppendCheckItem(-1, pane.caption)
            menuItem.Check(pane.IsShown())
            self.Bind(wx.EVT_MENU, self._menuItemClick(pane), menuItem)
        menuBar.Append(menu, "&Panels")
        return menuBar
    
    def _paneHide(self,event):
        pane = event.GetPane()
        mb = self.GetMenuBar()
        menuItemId = mb.FindMenuItem("Panels",pane.caption)
        mb.Check(menuItemId, False) # The Aui button only closes panels
        mb.Refresh() # Redraw menubar
        event.Skip() # Proceed to closing the panel
        
    # Hide/Show Panes
    def _menuItemClick(self,pane):
        def handler(event):
            # event == wx.wxEVT_COMMAND_MENU_SELECTED
            if event.IsChecked():
                pane.Show()
            else:
                pane.Hide()
            self._mgr.Update()
        return handler

    
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
        
        # Create shell
        self._shell = ShellPanel(self, locals = locals, introText = introText)
        
        # This should not be bound, it makes it impossible to edit functions
        # You can rebind this once CTRL+UP/DOWN does what the old up down do
        # The old/up down let you navigate the text
        self._shell.Bind(wx.EVT_KEY_DOWN, self._onShellKeyPress)

        paneInfo = wx.aui.AuiPaneInfo().Name("Shell")
        paneInfo = paneInfo.Caption("Shell").Center()
        self._addSubsystemPanel(paneInfo, self._shell, [])
    
    def _onShellKeyPress(self,event):
        if event.KeyCode == wx.WXK_UP and event.ControlDown():
            self._shell.OnHistoryReplace(step=+1)
            return
        elif event.KeyCode == wx.WXK_DOWN and event.ControlDown():
            self._shell.OnHistoryReplace(step=-1)
            return
        event.Skip()
    
    def _onClose(self, event):  
        # Write Config YAML data
        layoutStream = file(self.guiDataFile, 'w+')
        guiData = {}
        guiData["windowSize"] = self.GetSize()
        guiData["windowPos"] = self.GetPosition()
        guiData["paneLayout"] = self._mgr.SavePerspective()
        guiData["shellHistory"] = self._shell.history
        yaml.dump(guiData,layoutStream) 
        layoutStream.close()
        
        # Close the gui
        allPanes = self._mgr.GetAllPanes()
        for pane in allPanes: 
            self._mgr.ClosePane(pane)    

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
        self._mgr.AddPane(panel, paneInfo, paneInfo.caption)
        self._panels.append(panel)
