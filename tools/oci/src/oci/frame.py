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
from ram.gui.view import IPanelProvider
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
        self._layoutCfgs = None
        self._shell = None
        self._mgr = None
        
        # Determine config file location
        guiBasePath = wx.StandardPaths.Get().GetUserConfigDir()
        guiFileName = config.get('guiConfigFile', 'ramoci.yml')
        if wx.Platform == '__WXGTK__':
            guiFileName = '.' + guiFileName
        self.guiDataFile = os.path.abspath(os.path.join(guiBasePath, guiFileName))

        # Add panels for all the current subsystems
        self._mgr = wx.aui.AuiManager(self)
        self._addShell(config, subsystems)
        self._addSubsystemPanels(subsystems)
        self._mgr.Update()
        
        # Load configuration settings from disk
        guiData = {}
        try:
            # Load data file
            stream = file(self.guiDataFile, 'r')
            guiData = yaml.load(stream)    
            stream.close()
        except (IOError, yaml.YAMLError):
            pass # No history file availale, continue 
        
        # Load settings from the configuration file
        self._layoutCfgs = guiData.get('Layouts', {})
        self._loadLayout(self._layoutCfgs.get('Default', {}))
        self._shell.history = guiData.get("shellHistory", [])

        # Create the menu bar
        mb = self._buildMenuBar()
        self.SetMenuBar(mb)
        
        # Update the events
        self.Bind(wx.aui.EVT_AUI_PANE_CLOSE, self._onPaneHide)
        self.Bind(wx.EVT_CLOSE,self._onClose)

    # Helper functions
    def _buildMenuBar(self):
        menuBar = wx.MenuBar()
        
        # Create the panels menu
        menu = wx.Menu()
        for pane in self._mgr.GetAllPanes():
            menuItem = menu.AppendCheckItem(-1, pane.caption)
            menuItem.Check(pane.IsShown())
            self.Bind(wx.EVT_MENU, self._menuItemClick(pane), menuItem)
        menuBar.Append(menu, "&Panels")
        
        # Create the layouts menu
        menu = wx.Menu()

        menuItem = menu.Append(-1, '&Save Layout\tCtrl+S')
        self.Bind(wx.EVT_MENU, self._onSaveLayout, menuItem)
        menuItem = menu.Append(-1, '&Load Layout\tCtrl+L')
        self.Bind(wx.EVT_MENU, self._onLoadLayout, menuItem)
        menuItem = menu.Append(-1, '&Delete Layout\tCtrl+D')
        self.Bind(wx.EVT_MENU, self._onDeleteLayout, menuItem)

        menu.AppendSeparator()
        
        for name in self._layoutCfgs.iterkeys():
            self._addLayoutMenuItem(menu, name)
        menuBar.Append(menu, "&Layouts")
        
        return menuBar
    
    def _menuItemClick(self,pane):
        """Creates a custom handler that loads the given pane Hide/Show Panes"""
        def handler(event):
            # event == wx.wxEVT_COMMAND_MENU_SELECTED
            if event.IsChecked():
                pane.Show()
            else:
                pane.Hide()
            self._mgr.Update()
        return handler
    
    def _layoutChange(self, name):
        """Creates a custom handler the loads the given layout"""
        def handler(event):
            self._loadLayout(self._layoutCfgs[name])
        return handler
    
    def _addLayoutMenuItem(self, menu, name):
        menuItem = menu.Append(-1, name)
        self.Bind(wx.EVT_MENU, self._layoutChange(name), menuItem)
    
    def _addShell(self, config, subsystems):
        introText = 'Current Subsystems:\n'
        # Build locals
        locals = {}

        for subsystem in subsystems:
            # Make first letter lower case
            name = subsystem.getName()
            name = name[0].lower() + name[1:]
            locals[name] = subsystem
            introText += '%s: %s\n' % (name, type(subsystem))
        
        # Load shell init file
        shellBasePath = os.environ['RAM_SVN_DIR']
        shellInitFile = config.get('shellInitFile', 
                                       'tools/oci/src/shellinit.py')
        shellinit = os.path.abspath(os.path.join(shellBasePath, shellInitFile))

        # Check if the shell init file exists
        if not os.path.exists(shellinit):
            introText += "\nWARNING: No file found named %s\n" % shellinit
            shellinit = None

        # Create shell
        redirect = config.get('redirect', True)
        self._shell = ShellPanel(self, locals = locals, introText = introText,
                                 startup = shellinit, redirect = redirect)
        
        # This should not be bound, it makes it impossible to edit functions
        # You can rebind this once CTRL+UP/DOWN does what the old up down do
        # The old/up down let you navigate the text
        self._shell.Bind(wx.EVT_KEY_DOWN, self._onShellKeyPress)

        paneInfo = wx.aui.AuiPaneInfo().Name("Shell")
        paneInfo = paneInfo.Caption("Shell").Center()

        self._addSubsystemPanel(paneInfo, self._shell, [])
    
    def _loadLayout(self, guiData):
        """
        Loads the desired layout from the given configuration data, and sets 
        the window size/position and all sub pane positions.  
        """
        
        # Load window size position
        self.SetSize(guiData.get("windowSize", wx.Size(800, 600)))
        self.SetPosition(guiData.get("windowPos", wx.DefaultPosition))

        # Attempt to load the pane data
        if guiData.has_key("paneLayout"):
            self._mgr.LoadPerspective(guiData["paneLayout"])
            # Apply changes
            self._mgr.Update()    

    def _getLayoutData(self):
        """
        Saves the current window position/size and pane layout and returns it 
        as dict. 
        """
        guiData = {}
        guiData["windowSize"] = self.GetSize()
        guiData["windowPos"] = self.GetPosition()
        guiData["paneLayout"] = self._mgr.SavePerspective()
        return guiData
        
    def _addSubsystemPanels(self, subsystems):
        for provider in self.panelProviders:
            panelInfos = provider.getPanels(subsystems, self)
            for paneInfo, panel, sys in panelInfos:
                self._addSubsystemPanel(paneInfo, panel, sys)

    def _addSubsystemPanel(self, paneInfo, panel, usedSubsystems): 
        self._mgr.AddPane(panel, paneInfo, paneInfo.caption)
        self._panels.append(panel)
    
    # Event handlers
    def _onShellKeyPress(self,event):
        if event.KeyCode == wx.WXK_UP and event.ControlDown():
            self._shell.OnHistoryReplace(step=+1)
            return
        elif event.KeyCode == wx.WXK_DOWN and event.ControlDown():
            self._shell.OnHistoryReplace(step=-1)
            return
        event.Skip()
    
    def _onPaneHide(self,event):
        pane = event.GetPane()
        mb = self.GetMenuBar()
        menuItemId = mb.FindMenuItem("Panels",pane.caption)
        mb.Check(menuItemId, False) # The Aui button only closes panels
        mb.Refresh() # Redraw menubar
        event.Skip() # Proceed to closing the panel

    def _onSaveLayout(self, event):
        name = wx.GetTextFromUser(message = 'Enter the name of the layout',
                                  caption = 'Save Layout', parent = self)
        
        if 0 != len(name):
            # Save the data, update the menu
            newLayout = not self._layoutCfgs.has_key(name)
            self._layoutCfgs[name] = self._getLayoutData()
            
            if newLayout:
                mb = self.GetMenuBar()
                menu = mb.GetMenu(mb.FindMenu("Layouts"))
                self._addLayoutMenuItem(menu, name)
                mb.Refresh()

    def _onLoadLayout(self, event):
        choice = wx.GetSingleChoice(message = 'Select layout to load',
                                    caption = 'Load Layout', parent = self,
                                    choices = self._layoutCfgs.keys())
        
        if 0 != len(choice):
            self._loadLayout(self._layoutCfgs[choice])

    def _onDeleteLayout(self, event):
        choice = wx.GetSingleChoice(message = 'Select layout to delete',
                                    caption = 'Delete Layout', parent = self,
                                    choices = self._layoutCfgs.keys())
        
        if 0 != len(choice):
            # Remove item from menu
            mb = self.GetMenuBar()
            menu = mb.GetMenu(mb.FindMenu("Layouts"))
            menuItemId = menu.FindItem(choice)
            menu.Delete(menuItemId)
            mb.Refresh()
            
            # Remove from cfgs
            del self._layoutCfgs[choice]

    def _onClose(self, event):  
        # Read old data
        guiData = {}
        try:
            # Load data file
            stream = file(self.guiDataFile, 'r')
            guiData = yaml.load(stream)    
            stream.close()
        except (IOError, yaml.YAMLError):
            pass # No history file availale, continue 
        
        # Update the layouts
        self._layoutCfgs['Default'] = self._getLayoutData()
        guiData['Layouts'] = self._layoutCfgs
        guiData['shellHistory'] = self._shell.history
        
        # Write Config YAML data
        layoutStream = file(self.guiDataFile, 'w+')
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
        
    
