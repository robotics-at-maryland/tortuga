# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/view.py

# Library Imports
import wx

# Project Imports
import core
import ext.core
import ram.gui.led
import sim.vision
import gui.view

class VisionPanel(wx.Panel):
    core.implements(gui.view.IPanelProvider)
    
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._x = None
        self._y = None
        self._bouyLED = None
        
        # Controls
        self._createControls()
        
        # Events
        conn = eventHub.subscribe(sim.vision.SimVision.LIGHT_FOUND, vision,
                                  self._onBouyFound)
        self._connections.append(conn)
        
        conn = eventHub.subscribe(sim.vision.SimVision.LIGHT_LOST, vision,
                                  self._onBouyLost)
        self._connections.append(conn)
        
    def _createControls(self):
        sizer =  wx.GridBagSizer(10, 10)
        
        # Buoy Text and Label
        label = wx.StaticText(self, label = "Bouy")
        sizer.Add(label, (0,0), flag = wx.ALIGN_CENTER)
        self._bouyLED = ram.gui.led.LED(self, state = 0)
        sizer.Add(self._bouyLED, (0, 1), flag = wx.ALIGN_CENTER)
        
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+0.000')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        # Create X controls
        desiredLabel = wx.StaticText(self, label = 'X:')
        sizer.Add(desiredLabel, (1, 0), flag = wx.ALIGN_CENTER)
        self._x = wx.TextCtrl(self, size = textSize, style = textStyle)
        sizer.Add(self._x, (1, 1), flag = wx.ALIGN_CENTER)
        
        # Create Y controls
        actualLabel = wx.StaticText(self, label = 'Y:')
        sizer.Add(actualLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._y = wx.TextCtrl(self, size = textSize, style = textStyle)
        sizer.Add(self._y, (2, 1), flag = wx.ALIGN_CENTER)
        
        self.SetSizerAndFit(sizer)
        
    def _createDataControl(self, controlName, pos, label):
        pass
        
    def _onBouyFound(self, event):
        self._x.Value = "% 4.2f" % event.x
        self._y.Value = "% 4.2f" % event.y
        
        # The LED only does work when you change state, so calling this mutiple
        # times with the same value is ok
        self._bouyLED.SetState(2)
    
    def _onBouyLost(self, event):
        self._x.Value = 'NA'
        self._y.Value = 'NA'
        self._bouyLED.SetState(0)
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = ext.core.Subsystem.getSubsystemOfType(
            ext.core.QueuedEventHub, subsystems)
        
        vision = ext.core.Subsystem.getSubsystemOfType(sim.vision.SimVision,
                                                       subsystems)
        
        if vision is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Vision")
            paneInfo = paneInfo.Caption("Vision").Left()
        
            panel = VisionPanel(parent, eventHub, vision)
            return [(paneInfo, panel, [vision])]
        
        return []