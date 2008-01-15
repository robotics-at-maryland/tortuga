

# Library Imports
import wx

# Project Imports
from core import Component, implements
from gui.view import IPanelProvider
from oci.view.controls import DepthBar, ThrusterBar, RotationCtrl
import ext.vehicle
import ext.vehicle.device
import ext.core

import oci.model.subsystem as subsystemMod
def getSubsystemOfType(subsystems, subsystemType):
    for subsystem in subsystems:
        if isinstance(subsystem, subsystemType):
            return subsystem
    return None


class ThrusterPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, thrusters, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._thrusterList = []
        self._thrusterList = thrusters
        
        layout =  wx.GridBagSizer(10, 10)
        
        pos = 0
  
        for item in self._thrusterList:
            label = wx.StaticText(self, wx.ID_ANY, 
                                  item.getName().replace('Thruster',''))
            bar = ThrusterBar(self)           
            layout.Add(label, (0,pos),flag = wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(bar, (1,pos), flag = wx.EXPAND)
            layout.AddGrowableCol(pos)
            conn = item.subscribe(ext.vehicle.device.IThruster.FORCE_UPDATE, 
                                  self._update(bar))
            self._connections.append(conn)
            pos+=1
        
        layout.AddGrowableRow(1)
        self.SetSizerAndFit(layout)
        
    def _update(self, bar):
        def handler(event):
            bar.setVal(event.force)
        return handler
    
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
        closeEvent.Skip()
    
    @staticmethod
    def getPanels(subsystems, parentFunc):
        vehicle = ext.core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            # Get All the current Thrusters the vehicle has
            thrusters = []
            for name in vehicle.getDeviceNames():
                device = vehicle.getDevice(name)
                if isinstance(device, ext.vehicle.device.IThruster):
                    thrusters.append(device)
            
            
            paneInfo = wx.aui.AuiPaneInfo().Name("Thrusters")
            paneInfo = paneInfo.Caption("Thrusters").Left()
    
            return [(paneInfo, ThrusterPanel(parentFunc(), thrusters), 
                     [vehicle])]
            
        return []

class DepthPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, vehicle, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []
        
        layout =  wx.GridBagSizer(10, 10)        
        label = wx.StaticText(self,-1,"Depth")    
        self._depthbar = DepthBar(self)
    
        layout.Add(label, (0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self._depthbar, (1,0),flag=wx.EXPAND)
        
        layout.AddGrowableCol(0)
        layout.AddGrowableRow(1)
        
        self.SetSizerAndFit(layout)
        self.SetSizeHints(0,0,100,-1)
        
        conn = vehicle.subscribe(ext.vehicle.IVehicle.DEPTH_UPDATE, 
                                 self._update)
        self._connections.append(conn)
        
    def _update(self,event):
        self._depthbar.setVal(event.depth)

    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parentFunc):
        vehicle = ext.core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Depth")
            paneInfo = paneInfo.Caption("Depth").Left()
        
            return [(paneInfo, DepthPanel(parentFunc(), vehicle), [vehicle])]
        
        return []

class RotationPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, vehicle, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
      
        # Create Rotational Controls
        values = [('Roll', RotationCtrl.ROLL), 
                  ('Pitch', RotationCtrl.PITCH), 
                  ('Yaw', RotationCtrl.YAW)]
      
        layout = wx.GridBagSizer(0,0)

        pos = 0
        for name, style in values:
            # Create controls
            label = wx.StaticText(self, wx.ID_ANY, name)
            control = RotationCtrl(self, name, style = style)
            
            # Set variables
            setattr(self, '_' + name[0].lower() + name[1:] + 'Control', control)
            
            # Add to sizer
            layout.Add(label, (0,pos), flag = wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(control, (1,pos), flag = wx.EXPAND)
            layout.AddGrowableCol(pos)
            pos += 1

        layout.AddGrowableRow(1) 
        self.SetSizerAndFit(layout)
        
        # Subscribe to events
        self.Bind(wx.EVT_CLOSE,self._onClose)
        conn = vehicle.subscribe(ext.vehicle.IVehicle.ORIENTATION_UPDATE, 
                                 self._onOrientationUpdate)
        self._connections.append(conn)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
        closeEvent.Skip()
    
    def _onOrientationUpdate(self, event):
        quat = event.orientation
        self._rollControl.setOrientation(quat)
        self._pitchControl.setOrientation(quat)
        self._yawControl.setOrientation(quat)
  
    @staticmethod
    def getPanels(subsystems, parentFunc):
        vehicle = ext.core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Orientation")
            paneInfo = paneInfo.Caption("Orientation").Left()
        
            return [(paneInfo, RotationPanel(parentFunc(), vehicle), [vehicle])]
        
        return []
    
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
    def getPanels(subsystems, parentFunc):
        """
        Creates 
        """
        power = ext.core.Subsystem.getSubsystemOfType(subsystemMod.DemoPower,
                                                      subsystems)  
        
        if power is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Demo Power")
            paneInfo = paneInfo.Caption("Demo Power").Left()
        
            return [(paneInfo, DemoPowerPanel(parentFunc(), power), [power])]
    
        return []
    

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
            
        closeEvent.Skip()
            
    def _update(self, event):
        """
        Called when the power subsystem updates the power cosumption
        """
        self._xText.SetValue(str(event.x))
        self._yText.SetValue(str(event.y))

    @staticmethod
    def getPanels(subsystems, parentFunc):
        sonar = ext.core.Subsystem.getSubsystemOfType(subsystemMod.DemoSonar,
                                                      subsystems)
        
        if sonar is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Demo Sonar")
            paneInfo = paneInfo.Caption("Demo Sonar").Left()
        
            return [(paneInfo, DemoSonarPanel(parentFunc(), sonar), [sonar])]
        
        return []
    
