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
from ram.core import Component, implements

from ram.gui.view import IPanelProvider
from oci.view.controls import DepthBar, MultiBar, RotationCtrl
from oci.view.controls import PowerSourceDisplay, TempSensorDisplay
from oci.view.controls import ThrusterCurrentDisplay
import oci.model.subsystem as subsystemMod

import ext.math
import ext.vehicle
import ext.vehicle.device
import ext.control
import ext.core as core

import ram.ai.state 

class BasePanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._generatedControls = []
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
    def _createControls(self, name):
        # Creat box around controls
        box = wx.StaticBox(parent = self, label = name)
        topSizer = wx.StaticBoxSizer(box)
        
        self.sizer = wx.FlexGridSizer(0, 2, 10, 10)
        topSizer.Add(self.sizer, 1, wx.EXPAND)
        
        # Create controls
        self._createDataControls()

        # Start off greyed out
        for control in self._generatedControls:
            control.Enable(False)
        
        self.SetSizerAndFit(topSizer)

    def _createDataControls(self):
        pass

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
                                  self._fixName(item.getName()))

            bar = MultiBar(self)
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
        
    def _fixName(self, name):
        name = name.replace('Thruster','')
        name = name.replace('Starboard','Star')
        name = name.replace('Bottom', 'Bot ')
        name = name.replace('Aft', 'Aft ')
        return name

    def _update(self, bar):
        def handler(event):
            bar.setVal(event.number)
        return handler
    
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
        closeEvent.Skip()
    
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                     subsystems)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            # Get All the current Thrusters the vehicle has
            thrusters = []
            names = vehicle.getDeviceNames()
            
            for i in range(0,len(names)):
                device = vehicle.getDevice(names[i])
                if isinstance(device, ext.vehicle.device.IThruster):
                    thrusters.append(device)
    
            # Only create the panel if there are thrusters on the vehicle        
            if len(thrusters):
                paneInfo = wx.aui.AuiPaneInfo().Name("Thrusters")
                paneInfo = paneInfo.Caption("Thrusters").Bottom()
        
                paneInfoC = wx.aui.AuiPaneInfo().Name("TCurrents")   
                paneInfoC = paneInfoC.Caption("Thrusters Currents").Bottom()
        
                panel = ThrusterPanel(parent, eventHub, thrusters)
                panelC = ThrusterCurrentPanel(parent, eventHub, thrusters)
                return [(paneInfo, panel, [vehicle]),
                        (paneInfoC, panelC, [vehicle])]
            
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
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Depth")
            paneInfo = paneInfo.Caption("Depth").Right()
        
        
            panel = DepthPanel(parent, eventHub, vehicle, controller)
            return [(paneInfo, panel, [vehicle])]
        
        return []

class AIPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, stateMachine, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []

        layout =  wx.GridBagSizer(10, 10)
              
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+00.0')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        # Create Current controls
        currentLabel = wx.StaticText(self, label = 'Current State')
        layout.Add(currentLabel, (0, 0), flag = wx.ALIGN_CENTER)
        self._currentState = wx.TextCtrl(self, size = textSize,
                                         style = textStyle)
        layout.Add(self._currentState, (1, 0), 
                   flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Last controls
        lastLabel = wx.StaticText(self, label = 'Last State')
        layout.Add(lastLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._lastState = wx.TextCtrl(self, size = textSize,
                                      style = textStyle)
        layout.Add(self._lastState, (3, 0), 
                   flag = wx.ALIGN_CENTER | wx.EXPAND)
        

        layout.AddGrowableCol(0)
        #layout.AddGrowableRow(1)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_ENTERED,
                                        self._onEntered)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_EXITED,
                                        self._onExited)
        self._connections.append(conn)
        
    def _onEntered(self,event):
        self._currentState.Value = '%s' % event.state.__class__.__name__
        
    def _onExited(self,event):
        self._lastState.Value = '%s' % event.state.__class__.__name__
       
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        machine = core.Subsystem.getSubsystemOfType(ram.ai.state.Machine,
                                                    subsystems)

        if machine is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("AI")
            paneInfo = paneInfo.Caption("AI").Right()
        
            panel = AIPanel(parent, eventHub, machine)
            return [(paneInfo, panel, [machine])]
        
        return []

class SonarPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []
        self._vehicleOrientation = ext.math.Quaternion.IDENTITY;
        self._pingerOrientation = ext.math.Quaternion.IDENTITY;
        self._lastTime = 0

        layout =  wx.GridBagSizer(10, 10)
              
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+00.0')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        # Create X controls
        desiredLabel = wx.StaticText(self, label = 'X:')
        layout.Add(desiredLabel, (0, 0), flag = wx.ALIGN_CENTER)
        self._x = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._x, (0, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Y controls
        actualLabel = wx.StaticText(self, label = 'Y:')
        layout.Add(actualLabel, (1, 0), flag = wx.ALIGN_CENTER)
        self._y = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._y, (1, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Z controls
        actualLabel = wx.StaticText(self, label = 'Z:')
        layout.Add(actualLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._z = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._z, (2, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Time Controls
        actualLabel = wx.StaticText(self, label = 'Sp#:')
        layout.Add(actualLabel, (3, 0), flag = wx.ALIGN_CENTER)
        self._time = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._time, (3, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Bearing Control
        self._bearing = RotationCtrl(self, 'Bearing', style = RotationCtrl.YAW, 
                                     offset = 0, direction = -1)
        layout.Add(self._bearing, (4, 0), flag = wx.EXPAND,
                   span = wx.GBSpan(1,2))
        
        # Create graphical controls
        #self._depthbar = DepthBar(self)
        #self._depthbar.minValue = 20
        #layout.Add(self._depthbar, (3,0), 
        #           flag = wx.EXPAND)
        
        layout.AddGrowableCol(1)
        layout.AddGrowableRow(4)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        conn = eventHub.subscribeToType(ext.vehicle.device.ISonar.UPDATE, 
                                        self._update)
        self._connections.append(conn)
        
        conn = eventHub.subscribe(ext.vehicle.IVehicle.ORIENTATION_UPDATE, 
                                  vehicle, self._onOrientationUpdate)
        self._connections.append(conn)
        
    def _update(self,event):
        direction = event.direction
        self._x.Value = '% 6.4f' % direction.x
        self._y.Value = '% 6.4f' % direction.y
        self._z.Value = '% 6.4f' % direction.z
        self._time.Value = '% 8.1f' % event.pingTimeUSec
        
        if self._lastTime != event.pingTimeUSec:
	    self._lastTime = event.pingTimeUSec
            self._pingerOrientation = ext.math.Vector3.UNIT_X.getRotationTo(
                event.direction)
            self._pingerOrientation = self._vehicleOrientation * self._pingerOrientation 
        
        self._bearing.setOrientation(self._vehicleOrientation, 
                                     self._pingerOrientation)
       
    def _onOrientationUpdate(self, event):
        self._vehicleOrientation = event.orientation
        self._bearing.setOrientation(self._vehicleOrientation, 
                                     self._pingerOrientation)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)

        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Sonar")
            paneInfo = paneInfo.Caption("Sonar").Right()
        
            panel = SonarPanel(parent, eventHub, vehicle)
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
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Orientation")
            paneInfo = paneInfo.Caption("Orientation").Bottom()
        
            panel = RotationPanel(parent, eventHub, vehicle, controller)
            return [(paneInfo, panel, [vehicle])]
        
        return []
    
class BarDisplayPanel(wx.Panel):
    """
    Base panel for bar displays.  All the user has to do is subclass and 
    create the '_createBarDisplay' method which returns a new display
    """
    def __init__(self, parent, eventHub, sensors, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._sensors = sensors
        self._displays = []
         
        sizer =  wx.GridBagSizer(10, 10)
        lineNum = 0
  
        for sensor in self._sensors:
            # Create Control
            display = self._createBarDisplay(parent, eventHub, sensor,
                                             sizer, lineNum)
            self._displays.append(display)
            lineNum += 1
            
        # Only gauge column is growable
        sizer.AddGrowableCol(2)
        
        self.SetSizerAndFit(sizer)
        
        # Connect to events
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
    def _createBarDisplay(self, parent, eventHub, sensor, sizer, lineNum):
        """
        Create and return bar display here
        """
        return None
    
    def _onClose(self, closeEvent):
        """
        Disconnects from C++ events
        
        Ensure that functions of this object won't get called after its already
        disctructed.
        """
        for display in self._displays:
            display.disconnect()
            
        closeEvent.Skip()
    
class TempSensorPanel(BarDisplayPanel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, tempSensors, *args, **kwargs):
        BarDisplayPanel.__init__(self, parent, eventHub, tempSensors, *args, 
                                 **kwargs)
    
    def _createBarDisplay(self, parent, eventHub, sensor, sizer, lineNum):
        return TempSensorDisplay(parent = self, eventHub = eventHub,
                                 tempSensor = sensor, sizer = sizer, 
                                 lineNum = lineNum)
    
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                     subsystems)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            # Get All the current PowerSources the vehicle has
            tempSensors = []
            names = vehicle.getDeviceNames()
            
            for i in range(0,len(names)):
                dev = vehicle.getDevice(names[i])
                if isinstance(dev, ext.vehicle.device.ITempSensor):
                    tempSensors.append(dev)
    
            # Only create the panel if there are tempSensorss on the vehicle        
            if len(tempSensors):
                paneInfoV = wx.aui.AuiPaneInfo().Name("Temperatures")
                paneInfoV = paneInfoV.Caption("Temperatures").Bottom()
                panelV = TempSensorPanel(parent, eventHub, tempSensors)                
                return [(paneInfoV, panelV, [vehicle])]
            
        return []
    
class ThrusterCurrentPanel(BarDisplayPanel):
    def __init__(self, parent, eventHub, thrusters, *args, **kwargs):
        BarDisplayPanel.__init__(self, parent, eventHub, thrusters, *args, 
                                 **kwargs)
    
    def _createBarDisplay(self, parent, eventHub, sensor, sizer, lineNum):
        return ThrusterCurrentDisplay(parent = self, eventHub = eventHub,
                                      thruster = sensor, sizer = sizer, 
                                      lineNum = lineNum)

class PowerSourcePanel(BarDisplayPanel):
    implements(IPanelProvider)
    
    VOLTAGE = 1
    CURRENT = 2
        
    def __init__(self, parent, eventHub, powerSources, mode, *args, **kwargs):
        self._mode = mode
        BarDisplayPanel.__init__(self, parent, eventHub, powerSources, *args, 
                                 **kwargs)
    
    def _createBarDisplay(self, parent, eventHub, sensor, sizer, lineNum):
        return PowerSourceDisplay(parent = self, eventHub = eventHub,
                                  powerSource = sensor, sizer = sizer, 
                                  lineNum = lineNum, mode = self._mode)
                    
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub, 
                                                     subsystems)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)
        
        if vehicle is not None:
            # Get All the current PowerSources the vehicle has
            powerSources = []
            names = vehicle.getDeviceNames()
            
            for i in range(0,len(names)):
                dev = vehicle.getDevice(names[i])
                if isinstance(dev, ext.vehicle.device.IPowerSource):
                    powerSources.append(dev)
    
            # Only create the panel if there are powerSourcess on the vehicle        
            if len(powerSources):
                paneInfoV = wx.aui.AuiPaneInfo().Name("PowerSourceVoltages")
                paneInfoV = paneInfoV.Caption("Power Source Voltages").Bottom()
                panelV = PowerSourcePanel(parent, eventHub, powerSources,
                                         PowerSourcePanel.VOLTAGE)
                
                paneInfoC = wx.aui.AuiPaneInfo().Name("PowerSource Currents")
                paneInfoC = paneInfoC.Caption("Power Source Currents").Bottom()
                panelC = PowerSourcePanel(parent, eventHub, powerSources,
                                         PowerSourcePanel.CURRENT)
                
                return [(paneInfoV, panelV, [vehicle]), 
                        (paneInfoC, panelC, [vehicle])]
            
        return []

class ControlDebugPanel(BasePanel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub):
        BasePanel.__init__(self, parent)#, *args, **kwargs)
        self._configured = False
        self._count = 0
    
        # Controls
        self._createControls("Controller")
        
        # Events
        conn = eventHub.subscribeToType(ext.control.IController.PARAM_SETUP,
                                        self._onParamSetup)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ext.control.IController.PARAM_UPDATE,
                                        self._onParamUpdate)
        self._connections.append(conn)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
    
    def _onParamSetup(self, event):
        """
        Called when the controller configures its update
        """
        assert not self._configured
        self._count = len(event.labels)
        
        # Iterate over each label this is ugly because of a current bug in the
        # wrapping of C++ containers, typeof(event.labes) == vector<string>
        for i in xrange(0, len(event.labels)):
            labelName = event.labels[i]
            self._createDataControl(controlName = '_' + str(i),
                                    label = labelName + ': ')
        
        self._configured = True
        
        # Reset sizer
        #self.SetSizerAndFit(self.GetSizer())

    def _onParamUpdate(self, event):
        assert len(event.values)  == self._count
        
        # Go through value and match it up to its control
        for i in xrange(0, len(event.values)):
            labelControl = getattr(self, '_' + str(i))
            labelControl.Value = "% 4.2f" % event.values[i]

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if controller is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Controller")
            paneInfo = paneInfo.Caption("Controller").Right()
        
            panel = ControlDebugPanel(parent, eventHub)
            return [(paneInfo, panel, [controller])]
        
        return []

#class DemoSonarPanel(wx.Panel):
#    implements(IPanelProvider)
#    
#    def __init__(self, parent, sonar, *args, **kwargs):
#        wx.Panel.__init__(self, parent, *args, **kwargs)
#        self._connections = []
#        
#         Create Controls
#        text = wx.StaticText(self, label = 'Sonar Position', size = (100,20))
#        
#        xLabel = wx.StaticText(self, label = 'X: ')#, size = (40,20))
#        yLabel = wx.StaticText(self, label = 'Y: ')#, size = (40,20))
#        
#        self._xText = wx.TextCtrl(self, value = '', size = (80,20), 
#                                  style = wx.TE_READONLY)
#        self._yText = wx.TextCtrl(self, value = '', size = (80,20),
#                                  style = wx.TE_READONLY)
#        
#         Horizontal Postion sizer
#        horizontalSizer = wx.BoxSizer(wx.HORIZONTAL)
#        horizontalSizer.Add(xLabel)
#        horizontalSizer.Add(self._xText)
#        horizontalSizer.Add(yLabel)
#        horizontalSizer.Add(self._yText)
#        
#         Vertical sizer
#        sizer = wx.BoxSizer(wx.VERTICAL)
#        sizer.Add(text)
#        sizer.Add(horizontalSizer)
#        
#         Hook size up to the panel
#        self.SetSizer(sizer)
#        sizer.Fit(self)
#        self.SetMinSize(sizer.GetMinSize())
#        
#        
#         Connect to events
#        self.Bind(wx.EVT_CLOSE, self._onClose)
#        conn = sonar.subscribe(subsystemMod.DemoSonar.SONAR_UPDATE, 
#                               self._update)
#        self._connections.append(conn)
#
#    def _onClose(self, closeEvent):
#        """
#        Disconnects from C++ events
#        
#        Ensure that functions of this object won't get called after its already
#        disctructed.
#        """
#        for conn in self._connections:
#            conn.disconnect()
#            
#        closeEvent.Skip()
#            
#    def _update(self, event):
#        """
#        Called when the power subsystem updates the power cosumption
#        """
#        self._xText.SetValue(str(event.x))
#        self._yText.SetValue(str(event.y))
#
#    @staticmethod
#    def getPanels(subsystems, parent):
#        sonar = core.Subsystem.getSubsystemOfType(subsystemMod.DemoSonar,
#                                                      subsystems)
#        
#        if sonar is not None:
#            paneInfo = wx.aui.AuiPaneInfo().Name("Demo Sonar")
#            paneInfo = paneInfo.Caption("Demo Sonar").Left()
#        
#            return [(paneInfo, DemoSonarPanel(parent, sonar), [sonar])]
#        
#        return []
    
