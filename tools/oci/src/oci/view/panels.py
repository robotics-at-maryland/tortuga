# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>


# STD Imports
import math

# Library Imports
import wx

# Project Imports
from core import Component, implements

from gui.view import IPanelProvider
from oci.view.controls import DepthBar, ThrusterBar, RotationCtrl
import oci.model.subsystem as subsystemMod

import ext.math
import ext.vehicle
import ext.vehicle.device
import ext.control
import ext.core as core

class ThrusterPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, thrusters, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._thrusterList = []
        self._thrusterList = thrusters
        
        layout =  wx.GridBagSizer(10, 10)
        
        pos = 0
  
        for item in self._thrusterList:
            # Create Control
            label = wx.StaticText(self, wx.ID_ANY, 
                                  item.getName().replace('Thruster',''))
            bar = ThrusterBar(self)
            bar.minValue = item.getMinForce()
            bar.maxValue = item.getMaxForce()
                       
            # Add it to our layout
            layout.Add(label, (0,pos),flag = wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(bar, (1,pos), flag = wx.EXPAND)
            layout.AddGrowableCol(pos)
            pos += 1
            
            # Connect to its events
            conn = eventHub.subscribe(ext.vehicle.device.IThruster.FORCE_UPDATE,
                                      item, self._update(bar))
            self._connections.append(conn)
            
        
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
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                     subsystems)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            # Get All the current Thrusters the vehicle has
            thrusters = []
            for name in vehicle.getDeviceNames():
                device = vehicle.getDevice(name)
                if isinstance(device, ext.vehicle.device.IThruster):
                    thrusters.append(device)
    
            # Only create the panel if there are thrusters on the vehicle        
            if len(thrusters):
                paneInfo = wx.aui.AuiPaneInfo().Name("Thrusters")
                paneInfo = paneInfo.Caption("Thrusters").Left()
        
                panel = ThrusterPanel(parentFunc(), eventHub, thrusters)
                return [(paneInfo, panel, [vehicle])]
            
        return []

class DepthPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, controller, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []
        
        layout =  wx.GridBagSizer(10, 10)
              
        label = wx.StaticText(self, label = "Depth")
        layout.Add(label, (0,0), span = wx.GBSpan(1, 2),
                   flag = wx.ALIGN_CENTER_HORIZONTAL)
        
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+00.0')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        # Create Desired controls
        desiredLabel = wx.StaticText(self, label = 'Des:')
        layout.Add(desiredLabel, (1, 0), flag = wx.ALIGN_CENTER)
        self._desiredDepth = wx.TextCtrl(self, size = textSize,
                                         style = textStyle)
        layout.Add(self._desiredDepth, (1, 1), flag = wx.ALIGN_CENTER)
        
        # Create Actual controls
        actualLabel = wx.StaticText(self, label = 'Act:')
        layout.Add(actualLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._actualDepth = wx.TextCtrl(self, size = textSize,
                                        style = textStyle)
        layout.Add(self._actualDepth, (2, 1), flag = wx.ALIGN_CENTER)
        
        # Create graphical controls
        self._depthbar = DepthBar(self)
        self._depthbar.minValue = 20
        layout.Add(self._depthbar, (3,0), span = wx.GBSpan(1,2), 
                   flag = wx.EXPAND)
        
        layout.AddGrowableCol(1)
        layout.AddGrowableRow(3)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        if vehicle is not None:
            conn = eventHub.subscribe(ext.vehicle.IVehicle.DEPTH_UPDATE, 
                                     vehicle, self._depthUpdate)
            self._connections.append(conn)
        
        if controller is not None:
            conn = eventHub.subscribe(ext.control.IController.DESIRED_DEPTH_UPDATE,
                                      controller, self._desiredDepthUpdate)
        
            self._connections.append(conn)    
        
    def _depthUpdate(self,event):
        self._actualDepth.Value = '% 4.1f' % event.number
        self._depthbar.setVal(event.number)
        
    def _desiredDepthUpdate(self, event):
        self._desiredDepth.Value = '% 4.1f' % event.number
        self._depthbar.desiredValue = event.number

    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parentFunc):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Depth")
            paneInfo = paneInfo.Caption("Depth").Left()
        
        
            panel = DepthPanel(parentFunc(), eventHub, vehicle, controller)
            return [(paneInfo, panel, [vehicle])]
        
        return []

class RotationPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, controller, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._actualOrientation = ext.math.Quaternion.IDENTITY
        self._desiredOrientation = ext.math.Quaternion.IDENTITY
      
        # Create Rotational Controls
        values = [('Roll', RotationCtrl.ROLL, 0, 1), 
                  ('Pitch', RotationCtrl.PITCH, math.pi/2, 1), 
                  ('Yaw', RotationCtrl.YAW, 0, -1)]
      
        layout = wx.GridBagSizer(0,0)

        pos = 0
        for name, style, offset, direction in values:
            # Create controls
            label = wx.StaticText(self, wx.ID_ANY, name)
            control = RotationCtrl(self, name, style = style, offset = offset,
                                   direction = direction)
            
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
        conn = eventHub.subscribe(ext.vehicle.IVehicle.ORIENTATION_UPDATE, 
                                  vehicle, self._onOrientationUpdate)
        
        conn = eventHub.subscribe(ext.control.IController.DESIRED_ORIENTATION_UPDATE, 
                                  controller, self._onDesiredOrientationUpdate)
        self._connections.append(conn)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
        closeEvent.Skip()
    
    def _onDesiredOrientationUpdate(self, event):
        quat = event.orientation
        self._desiredOrientation = quat
        self._rollControl.setOrientation(self._actualOrientation, quat)
        self._pitchControl.setOrientation(self._actualOrientation, quat)
        self._yawControl.setOrientation(self._actualOrientation, quat)       
    
    def _onOrientationUpdate(self, event):
        quat = event.orientation
        self._actualOrientation = quat
        self._rollControl.setOrientation(quat, self._desiredOrientation)
        self._pitchControl.setOrientation(quat, self._desiredOrientation)
        self._yawControl.setOrientation(quat, self._desiredOrientation)
  
    @staticmethod
    def getPanels(subsystems, parentFunc):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Orientation")
            paneInfo = paneInfo.Caption("Orientation").Left()
        
            panel = RotationPanel(parentFunc(), eventHub, vehicle, controller)
            return [(paneInfo, panel, [vehicle])]
        
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
        power = core.Subsystem.getSubsystemOfType(subsystemMod.DemoPower,
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
        sonar = core.Subsystem.getSubsystemOfType(subsystemMod.DemoSonar,
                                                      subsystems)
        
        if sonar is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Demo Sonar")
            paneInfo = paneInfo.Caption("Demo Sonar").Left()
        
            return [(paneInfo, DemoSonarPanel(parentFunc(), sonar), [sonar])]
        
        return []
    
