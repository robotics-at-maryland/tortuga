# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>


# STD Imports
import math
from datetime import datetime

# Library Imports
import wx
import wx.grid

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
import ram.filter as filter
import ram.timer as timer

class BasePanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._generatedControls = []
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
    def _createControls(self, name, startEnable = False):
        # Creat box around controls
        box = wx.StaticBox(parent = self, label = name)
        topSizer = wx.StaticBoxSizer(box)
        
        self.sizer = wx.FlexGridSizer(0, 2, 10, 10)
        topSizer.Add(self.sizer, 1, wx.EXPAND)
        
        # Create controls
        self._createDataControls()

        # Start off greyed out if desired
        for control in self._generatedControls:
            control.Enable(startEnable)
        
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
    fmt = "MM:SS.mm"
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
        lastLabel = wx.StaticText(self, label = 'Previous States')
        layout.Add(lastLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._stateList = wx.ListBox(self, wx.ID_ANY, name = 'State List',
                                     style = wx.TE_RIGHT | wx.LB_SINGLE)
        layout.Add(self._stateList, (3, 0), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        layout.AddGrowableCol(0)
        layout.AddGrowableRow(3)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_ENTERED,
                                        self._onEntered)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_EXITED,
                                        self._onExited)
        self._connections.append(conn)
        
        self._startTime = timer.time()
        
    def _onEntered(self,event):
        self._currentState.Value = '%s' % event.string
        
    def _onExited(self,event):
        # Get the time stamp and its difference from the beginning
        timeStamp = datetime.fromtimestamp(event.timeStamp - self._startTime)
        
        # Format the string MM:SS.mm EventName
        string = timeStamp.strftime("%M:%S.") + \
            "%.0f" % (timeStamp.microsecond / 10000.0)
        string = string + (' %s' % event.string)
        self._stateList.InsertItems([string], pos = 0)
       
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

class EventRatePanel(wx.grid.Grid):
    implements(IPanelProvider)
    
    class EventData(object):
        def __init__(self, name, location, timeStamp,
                     filter_ = None, active = True):
            self.name = name
            self.location = location
            if filter_ is None:
                self.filter = filter.MovingAverageFilter(10)
            else:
                self.filter = filter_
            self.timeStamp = timeStamp
            self.active = active
            self.timesInActive = 0
    
    def __init__(self, parent, eventHub, *args, **kwargs):
        """Create the Control Panel"""
        wx.grid.Grid.__init__(self, parent, *args, **kwargs)
        self.CreateGrid(0, 2)
        self.SetColLabelSize(0)
        self.SetRowLabelSize(0)
        
        self._timer = wx.Timer()
        self._timer.Bind(wx.EVT_TIMER, self._timerHandler)
        
        self._connections = []
        
        # Event Rate Table points data in EventData
        self._eventRateTable = dict()
        
        conn = eventHub.subscribeToAll(self._handler)
        self._connections.append(conn)
        
        self._timer.Start(250)
        
    def _handler(self, event):
        if self._eventRateTable.has_key(event.type):
            data = self._eventRateTable.get(event.type)
            if data.active is True:
                timeDifference = event.timeStamp - data.timeStamp
                data.filter.append(timeDifference)
            data.timeStamp = event.timeStamp
            data.active = True
            data.timesInActive = 0
        else:
            name = str(event.type)
            name = name[name.find(' ')+1:]
            
            list = [b for b in self._eventRateTable.itervalues()]
            list.sort(cmp = self._compareLocations)
            
            # If the event hasn't been seen before, create it in the table
            location = self._findLocation(list, name)
            
            # Insert a row at the location
            self.InsertRows(pos = location)
            
            # Increment the location for all data below the new one
            for oldValue in list:
                if oldValue.location >= location:
                    oldValue.location += 1
            
            self._eventRateTable[event.type] = \
                EventRatePanel.EventData(name, location, event.timeStamp)
            
            self.SetCellValue(location, 0, name)
            self.SetReadOnly(location, 0, isReadOnly = True)
            self.SetReadOnly(location, 1, isReadOnly = True)
            self.AutoSizeColumn(0)
            self.AutoSizeColumn(1)
            
    def _timerHandler(self, event):
        deleteQueue = []
        for key in self._eventRateTable.iterkeys():
            data = self._eventRateTable[key]
            rate = 0
            average = data.filter.getAverage() 
            if average != 0:
                rate = 1.0 / average
                
            currentTime = timer.time()
            if data.active:
                if (currentTime - data.timeStamp) > (10*average):
                    data.filter = filter.MovingAverageFilter(10)
                    rate = 0
                    data.active = False
                    data.timesInActive = 1
            else:
                # If the event has been inactive for 5 seconds, then queue
                # it for deletion
                # This runs 4 times a second, so the number is 20
                if data.timesInActive >= 20:
                    self.DeleteRows(pos = data.location)
                    list = [b for b in self._eventRateTable.itervalues()]
                    for b in list:
                        if b.location > data.location:
                            b.location -= 1
                    # Add the key for deletion upon exit
                    deleteQueue.append(key)
                # Otherwise iterate the count
                data.timesInActive += 1
                
            self.SetCellValue(data.location, 1, "%4.1f" % rate)
            
        # Delete any items in the deleteQueue
        for toDelete in deleteQueue:
            del self._eventRateTable[toDelete]
            
        # Autosize
        self.AutoSizeColumn(0)
        self.AutoSizeColumn(1)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    def _findLocation(self, list, name, iter = 0):
        size = len(list)
        while (iter < size) and (list[iter].name < name):
            iter += 1
        return iter
    
    def _compareLocations(self, dataA, dataB):
        return dataA.location - dataB.location
    
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        paneInfo = wx.aui.AuiPaneInfo().Name("Event Rate")
        paneInfo = paneInfo.Caption("Event Rate").Right()

        panel = EventRatePanel(parent, eventHub)
        return [(paneInfo, panel, [])]
        
        return []

class PayloadPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._markersLeds = []
        self._torpLeds = []
        self._markersDropped = 0
        self._torpedosLaunched = 0
        
        # Subscribe to events
        markerDropper = vehicle.getDevice('MarkerDropper')
        conn = eventHub.subscribe(
            ext.vehicle.device.IPayloadSet.OBJECT_RELEASED, markerDropper,
            self._markerDropped)
        self._connections.append(conn)
            
        torpedoLauncher = vehicle.getDevice('TorpedoLauncher')
        conn = eventHub.subscribe(
            ext.vehicle.device.IPayloadSet.OBJECT_RELEASED, torpedoLauncher,
            self._torpedoLaunched)
        self._connections.append(conn)
        
        
        
        # Determine the size of the sizer
        markerCount = markerDropper.initialObjectCount()
        torpedoCount = torpedoLauncher.initialObjectCount()
        if markerCount > torpedoCount:
            raise "ERROR marker count bigger then torpedo count, FIX ME!!"
        
        sizerWidth = markerCount + 1
        overWidth = 0
        if sizerWidth < (torpedoCount + 1):
            sizerWidth = (torpedoCount + 1)
            overWidth = torpedoCount - markerCount
        
        # Flexible grid around controls
        self.sizer = wx.FlexGridSizer(0, sizerWidth, 10, 10)
        
        # Create controls
        self._markersLeds = self._createPayloadRow("Markers", sizerWidth - 1)
        self._torpLeds = self._createPayloadRow("Trops", sizerWidth - 1)
        
        self.SetSizerAndFit(self.sizer)
        
        # Hide extra marker controls if needed
        if overWidth > 0:
            idxStart = markerCount + 1;
            for idx in xrange(idxStart, idxStart + overWidth):
                self.sizer.Hide(idx)
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
    def _createPayloadRow(self, label, count):
        # Create Label
        desiredLabel = wx.StaticText(self, label = label)
        self.sizer.Add(desiredLabel, 1, flag = wx.ALIGN_RIGHT)
        
        # Create Row of LEDs
        leds = []
        for i in xrange(0, count):
            led = ram.gui.led.LED(self, state = 2)
            led.MinSize = led.GetSize()
            self.sizer.Add(led, 1, flag = wx.ALIGN_CENTER)
            leds.append(led)
        return leds

    def _markerDropped(self, event):
        self._markersLeds[self._markersDropped].SetState(3)
        self._markersDropped += 1
    
    def _torpedoLaunched(self, event):
        self._torpLeds[self._torpedosLaunched].SetState(3)
        self._torpedosLaunched += 1
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(ext.vehicle.IVehicle,
                                                        subsystems)

        if (vehicle is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Payload")
            paneInfo = paneInfo.Caption("Payload").Right()
        
            panel = PayloadPanel(parent, eventHub, vehicle)
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

class SonarPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []
        self._vehicleOrientation = ext.math.Quaternion.IDENTITY;
        self._pingerOrientations = [ext.math.Quaternion.IDENTITY];
        self._lastPingCount = 0
        self._currentPingerID = 0
        self._pingerEvents = {}
        
        layout =  wx.GridBagSizer(10, 10)
              
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+00.0')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY
        
        # Create Pinger selection controls
        label = wx.StaticText(self, label = 'Pinger:')
        layout.Add(label, (0, 0), flag = wx.ALIGN_CENTER)

        size = wx.Size(10, wx.DefaultSize.height)
        self._pingerChoice = wx.Choice(self, id = wx.ID_ANY, size = size)
        self._pingerChoice.Append('0')
        self._pingerChoice.SetSelection(0)
        self._pingerChoice.Bind(wx.EVT_CHOICE, self._onPingerChoice)
        layout.Add(self._pingerChoice, (0, 1), 
                   flag = wx.ALIGN_CENTER | wx.EXPAND)

        # Create X controls
        desiredLabel = wx.StaticText(self, label = 'X:')
        layout.Add(desiredLabel, (1, 0), flag = wx.ALIGN_CENTER)
        self._x = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._x, (1, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Y controls
        actualLabel = wx.StaticText(self, label = 'Y:')
        layout.Add(actualLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._y = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._y, (2, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Z controls
        actualLabel = wx.StaticText(self, label = 'Z:')
        layout.Add(actualLabel, (3, 0), flag = wx.ALIGN_CENTER)
        self._z = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._z, (3, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Time Controls
        actualLabel = wx.StaticText(self, label = 'Sp#:')
        layout.Add(actualLabel, (4, 0), flag = wx.ALIGN_CENTER)
        self._pingCount = wx.TextCtrl(self, size = textSize,
                              style = textStyle)
        layout.Add(self._pingCount, (4, 1), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Bearing Control
        self._bearing = RotationCtrl(self, 'Bearing', style = RotationCtrl.YAW, 
                                     offset = 0, direction = -1)
        layout.Add(self._bearing, (5, 0), flag = wx.EXPAND,
                   span = wx.GBSpan(1,2))
        
        # Create graphical controls
        #self._depthbar = DepthBar(self)
        #self._depthbar.minValue = 20
        #layout.Add(self._depthbar, (3,0), 
        #           flag = wx.EXPAND)
        
        layout.AddGrowableCol(1)
        layout.AddGrowableRow(5)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        conn = eventHub.subscribeToType(ext.vehicle.device.ISonar.UPDATE, 
                                        self._update)
        self._connections.append(conn)
        
        conn = eventHub.subscribe(ext.vehicle.IVehicle.ORIENTATION_UPDATE, 
                                  vehicle, self._onOrientationUpdate)
        self._connections.append(conn)
        
    def _update(self,event):
        # Store the event
        self._pingerEvents[event.pingerID] = event
            
        # Update the numeric displays
        self._updateNumericDisplay(event)
        
        # Expand pinger orientations and pinger ID list
        while (event.pingerID + 1) > len(self._pingerOrientations):
            self._pingerChoice.Append(str(len(self._pingerOrientations)))
            self._pingerOrientations.append(ext.math.Quaternion.IDENTITY)

        # Only do an update if we got a new ping
        if self._lastPingCount != event.pingCount:
            self._lastPingCount = event.pingCount
            # Correct the pinger orientation from relative to absolute
            relPingerOrientation = \
                ext.math.Vector3.UNIT_X.getRotationTo(event.direction)
            absPingerOrientation = \
                self._vehicleOrientation * relPingerOrientation 
            self._pingerOrientations[event.pingerID] = absPingerOrientation
       
        # Build up and send the set of orientations to display on the gauge
        orientations = [self._vehicleOrientation]
        orientations.extend(self._pingerOrientations)
        self._bearing.setMultipleOrientations(orientations)

    def _updateNumericDisplay(self, event):
        direction = event.direction
        if self._currentPingerID == event.pingerID:
            self._x.Value = '% 6.4f' % direction.x
            self._y.Value = '% 6.4f' % direction.y
            self._z.Value = '% 6.4f' % direction.z
        self._pingCount.Value = '% 8.1f' % event.pingCount
        
    def _onPingerChoice(self, event):
        self._currentPingerID = event.GetSelection()
        self._updateNumericDisplay(self._pingerEvents[self._currentPingerID])

    def _onOrientationUpdate(self, event):
        self._vehicleOrientation = event.orientation
        orientations = [self._vehicleOrientation]
        orientations.extend(self._pingerOrientations)
        self._bearing.setMultipleOrientations(orientations)
        
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


class VelocityPosition(BasePanel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub):
        BasePanel.__init__(self, parent)#, *args, **kwargs)
    
        # Controls
        self._createControls("Pos & Vel", startEnable = True)
        
        # Events
        conn = eventHub.subscribeToType(ext.vehicle.IVehicle.VELOCITY_UPDATE,
                                        self._onVelocityUpdate)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ext.vehicle.IVehicle.POSITION_UPDATE,
                                        self._onPositionUpdate)
        self._connections.append(conn)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
    
    def _createDataControls(self):
        self._createDataControl(controlName = '_xPos', label = 'X Pos: ')
        self._createDataControl(controlName = '_yPos', label = 'Y Pos: ')
        self._createDataControl(controlName = '_xVel', label = 'X Vel: ')
        self._createDataControl(controlName = '_yVel', label = 'Y Vel: ')
    
    def _onVelocityUpdate(self, event):
        vel = event.vector2
        self._xVel.Value = "% 4.2f" % vel.x
        self._yVel.Value = "% 4.2f" % vel.y

    def _onPositionUpdate(self, event):
        pos = event.vector2
        self._xPos.Value = "% 4.2f" % pos.x
        self._yPos.Value = "% 4.2f" % pos.y

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vehicle = core.Subsystem.getSubsystemOfType(
            ext.vehicle.IVehicle, subsystems)
        
        if vehicle is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Pos & Vel")
            paneInfo = paneInfo.Caption("Pos & Vel").Right()
        
            panel = VelocityPosition(parent, eventHub)
            return [(paneInfo, panel, [vehicle])]
        
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
    
