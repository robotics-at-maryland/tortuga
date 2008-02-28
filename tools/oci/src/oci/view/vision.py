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
import ext.vision
import ram.gui.led
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
        self._generatedControls = []
        self._controlsShowing = True
        self._hide = None
        
        # Controls
        self._createControls()
        
        # Events
        conn = eventHub.subscribeToType(ext.vision.EventType.LIGHT_FOUND,
                                        self._onBouyFound)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ext.vision.EventType.LIGHT_LOST,
                                        self._onBouyLost)
        self._connections.append(conn)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
    def _createControls(self):
        # Creat box around controls
        box = wx.StaticBox(parent = self, label = "Bouy")
        topSizer = wx.StaticBoxSizer(box)
        
        self.sizer = wx.FlexGridSizer(0, 2, 10, 10)
        topSizer.Add(self.sizer, 1, wx.EXPAND)
        
        # Buoy Text and Label
        self._hide = wx.Button(self, label = "Hide")
        self.sizer.Add(self._hide, 1, flag = wx.ALIGN_CENTER)
        self._hide.Bind(wx.EVT_BUTTON, self._onButton)

        size = (self._getTextSize()[0], ram.gui.led.LED.HEIGHT)
        self._bouyLED = ram.gui.led.LED(self, state = 3, size = size)
        self._bouyLED.MinSize = size
        self.sizer.Add(self._bouyLED, 1, flag = wx.ALIGN_CENTER)
        
        # Create controls
        self._createDataControl(controlName = '_x', label = 'Y Pos: ')
        self._createDataControl(controlName = '_y', label = 'X Pos: ')
        self._createDataControl(controlName = '_azimuth', label = 'Azimuth: ')
        self._createDataControl(controlName = '_elevation',
                                label = 'Elvevation: ')
        self._createDataControl(controlName = '_range',  label = 'Range: ')

        # Start off greyed out
        for control in self._generatedControls:
            control.Enable(False)
        
        self.SetSizerAndFit(topSizer)

    def _getTextSize(self):
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+0.000')
        return wx.Size(textWidth, wx.DefaultSize.height)         
        
    def _createDataControl(self, controlName, label):
        textSize = self._getTextSize()
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        desiredLabel = wx.StaticText(self, label = label)
        self.sizer.Add(desiredLabel, 1, flag = wx.ALIGN_RIGHT)
        
        control = wx.TextCtrl(self, size = textSize, style = textStyle)
        setattr(self, controlName, control)
        self._generatedControls.append(control)
        self.sizer.Add(control, proportion = 1 , flag = wx.ALIGN_CENTER)

    def _onButton(self, event):
        if self._controlsShowing:
            self._hide.Label = "Show"
        else:
            self._hide.Label = "Hide"
        
        self._controlsShowing = not self._controlsShowing
        for i in xrange(2, (len(self._generatedControls) + 1) * 2):
            self.sizer.Show(i, self._controlsShowing)
        self.sizer.Layout()
        
    def _onBouyFound(self, event):
        for control in self._generatedControls:
            control.Enable()
        
        self._x.Value = "% 4.2f" % event.x
        self._y.Value = "% 4.2f" % event.y    
        self._azimuth.Value = "% 4.2f" % event.azimuth.valueDegrees()
        self._elevation.Value = "% 4.2f" % event.elevation.valueDegrees()
        self._range.Value = "% 4.2f" % event.range
        
        # The LED only does work when you change state, so calling this mutiple
        # times with the same value is ok
        self._bouyLED.SetState(2)
    
    def _onBouyLost(self, event):
        for control in self._generatedControls:
            control.Enable(False)
        
        self._bouyLED.SetState(0)
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = ext.core.Subsystem.getSubsystemOfType(
            ext.core.QueuedEventHub, subsystems)
        
        vision = ext.core.Subsystem.getSubsystemOfType(ext.vision.VisionSystem,
                                                       subsystems)
        
        if vision is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Vision")
            paneInfo = paneInfo.Caption("Vision").Left()
        
            panel = VisionPanel(parent, eventHub, vision)
            return [(paneInfo, panel, [vision])]
        
        return []
