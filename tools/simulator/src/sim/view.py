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
        self._azimuth = None
        self._elevation = None
        self._range = None
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
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
    def _onClose(self):
        for conn in self._connections:
            conn.disconnect()
        
    def _createControls(self):
        sizer =  wx.GridBagSizer(10, 10)
        
        # Buoy Text and Label
        label = wx.StaticText(self, label = "Bouy")
        sizer.Add(label, (0,0), flag = wx.ALIGN_CENTER)
        self._bouyLED = ram.gui.led.LED(self, state = 0)
        sizer.Add(self._bouyLED, (0, 1), flag = wx.ALIGN_CENTER)
        
        # Create controls
        self._createDataControl(sizer = sizer, controlName = '_x', 
                                pos = (1, 0), label = 'Y Pos: ')
        self._createDataControl(sizer = sizer, controlName = '_y', 
                                pos = (2, 0), label = 'X Pos: ')
        self._createDataControl(sizer = sizer, controlName = '_azimuth', 
                                pos = (3, 0), label = 'Azimuth: ')
        self._createDataControl(sizer = sizer, controlName = '_elevation', 
                                pos = (4, 0), label = 'Elvevation: ')
        self._createDataControl(sizer = sizer, controlName = '_range', 
                                pos = (5, 0), label = 'Range: ')
        
        self.SetSizerAndFit(sizer)
        
    def _createDataControl(self, sizer, controlName, pos, label):
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+0.000')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        desiredLabel = wx.StaticText(self, label = label)
        sizer.Add(desiredLabel, pos, flag = wx.ALIGN_RIGHT)
        
        control = wx.TextCtrl(self, size = textSize, style = textStyle)
        setattr(self, controlName, control)
        
        
        sizer.Add(control, (pos[0], pos[1] + 1) , flag = wx.ALIGN_CENTER)
        
        
    def _onBouyFound(self, event):
        self._x.Value = "% 4.2f" % event.x
        self._y.Value = "% 4.2f" % event.y    
        self._azimuth.Value = "% 4.2f" % event.azimuth
        self._elevation.Value = "% 4.2f" % event.elevation
        self._range.Value = "% 4.2f" % event.range
        
        # The LED only does work when you change state, so calling this mutiple
        # times with the same value is ok
        self._bouyLED.SetState(2)
    
    def _onBouyLost(self, event):
        self._x.Value = 'NA'
        self._y.Value = 'NA'
        self._azimuth.Value = 'NA'
        self._elevation.Value = 'NA'
        self._range.Value = 'NA'
        
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