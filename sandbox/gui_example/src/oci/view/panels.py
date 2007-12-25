

# Library Imports
import wx

# Project Imports
from core import Component, implements
from gui.view import IPanelProvider

import oci.model.subsystem as subsystemMod

def getSubsystemOfType(subsystems, subsystemType):
    for subsystem in subsystems:
        if isinstance(subsystem, subsystemType):
            return subsystem
    return None

class DemoPowerPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, power, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        
        # Create Controls
        text = wx.StaticText(self, label = 'Power Level', size = (100,20))
        self._gauge = wx.Gauge(self, range =  100, size = (300, 25))
        
        # Vertical sizer
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text)
        sizer.Add(self._gauge)
        
        # Hook size up to the panel
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.SetMinSize(sizer.GetMinSize())

        # Connect to events
        self.Bind(wx.EVT_CLOSE, self._onClose)
        conn = power.subscribe(subsystemMod.DemoPower.POWER_UPDATE, 
                               self._update)
        self._connections.append(conn)

        
    def _onClose(self, closeEvent):
        """
        Disconnects from C++ events
        
        Ensure that functions of this object won't get called after its already
        disctructed.
        """
        for conn in self._connections:
            conn.disconnect()
            
    def _update(self, event):
        """
        Called when the power subsystem updates the power cosumption
        """
        self._gauge.SetValue(event.power)
        
    @staticmethod
    def getPanels(subsystems, parent):
        """
        Creates 
        """
        power = getSubsystemOfType(subsystems, subsystemMod.DemoPower)
        
        paneInfo = wx.aui.AuiPaneInfo().Name("Demo Power")
        paneInfo = paneInfo.Caption("Demo Power").Left()
        
        return [(paneInfo, DemoPowerPanel(parent, power), [power])]
    

class DemoSonarPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, sonar, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        
        # Create Controls
        text = wx.StaticText(self, label = 'Sonar Position', size = (100,20))
        
        xLabel = wx.StaticText(self, label = 'X: ')#, size = (40,20))
        yLabel = wx.StaticText(self, label = 'Y: ')#, size = (40,20))
        
        self._xText = wx.TextCtrl(self, value = '', size = (80,20), 
                                  style = wx.TE_READONLY)
        self._yText = wx.TextCtrl(self, value = '', size = (80,20),
                                  style = wx.TE_READONLY)
        
        # Horizontal Postion sizer
        horizontalSizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontalSizer.Add(xLabel)
        horizontalSizer.Add(self._xText)
        horizontalSizer.Add(yLabel)
        horizontalSizer.Add(self._yText)
        
        # Vertical sizer
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text)
        sizer.Add(horizontalSizer)
        
        # Hook size up to the panel
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.SetMinSize(sizer.GetMinSize())
        
        
        # Connect to events
        self.Bind(wx.EVT_CLOSE, self._onClose)
        conn = sonar.subscribe(subsystemMod.DemoSonar.SONAR_UPDATE, 
                               self._update)
        self._connections.append(conn)

    def _onClose(self, closeEvent):
        """
        Disconnects from C++ events
        
        Ensure that functions of this object won't get called after its already
        disctructed.
        """
        for conn in self._connections:
            conn.disconnect()
            
    def _update(self, event):
        """
        Called when the power subsystem updates the power cosumption
        """
        self._xText.SetValue(str(event.x))
        self._yText.SetValue(str(event.y))

    @staticmethod
    def getPanels(subsystems, parent):
        sonar = getSubsystemOfType(subsystems, subsystemMod.DemoSonar)
        
        paneInfo = wx.aui.AuiPaneInfo().Name("Demo Sonar")
        paneInfo = paneInfo.Caption("Demo Sonar").Left()
        
        return [(paneInfo, DemoSonarPanel(parent, sonar), [sonar])]
