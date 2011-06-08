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
import ext.estimation
import ext.vehicle
import ext.vehicle.device
import ext.control
import ext.vision
import ext.core as core
import ext.logging as logging

from ram.logloader import resolve
import ram.ai.state
import ram.ai.task as task
import ram.filter as filter
import ram.timer as timer
import ram.motion as motion
import ram.motion.common
import ram.motion.pipe
import ram.monitor as monitor

class BasePanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self._connections = []
        self._generatedControls = []
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
    def _createControls(self, name, startEnable = False):
        # Create box around controls
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
    
    def __init__(self, parent, eventHub, estimator, controller, *args, **kwargs):
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
        
        if estimator is not None:
            conn = eventHub.subscribeToType(
                ext.estimation.IStateEstimator.ESTIMATED_DEPTH_UPDATE,
                self._depthUpdate)
            self._connections.append(conn)
        
        if controller is not None:
            conn = eventHub.subscribeToType(ext.control.IController.DESIRED_DEPTH_UPDATE,
                                            self._desiredDepthUpdate)
        
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
        
        estimator = core.Subsystem.getSubsystemOfType(
            ext.estimation.IStateEstimator, subsystems)
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (estimator is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Depth")
            paneInfo = paneInfo.Caption("Depth").Right()
        
        
            panel = DepthPanel(parent, eventHub, estimator, controller)
            return [(paneInfo, panel, [estimator])]
        
        return []
    
class MovementPanel(wx.Panel):
    implements(IPanelProvider)
    def __init__(self, parent, stateMachine, controller, motionManager, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        layout =  wx.BoxSizer(wx.HORIZONTAL)

        self._stateMachine = stateMachine
        self._controller = controller
        self._motionManager = motionManager
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []

        stopButton = wx.Button(self, label = "Stop")
        layout.Add(stopButton, flag = wx.ALIGN_CENTER | wx.ALL)
        stopButton.Bind(wx.EVT_BUTTON, self._onStop)

        self.SetSizer(layout)
        
    def _stopMotion(self):
        self._stateMachine.stop()
        self._motionManager.stopCurrentMotion()
        self._controller.setSpeed(0)
        self._controller.setSidewaysSpeed(0)
        self._controller.holdCurrentDepth()
        self._controller.holdCurrentHeading()

    def _onStop(self, event):
        self._stopMotion()
    
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
        
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parent):
        machine = core.Subsystem.getSubsystemOfType(ram.ai.state.Machine,
                                                    subsystems)
        
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        motionManager = core.Subsystem.getSubsystemOfType(ram.motion.basic.MotionManager,
                                                    subsystems)

        if machine is not None and controller is not None and motionManager is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Movement")
            paneInfo = paneInfo.Caption("Movement").Right()
        
            panel = MovementPanel(parent, machine, controller, motionManager)
            return [(paneInfo, panel, [motionManager])]
        
        return []

class AIPanel(wx.Panel):
    implements(IPanelProvider)
    fmt = "MM:SS.mm"
    def __init__(self, parent, eventHub, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []

        layout = wx.GridBagSizer(10, 10)
              
        textWidth, textHeight = wx.ClientDC(self).GetTextExtent('+00.0')
        textSize = wx.Size(textWidth, wx.DefaultSize.height) 
        textStyle = wx.TE_RIGHT | wx.TE_READONLY

        # Create Current Task controls
        currentLabel = wx.StaticText(self, label = 'Current Task')
        layout.Add(currentLabel, (0, 0), flag = wx.ALIGN_CENTER)
        self._taskTimer = wx.StaticText(self, label = '00:00.00')
        layout.Add(self._taskTimer, (0, 1), flag = wx.ALIGN_CENTER)
        self._currentTask = wx.TextCtrl(self, size = textSize,
                                         style = textStyle)
        layout.Add(self._currentTask, (1, 0), span = (1, 2),
                   flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Current controls
        currentLabel = wx.StaticText(self, label = 'Current State')
        layout.Add(currentLabel, (2, 0), flag = wx.ALIGN_CENTER)
        self._stateTimer = wx.StaticText(self, label = '00:00.00')
        layout.Add(self._stateTimer, (2, 1), flag = wx.ALIGN_CENTER)
        self._currentState = wx.TextCtrl(self, size = textSize,
                                         style = textStyle)
        layout.Add(self._currentState, (3, 0), span = (1, 2),
                   flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        # Create Last controls
        lastLabel = wx.StaticText(self, label = 'Previous States')
        layout.Add(lastLabel, (4, 0), span = (1, 2), flag = wx.ALIGN_CENTER)
        self._stateList = wx.ListBox(self, name = 'State List',
                                     style = wx.TE_RIGHT | wx.LB_SINGLE)
        layout.Add(self._stateList, (5, 0), span = (1, 2), flag = wx.ALIGN_CENTER | wx.EXPAND)
        
        clearButton = wx.Button(self, label = 'Clear')
        layout.Add(clearButton, (6, 0), flag = wx.ALIGN_CENTER)
        clearButton.Bind(wx.EVT_BUTTON, self._onClear)
        
        self._freezeButton = wx.Button(self, label = 'Freeze')
        layout.Add(self._freezeButton, (6, 1), flag = wx.ALIGN_CENTER)
        self._freezeButton.Bind(wx.EVT_BUTTON, self._onFreeze)
        
        layout.AddGrowableCol(0)
        layout.AddGrowableCol(1)
        layout.AddGrowableRow(5)
        
        self.SetSizerAndFit(layout)
        #self.SetSizeHints(0,0,100,-1)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_ENTERED,
                                        self._onEntered)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_EXITED,
                                        self._onExited)
        self._connections.append(conn)
        
        # Set the frozen value to False
        self._frozen = False
        self._exitList = []
        
        # Set the time we start for the previous states log
        self._firstRun = True
        self._startTime = timer.time()
        
        # Set these for the timer to know whether to update the task and state
        # timers
        self._taskRunning = False
        self._stateRunning = False
        
        # Start the timer for the current task and state
        self._timer = wx.Timer()
        self._timer.Bind(wx.EVT_TIMER, self._onTimer)
        
    def _onEntered(self,event):
        if self._firstRun:
            self._startTime = event.timeStamp
            self._firstRun = False
        fullClassName = str(event.string)
        
        # Recreate the class
        stateClass = resolve(fullClassName)
        if issubclass(stateClass, task.Task):
            # It's a task, put it there
            self._taskRunning = True
            self._taskTimeStamp = timer.time()
            self._currentTask.Value = '%s' % fullClassName.split('.')[-1]
        else:
            self._stateRunning = True
            self._stateTimeStamp = timer.time()
            self._currentState.Value = '%s' % fullClassName.split('.')[-1]
            
        if not self._timer.IsRunning():
            self._timer.Start(milliseconds = 100)
        
    def _onExited(self,event):
        # Get the time stamp and its difference from the beginning
        fullClassName = str(event.string)
        eventName = '%s' % fullClassName.split('.')[-1]
        stateClass = resolve(fullClassName)
        
        # Check if the exited state is still shown as the current task/state
        if issubclass(stateClass, task.Task):
            # It's a task, remove the task
            string = self._timeStamp(self._startTime, event.timeStamp)
            if self._currentTask.Value == eventName:
                self._currentTask.Value = ""
        else:
            # It's a state, remove the state
            string = self._timeStamp(self._startTime, event.timeStamp)
            if self._currentState.Value == eventName:
                self._currentState.Value = ""

        # Format the string MM:SS.mm EventName
        string = string + " " + eventName
        if self._frozen:
            self._exitList.append(string)
        else:
            self._stateList.Insert(string, pos = 0)
            self._stateList.EnsureVisible(0)
        
        self._timer.Stop()
        
    def _onClear(self, event):
        self._firstRun = True
        self._taskTimer.SetLabel("00:00.00")
        self._stateTimer.SetLabel("00:00.00")
        self._stateList.Clear()
        
    def _onFreeze(self, event):
        if self._frozen:
            self._frozen = False
            for x in self._exitList:
                self._stateList.Insert(x, pos = 0)
            self._stateList.EnsureVisible(0)
            self._freezeButton.SetLabel('Freeze')
        else:
            self._frozen = True
            self._freezeButton.SetLabel('Continue')
            
    def _onTimer(self, event):
        currentTime = timer.time()
        if self._taskRunning:
            string = self._timeStamp(self._taskTimeStamp, currentTime)
            self._taskTimer.SetLabel(string)
        if self._stateRunning:
            string = self._timeStamp(self._stateTimeStamp, currentTime)
            self._stateTimer.SetLabel(string)
            
    def _timeStamp(self, startTime, endTime):
        timeStamp = datetime.fromtimestamp(endTime - startTime)
        string = timeStamp.strftime("%M:%S.") + \
            "%.0f" % (timeStamp.microsecond / 10000.0)
            
        return string
        
    def _onClose(self, closeEvent):
        for conn in self._connections:
            conn.disconnect()
       
        closeEvent.Skip()
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)

        paneInfo = wx.aui.AuiPaneInfo().Name("AI")
        paneInfo = paneInfo.Caption("AI").Right()
        
        panel = AIPanel(parent, eventHub)
        return [(paneInfo, panel, [])]

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
                if (currentTime - data.timeStamp) > 5:
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
                
            self.SetCellValue(data.location, 1, "%5.2f" % rate)
            
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

class ProfilePanel(wx.grid.Grid):
    implements(IPanelProvider)
        
    def __init__(self, parent, eventHub, *args, **kwargs):
        wx.grid.Grid.__init__(self, parent, *args, **kwargs)
        self.CreateGrid(0, 2)
        self.SetColLabelSize(0)
        self.SetRowLabelSize(0)

        self._size = 0
        self._profileTable = {}

        self.Bind(wx.EVT_CLOSE, self._onClose)
        self._timer = wx.Timer()
        self._timer.Bind(wx.EVT_TIMER, self._timerHandler)

        self._conn = eventHub.subscribeToType(core.IUpdatable.PROFILE, self._onProfile)
        
        self._timer.Start(1000)
    
    def _onProfile(self, event):
        if event.sender != "UNNAMED":
            sender = str(event.sender.getPublisherName())
    
            if not self._profileTable.has_key(sender):
                self._profileTable[sender] = str(event.data)
                location = self._findLocation(sender)
                
                self.InsertRows(pos = location )
                self._size += 1
                self.SetCellValue(location, 0, sender)
                self.SetCellValue(location, 1, self._profileTable[sender])
                self.SetReadOnly(location, 0, isReadOnly = True)
                self.SetReadOnly(location, 1, isReadOnly = True)
                self.AutoSizeColumn(0)
                self.AutoSizeColumn(1)
                self.AutoSizeRow(location)
            else:
                self._profileTable[sender] = str(event.data)

    def _timerHandler(self, event):
         for row in range(self._size):
             data = self._profileTable[self.GetCellValue(row, 0)]
             self.SetCellValue(row, 1, data)
           
    def _findLocation(self, name):
        position = 0        
        for row in range(self._size):
            curr = self.GetCellValue(row, 0)
            if name < curr:
                position += 1
       
        return position
        
    def _onClose(self, closeEvent):
        self._conn.disconnect()

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)

        paneInfo = wx.aui.AuiPaneInfo().Name("Profile")
        paneInfo = paneInfo.Caption("Profile").Right()

        panel = ProfilePanel(parent, eventHub)

        return [(paneInfo, panel, [])]


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

        if (vehicle is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Payload")
            paneInfo = paneInfo.Caption("Payload").Right()
        
            panel = PayloadPanel(parent, eventHub, vehicle)
            return [(paneInfo, panel, [vehicle])]
        
        return []

class RotationPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, estimator, controller, *args,**kwargs):
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
        conn = eventHub.subscribeToType(
            ext.estimation.IStateEstimator.ESTIMATED_ORIENTATION_UPDATE,
            self._onOrientationUpdate)
        
        conn = eventHub.subscribeToType(
            ext.control.IController.DESIRED_ORIENTATION_UPDATE, 
            self._onDesiredOrientationUpdate)
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
        
        estimator = core.Subsystem.getSubsystemOfType(
            ext.estimation.IStateEstimator, subsystems)
        
        controller = core.Subsystem.getSubsystemOfType(
            ext.control.IController, subsystems)
        
        if (estimator is not None) or (controller is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Orientation")
            paneInfo = paneInfo.Caption("Orientation").Bottom()
        
            panel = RotationPanel(parent, eventHub, estimator, controller)
            return [(paneInfo, panel, [estimator])]
        
        return []

class SonarPanel(wx.Panel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, vehicle, estimator, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._connections = []
        self._vehicleOrientation = ext.math.Quaternion.IDENTITY;
        self._pingerOrientations = [ext.math.Quaternion.IDENTITY];
        self._lastPingCount = -1
        self._currentPingerID = -1
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
        
        conn = eventHub.subscribeToType(
            ext.estimation.IStateEstimator.ESTIMATED_ORIENTATION_UPDATE, 
            self._onOrientationUpdate)
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
        #if self._currentPingerID == event.pingerID:
        self._x.Value = '% 6.4f' % direction.x
        self._y.Value = '% 6.4f' % direction.y
        self._z.Value = '% 6.4f' % direction.z
        self._pingCount.Value = '% 8.1f' % event.pingCount
        
    def _onPingerChoice(self, event):
        self._currentPingerID = event.GetSelection()
        if -1 != self._lastPingCount:
            event = self._pingerEvents[self._currentPingerID]
            self._updateNumericDisplay(event)

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

        estimator = core.Subsystem.getSubsystemOfType(
            ext.estimation.IStateEstimator, subsystems)

        if (vehicle is not None) and (estimator is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Sonar")
            paneInfo = paneInfo.Caption("Sonar").Right()
        
            panel = SonarPanel(parent, eventHub, vehicle, estimator)
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


class VelocityPosition(BasePanel):
    implements(IPanelProvider)
    
    def __init__(self, parent, eventHub, estimator, *args, **kwargs):
        BasePanel.__init__(self, parent, *args, **kwargs)
    
        self.estimator = estimator

        # Controls
        self._createControls("Pos & Vel", startEnable = True)

        # Only update the velocity and position at the timer (so we can read it)
        #self._timer = wx.Timer()
        #self._timer.Bind(wx.EVT_TIMER, self._update)
        #self._timer.Start(100)
        
        # Events
        conn = eventHub.subscribeToType(
            ext.estimation.IStateEstimator.ESTIMATED_VELOCITY_UPDATE,
            self._onEstimatedVelocityUpdate)
        self._connections.append(conn)
        
        conn = eventHub.subscribeToType(
            ext.estimation.IStateEstimator.ESTIMATED_POSITION_UPDATE,
            self._onEstimatedPositionUpdate)
        self._connections.append(conn)
     
        conn = eventHub.subscribeToType(ext.control.IController.DESIRED_POSITION_UPDATE, self._onDesiredPositionUpdate)
        self._connections.append(conn)
       
        conn = eventHub.subscribeToType(ext.control.IController.DESIRED_VELOCITY_UPDATE, self._onDesiredVelocityUpdate)
        self._connections.append(conn)
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
    
    def _createDataControls(self):
        self._createDataControl(controlName = '_xEPos', label = 'Estimated X Pos: ')
        self._createDataControl(controlName = '_yEPos', label = 'Estimated Y Pos: ')
        self._createDataControl(controlName = '_xEVel', label = 'Estimated X Vel: ')
        self._createDataControl(controlName = '_yEVel', label = 'Esimated Y Vel: ')
     
        self._createDataControl(controlName = '_xDPos', label = 'Desired X Pos:') 
        self._createDataControl(controlName = '_yDPos', label = 'Desired Y Pos:')
        self._createDataControl(controlName = '_xDVel', label = 'Desired X Vel:')
        self._createDataControl(controlName = '_yDVel', label = 'Desired Y Vel:')
    def _onEstimatedVelocityUpdate(self, event):
        vel = event.vector2
        self._xEVel.Value = "% 4.2f" % vel.x
        self._yEVel.Value = "% 4.2f" % vel.y

    def _onEstimatedPositionUpdate(self, event):
        pos = event.vector2
        self._xEPos.Value = "% 4.2f" % pos.x
        self._yEPos.Value = "% 4.2f" % pos.y
        
    def _onDesiredVelocityUpdate(self, event):
        vel = event.vector2
        self._xDVel.Value = "% 4.2f" % vel.x
        self._yDVel.Value = "% 4.2f" % vel.y
        
    
    def _onDesiredPositionUpdate(self, event):
        pos = event.vector2
        self._xDPos.Value = "% 4.2f" % pos.x
        self._yDPos.Value = "% 4.2f" % pos.y

    def _onClose(self, event):
        for conn in self. _connections:
            conn.disconnect() 
       
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        estimator = core.Subsystem.getSubsystemOfType(
            ext.estimation.IStateEstimator, subsystems)
        
        if estimator is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Pos & Vel")
            paneInfo = paneInfo.Caption("Pos & Vel").Right()
        
            panel = VelocityPosition(parent, eventHub, estimator)
            return [(paneInfo, panel, [estimator])]
        
        return []

class EventPlayerPanel(wx.Panel):
    implements(IPanelProvider)

    FORMAT_HOURS = 1
    FORMAT_MINUTES = 2
    FORMAT_SECONDS = 3
    FPS = 30

    def __init__(self, parent, eventPlayer, eventHub, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        # TODO: Figure out how to properly do this
        self._eventPlayer = eventPlayer
        self._format = EventPlayerPanel.FORMAT_SECONDS
        self._sliderDown = False
        self._play = wx.Button(self, label = "Play")
        self._stop = wx.Button(self, label = "Stop")
        self._slider = wx.Slider(self, 0, 0, 100)
        self._text = wx.StaticText(self, label = '0.00 / 0.00',
                             style = wx.ALIGN_RIGHT)

        self._sizer = wx.BoxSizer(wx.HORIZONTAL)
        self._sizer.Add(self._play, proportion = 0,
                        flag = wx.ALIGN_CENTER | wx.ALL, border = 3)
        self._sizer.Add(self._stop, proportion = 0,
                        flag = wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM,
                        border = 3)
        self._sizer.Add(self._slider, proportion = 1,
                        flag = wx.ALIGN_CENTER | wx.ALIGN_CENTER |
                        wx.TOP | wx.BOTTOM, border = 3)
        self._sizer.Add(self._text, proportion = 0,
                        flag = wx.ALIGN_CENTER | wx.ALL, border = 3)
        self._sizer.SetSizeHints(self)

        self._connections = []

        # Subscribe to eventPlayer events
        conn = eventHub.subscribeToType(
            ext.logging.EventPlayer.PLAYER_UPDATE,
            self._onNewImage)
        self._connections.append(conn)

        conn = eventHub.subscribeToType(
            ext.logging.EventPlayer.PLAYER_SETUP,
            self._onImageSourceChanged)
        self._connections.append(conn)

        # Bind the play/stop buttons
        self._play.Bind(wx.EVT_BUTTON, self._onPlay)
        self._stop.Bind(wx.EVT_BUTTON, self._onStop)

        # Bind the slider events
        self._slider.Bind(wx.EVT_SCROLL, self._onScrollChanged)
        self.SetSizer(self._sizer)

    def _onNewImage(self, event):
        self._updateTimeDisplay()
        if not self._sliderDown:
            self._slider.SetValue(self._eventPlayer.currentTime() * \
                                      EventPlayerPanel.FPS)

    def _onImageSourceChanged(self, event):
        if not self._sliderDown:
            self._slider.Enable()
            self._slider.SetRange(0, (EventPlayerPanel.FPS*self._eventPlayer.duration()))

        self._determineTimeFormat()
        self._updateTimeDisplay()
        self._sizer.RecalcSizes()
        self._sizer.Layout()
        
    def _onPlay(self, event):
        self._eventPlayer.start()

    def _onStop(self, event):
        self._eventPlayer.stop()

    def _onThumbTrack(self, event):
        self._sliderDown = True
        self._updateBasedOnSliderEvent(event)

    def _onThumbRelease(self, event):
        self._sliderDown = False

    def _onScrollChanged(self, event):
        self._updateBasedOnSliderEvent(event)

    def _updateBasedOnSliderEvent(self, event):
        timeStamp = event.GetPosition() / EventPlayerPanel.FPS
        self._eventPlayer.seekToTime(timeStamp)
        self._updateTimeDisplay()

    def _updateTimeDisplay(self):
        time = self._breakUpTime(self._eventPlayer.currentTime())
        totalTime = self._breakUpTime(self._eventPlayer.duration())

        if self._format == EventPlayerPanel.FORMAT_HOURS:
            label = '%02d:%02d:%4.2f / %02d:%02d:%4.2f' % \
                (time[0], time[1], time[2], totalTime[0], totalTime[1], \
                 totalTime[2])
        elif self._format == EventPlayerPanel.FORMAT_MINUTES:
            label = '%02d:%4.2f / %02d:%4.2f' % \
                (time[1], time[2], totalTime[1], totalTime[2])
        elif self._format == EventPlayerPanel.FORMAT_SECONDS:
            label = '%4.5f / %4.5f' % (time[2], totalTime[2])
        else:
            label = 'ERROR'
        self._text.SetLabel(label)

    def _determineTimeFormat(self):
        duration = self._eventPlayer.duration()
        time = self._breakUpTime(duration)

        if 0 != time[0]:
            self._format = EventPlayerPanel.FORMAT_HOURS
        elif 0 != time[1]:
            self._format = EventPlayerPanel.FORMAT_MINUTES
        else:
            self._format = EventPlayerPanel.FORMAT_SECONDS

    def _breakUpTime(self, seconds):
        outHours = math.floor(seconds/3600)
        seconds -= outHours * 3600

        outMinutes = math.floor(seconds/60)

        outSeconds = seconds - outMinutes * 60

        return (outHours, outMinutes, outSeconds)

    @staticmethod
    def getPanels(subsystems, parent):
        eventPlayer = core.Subsystem.getSubsystemOfType(
            logging.EventPlayer, subsystems)

        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, 
                                                     nonNone = True)
        
        if eventPlayer is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Event Player")
            paneInfo = paneInfo.Caption("Event Player").Left()

            return [(paneInfo, EventPlayerPanel(parent, eventPlayer, 
                                                eventHub),
                     [eventPlayer])]

        return []

class RecorderPanel(wx.Panel):
    implements(IPanelProvider)

    def __init__(self, parent, vision, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        layout = wx.GridBagSizer(10, 10)

        streamLabel = wx.StaticText(self, label = 'Recording')
        layout.Add(streamLabel, (0, 0), flag = wx.ALIGN_CENTER,
                   span = (1, 2))
        self._vision = vision
       
        self._fstreamButton = wx.ToggleButton(self, label = "Forward On")
        layout.Add(self._fstreamButton, (1, 0), flag = wx.ALIGN_CENTER)
        self._fstreamButton.Bind(wx.EVT_TOGGLEBUTTON, self._onForwardPressed)
        self._forwardRecorderName = "None"
        
        self._dstreamButton = wx.ToggleButton(self, label = "Downward On")
        layout.Add(self._dstreamButton, (1,1), flag = wx.ALIGN_CENTER)
        self._dstreamButton.Bind(wx.EVT_TOGGLEBUTTON, self._onDownwardPressed)
        self._downwardRecorderName = "None"

        self.SetSizerAndFit(layout)
   
    def _onForwardPressed(self, event):
        timeStamp = datetime.fromtimestamp(timer.time())
        name = timeStamp.strftime("%Y%m%d%H%M%S") + "_forward.rmv"
        if self._fstreamButton.GetValue() == True:
            self._forwardRecorderName = name
            self._vision.addForwardRecorder(name, 5)
            self._fstreamButton.SetLabel(label = "Forward Off")
        else:
            self._vision.removeForwardRecorder(self._forwardRecorderName)
            self._fstreamButton.SetLabel(label = "Forward On")
    
    def _onDownwardPressed(self, event):
        timeStamp = datetime.fromtimestamp(timer.time())
        name = timeStamp.strftime("%Y%m%d%H%M%S") + "_downward.rmv"
        if self._dstreamButton.GetValue() == True:
            self._downwardRecorderName = name
            self._vision.addDownwardRecorder(name, 5)
            self._dstreamButton.SetLabel(label = "Downward Off")
        else:
            self._vision.removeDownwardRecorder(self._downwardRecorderName)
            self._dstreamButton.SetLabel(label = "Downward On")
            
    @staticmethod
    def getPanels(subsystems, parent):
        vision = ext.core.Subsystem.getSubsystemOfType(ext.vision.VisionSystem,
                                             subsystems)

        if vision is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Recorder")
            paneInfo = paneInfo.Caption("Recorder").Left()

            return [(paneInfo, RecorderPanel(parent, vision),
                     [vision])]

        return []

class MonitorPanel(BasePanel):
    implements(IPanelProvider)

    # These correspond to the LED color, NOT the threat level
    NOMINAL = 2
    WARNING = 1
    CRITICAL = 0

    def __init__(self, parent, eventHub, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        self._layout = wx.FlexGridSizer(2, 1, 5, 0)
        self._layout.AddGrowableCol(0)
        self._layout.AddGrowableRow(1)

        infoBox = wx.FlexGridSizer(1, 3, 0, 5)
        infoBox.AddGrowableRow(0)
        infoBox.AddGrowableCol(1)

        # LED
        size = (self._getTextSize()[0], ram.gui.led.LED.HEIGHT)
        self._buoyLED = ram.gui.led.LED(self, state = 3, size = size)
        self._buoyLED.MinSize = size
        infoBox.Add(self._buoyLED, 1, flag = wx.ALIGN_CENTER | wx.EXPAND)
        self._layout.Add(infoBox, 1, flag = wx.ALIGN_CENTER | wx.EXPAND)

        label = wx.StaticText(self, label = 'Status')
        infoBox.Add(label, 1, flag = wx.ALIGN_CENTER)

        self._display = True
        self._hide = wx.Button(self, label = "Hide")
        infoBox.Add(self._hide, 1, flag = wx.ALIGN_CENTER)
        self._hide.Bind(wx.EVT_BUTTON, self._onButton)

        self._messages = wx.ListBox(self)
        self._layout.Add(self._messages, 1, flag = wx.EXPAND)

        self._warningLevels = {}

        self.SetSizerAndFit(self._layout)

        # Subscribe to monitor subsystem signals
        self._connections = []
        conn = eventHub.subscribeToType(monitor.Monitor.NOMINAL,
                                        self._onNominalSignal)
        self._connections.append(conn)

        conn = eventHub.subscribeToType(monitor.Monitor.WARNING,
                                        self._onWarningSignal)
        self._connections.append(conn)

        conn = eventHub.subscribeToType(monitor.Monitor.CRITICAL,
                                        self._onCriticalSignal)
        self._connections.append(conn)

    def _toggleDisplay(self, display):
        self._display = display

        # Show/Hide button text change
        if self._display:
            self._hide.Label = "Hide"
        else:
            self._hide.Label = "Show"

        self._layout.Show(1, display)
        self._layout.Layout()

    def _onButton(self, event):
        self._toggleDisplay(not self._display)

    def _onClose(self, event):
        for conn in self._connections:
            conn.disconnect()

    def _onNominalSignal(self, event):
        eventType, property, name, value = event.string.split(':', 4)
        self._messages.InsertItems(['NOMINAL: ' + name + ' - ' + value], 0)
        self._warningLevels[name] = MonitorPanel.NOMINAL
        self._refreshWarningState()

    def _onWarningSignal(self, event):
        eventType, property, name, value = event.string.split(':', 4)
        self._messages.InsertItems(['WARNING: ' + name + ' - ' + value], 0)
        self._warningLevels[name] = MonitorPanel.WARNING
        self._refreshWarningState()

    def _onCriticalSignal(self, event):
        eventType, property, name, value = event.string.split(':', 4)
        self._messages.InsertItems(['CRITICAL: ' + name + ' - ' + value], 0)
        self._warningLevels[name] = MonitorPanel.CRITICAL
        self._refreshWarningState()

    def _refreshWarningState(self):
        """
        Sets the LED to reflect the worst warning condition currently in effect
        """
        highestLevel = MonitorPanel.NOMINAL
        for name, state in self._warningLevels.iteritems():
            if state == MonitorPanel.CRITICAL:
                highestLevel = MonitorPanel.CRITICAL
            elif state == MonitorPanel.WARNING and \
                    highestLevel != MonitorPanel.CRITICAL:
                highestLevel = MonitorPanel.WARNING
            else:
                # Do not change anything if state is nominal
                pass

        self._buoyLED.SetState(highestLevel)

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
                                                     subsystems, nonNone = True)

        mainMonitor = core.Subsystem.getSubsystemOfType(monitor.Monitor,  
                                                        subsystems)

        if (eventHub is not None) and (mainMonitor is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name("Monitor")
            paneInfo = paneInfo.Caption("Monitor").Left()

            return [(paneInfo, MonitorPanel(parent, eventHub),
                     [eventHub])]

        return []

class CpuPanel(BasePanel):
    implements(IPanelProvider)

    def __init__(self, parent, eventHub, *args, **kwargs):
        BasePanel.__init__(self, parent, *args, **kwargs)

        textSize = self._getTextSize()
        textStyle = wx.TE_RIGHT | wx.TE_READONLY

        self._text = wx.TextCtrl(self, size = textSize, style = textStyle)

        self._filter = filter.MovingAverageFilter(10)

        # Subscribe to cpu idle event
        self._conn = eventHub.subscribeToType(monitor.CpuMonitor.IDLE_UPDATE,
                                              self._onUpdate)

    def _onUpdate(self, event):
        # Given as a percentage 0-100
        idle_time = event.number

        # Run an average filter on the incoming data
        self._filter.append(100 - idle_time)

        # Cpu usage is 100 - idle_time
        self._text.SetValue(str(self._filter.getAverage()))

    def _onClose(self, event):
        self._conn.disconnect()
        BasePanel._onClose(self, event)

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,
                                                     subsystems, nonNone = True)

        cpuMonitor = core.Subsystem.getSubsystemOfType(monitor.CpuMonitor,
                                                    subsystems)


        if (eventHub is not None) and (cpuMonitor is not None):
            paneInfo = wx.aui.AuiPaneInfo().Name('Cpu')
            paneInfo = paneInfo.Caption('Cpu').Left()

            return [(paneInfo, CpuPanel(parent, eventHub), [eventHub])]

        return []
        

#class SeekObjectivePanel(BasePanel):
#    implements(IPanelProvider)

#    def __init__(self, parent, eventHub, machine, *args, **kwargs):
#        BasePanel.__init__(self, parent, *args, **kwargs)

        # Make sure we shut down all events on close
#        self.Bind(wx.EVT_CLOSE, self._onClose)

#        self._eventHub = eventHub
#        self._machine = machine
#        self._targetHub = None

#        self._createControls("Objective")

        # Create the reference to the object we will track
#        self._target = None

#        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_ENTERED,
#                                        self._onEntered)
#        self._connections.append(conn)

#        conn = eventHub.subscribeToType(ram.ai.state.Machine.STATE_EXITED,
#                                        self._onExited)
#        self._connections.append(conn)

#        self._targetConnections = []

#    def enableControls(self):
#        for control in self._generatedControls:
#            control.Enable()

#    def disableControls(self):
#        for control in self._generatedControls:
#            control.Disable()

#    def _createDataControls(self):
#        self._createDataControl(controlName = '_x', label = 'X Pos: ')
#        self._createDataControl(controlName = '_y', label = 'Y Pos: ')

#    def _onEntered(self, event):
        # Get the current state
#        cstate = self._machine.currentState()

        # Check if there are any branches. If there are, we are in a task
        # and need to check the first branch instead
#        if len(self._machine.branches) > 0:
            # This will just take the first branch as the entered branch
            # TODO: Think of a better way to do this that isn't as fragile
            #print self._machine.branches.values()
            #print type(self._machine.branches.values()[0])
#            cstate = self._machine.branches.values()[0].currentState()

        # Check the state for any tracked objects in the attributes
#        for var in dir(cstate):
#            if isinstance(getattr(cstate, var), motion.common.Target):
                # We have found the target, now track it
#                self._target = getattr(cstate, var)
                # Subscribe to this target's update events and store the
                # connection as our first connection
#                self._targetConnections.insert(0, self._target.subscribe(
#                    motion.common.Target.UPDATE, self._onUpdate))
#                self.enableControls()

#    def _onExited(self, event):
        # Delete the last connection as it is the one that exited
#        if len(self._targetConnections) > 0:
            # Stop the connection and remove it
#            self._targetConnections.pop().disconnect()
            
#        self._target = None
#        self.disableControls()

#    def _onUpdate(self, event):
#        if self._target is not None:
#            errorAdj = self._target.errorAdj()
#            self._x.Value = "% 4.2f" % errorAdj[0]
#            self._y.Value = "% 4.2f" % errorAdj[1]

#    def _onClose(self, event):
#        for conn in self._targetConnections:
#            conn.disconnect()
#        BasePanel._onClose(self, event)

#    @staticmethod
#    def getPanels(subsystems, parent):
#        eventHub = core.Subsystem.getSubsystemOfType(core.QueuedEventHub,  
#                                                     subsystems, nonNone = True)
        
#        machine = core.Subsystem.getSubsystemOfType(ram.ai.state.Machine,
#                                                    subsystems)

#        if eventHub is not None and machine is not None:
#            paneInfo = wx.aui.AuiPaneInfo().Name("Seek Objective")
#            paneInfo = paneInfo.Caption("Seek Objective").Left()

#            return [(paneInfo, SeekObjectivePanel(parent, eventHub, machine),
#                     [machine])]

#        return []

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
    
