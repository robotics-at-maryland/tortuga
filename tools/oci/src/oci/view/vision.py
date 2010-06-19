# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/view.py

# Library Imports
import wx

# Project Imports
import ram.core as core
import ext.math
import ext.core
import ext.vision
import ram.gui.led
import ram.gui.view as view
import ram.ai.subsystem
import ram.ai.bin
import ram.ai.buoy
from oci.view.panels import BasePanel

class MasterVisionPanel(BasePanel):
    core.implements(view.IPanelProvider)
    
    def __init__(self, parent, eventHub, vision, ai, machine, *args, **kwargs):
        BasePanel.__init__(self, parent, *args, **kwargs)
        
        # Make sure we shut down all events on close
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
        self._sizer = wx.BoxSizer(wx.VERTICAL)
        
        if vision is not None:
            #redLightPanel = RedLightPanel(self, self._childChangedSize, eventHub, vision)
            #self._sizer.Add(redLightPanel)

            buoyPanel = BuoyPanel(self, self._childChangedSize, eventHub, vision, machine)
            self._sizer.Add(buoyPanel)
            
            pipePanel = OrangePipePanel(self, self._childChangedSize, eventHub, vision)
            self._sizer.Add(pipePanel)
            
            binPanel = BinPanel(self, self._childChangedSize, eventHub, vision, ai = ai)
            self._sizer.Add(binPanel)
            
            targetPanel = TargetPanel(self, self._childChangedSize, eventHub, vision)
            self._sizer.Add(targetPanel)

            windowPanel = WindowPanel(self, self._childChangedSize, eventHub, vision, machine = machine)
            self._sizer.Add(windowPanel)

            #barbedWirePanel = BarbedWirePanel(self, self._childChangedSize, eventHub, vision)
            #self._sizer.Add(barbedWirePanel)

            hedgePanel = HedgePanel(self, self._childChangedSize, eventHub,
                                    vision)
            self._sizer.Add(hedgePanel)
            
        self.SetSizerAndFit(self._sizer)
        
    def _childChangedSize(self):
        self.SetSizerAndFit(self._sizer)
        
    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = ext.core.Subsystem.getSubsystemOfType(ext.core.QueuedEventHub,  
                                                     subsystems, nonNone = True)
        
        vision = ext.core.Subsystem.getSubsystemOfType(ext.vision.VisionSystem, 
                                                       subsystems)
        
        ai = ext.core.Subsystem.getSubsystemOfType(
                    ram.ai.subsystem.AI, subsystems)

        machine = ext.core.Subsystem.getSubsystemOfType(
            ram.ai.state.Machine, subsystems)

        if vision is not None:
            paneInfo = wx.aui.AuiPaneInfo().Name("Vision")
            paneInfo = paneInfo.Caption("Vision").Right()
        
            panel = MasterVisionPanel(parent, eventHub, vision, ai, machine)
            return [(paneInfo, panel, [vision])]
        
        return []
        
        

class BaseVisionPanel(BasePanel):
    
    def __init__(self, parent, buttonHandler, *args, **kwargs):
        BasePanel.__init__(self, parent, *args, **kwargs)
        self._controlsShowing = True
        self._hide = None
        self._bouyLED = None
        self._buttonHandler = buttonHandler
        
        # Make sure we shut down all events on close
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
    def _subscribeToType(self, eventHub, eventType, handler):
        """
        Subscribes the event of the desired type, with the given handler and
        adds the connections to our list of connections.
        """
        conn = eventHub.subscribeToType(eventType, handler)
        self._connections.append(conn)
                
    def _createControls(self, name):
        # Create box around controls
        box = wx.StaticBox(parent = self, label = name)
        topSizer = wx.StaticBoxSizer(box)
        
        self.sizer = wx.FlexGridSizer(0, 2, 10, 10)
        topSizer.Add(self.sizer, 1, wx.EXPAND)
        
        # Buoy Text and Label
        self._hide = wx.Button(self, label = "Off")
        self.sizer.Add(self._hide, 1, flag = wx.ALIGN_CENTER)
        self._hide.Bind(wx.EVT_BUTTON, self._onButton)

        size = (self._getTextSize()[0], ram.gui.led.LED.HEIGHT)
        self._bouyLED = ram.gui.led.LED(self, state = 3, size = size)
        self._bouyLED.MinSize = size
        self.sizer.Add(self._bouyLED, 1, flag = wx.ALIGN_CENTER)
        
        self.SetSizerAndFit(topSizer)
        self._startSize = self.GetSize()
        
        # Create controls
        self._createDataControls()
        
        # Start off greyed out
        for control in self._generatedControls:
            control.Enable(False)
        
        self.SetSizerAndFit(topSizer)
        self._fullSize = self.GetSize()
        
        # Hide controls by default
        self._toggleSize(False)

    def _createDataControls(self):
        pass

    def _toggleSize(self, showControls):
        if not showControls:
            self._hide.Label = "On"
        else:
            self._hide.Label = "Off"
        
        self._controlsShowing = showControls
        for i in xrange(2, (len(self._generatedControls) + 1) * 2):
            self.sizer.Show(i, self._controlsShowing)
        self.sizer.Layout()
        
        if showControls:
            self.SetMinSize(self._fullSize)
            self.SetClientSize(self._fullSize)
        else:
            self.SetMinSize(self._startSize)
            self.SetClientSize(self._startSize)
            
        self._buttonHandler()

    def _onButton(self, event):
        self._toggleSize(showControls = not self._controlsShowing)
        
    def enableControls(self):
        for control in self._generatedControls:
            control.Enable()
        # The LED only does work when you change state, so calling this mutiple
        # times with the same value is ok
        self._bouyLED.SetState(2)
            
    def disableControls(self):
        for control in self._generatedControls:
            control.Enable(False)
        self._bouyLED.SetState(0)

    #@staticmethod
    #def getPanels(subsystems, parent):
    #    eventHub = ext.core.Subsystem.getSubsystemOfType(
    #        ext.core.QueuedEventHub, subsystems)
    #    
    #    vision = ext.core.Subsystem.getSubsystemOfType(ext.vision.VisionSystem, 
    #                                                   subsystems)
    #    ai = ext.core.Subsystem.getSubsystemOfType(
    #        ram.ai.subsystem.AI, subsystems)
    #    
    #    if vision is not None:
    #        buoyPaneInfo = wx.aui.AuiPaneInfo().Name("Red Light")
    #        buoyPaneInfo = buoyPaneInfo.Caption("Red Light").Left()
    #        redLightPanel = RedLightPanel(parent, eventHub, vision)
    #        
    #        pipePaneInfo = wx.aui.AuiPaneInfo().Name("Orange Pipe")
    #        pipePaneInfo = pipePaneInfo.Caption("Orange Pipe").Left()
    #        pipePanel = OrangePipePanel(parent, eventHub, vision)
    #        
    #        binPaneInfo = wx.aui.AuiPaneInfo().Name("Bin")
    #        binPaneInfo = binPaneInfo.Caption("Bin").Left()
    #        binPanel = BinPanel(parent, eventHub, vision, ai = ai)

    #        ductPaneInfo = wx.aui.AuiPaneInfo().Name("Duct")
    #        ductPaneInfo = ductPaneInfo.Caption("Duct").Left()
    #        ductPanel = DuctPanel(parent, eventHub, vision)
    #        
    #        safePaneInfo = wx.aui.AuiPaneInfo().Name("Safe")
    #        safePaneInfo = safePaneInfo.Caption("Safe").Left()
    #        safePanel = SafePanel(parent, eventHub, vision)
    #        
    #        targetPaneInfo = wx.aui.AuiPaneInfo().Name("Target")
    #        targetPaneInfo = targetPaneInfo.Caption("Target").Left()
    #        targetPanel = TargetPanel(parent, eventHub, vision)

    #        barbedWirePaneInfo = wx.aui.AuiPaneInfo().Name("BarbedWire")
    #        barbedWirePaneInfo = \
    #            barbedWirePaneInfo.Caption("BarbedWire").Left()
    #        barbedWirePanel = BarbedWirePanel(parent, eventHub, vision)

    #        return [(buoyPaneInfo, redLightPanel, [vision]), 
    #                (pipePaneInfo, pipePanel, [vision]), 
    #                (binPaneInfo, binPanel, [vision]),
    #                (ductPaneInfo, ductPanel, [vision]),
    #                (safePaneInfo, safePanel, [vision]),
    #                (barbedWirePaneInfo, barbedWirePanel, [vision]),
    #                (targetPaneInfo, targetPanel, [vision])]
        
    #    return []

class RedLightPanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._azimuth = None
        self._elevation = None
        self._range = None
        self._detector = False
        self._vision = vision

        # Controls
        self._createControls("RedLight")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.LIGHT_FOUND, 
                              self._onBouyFound)

        self._subscribeToType(eventHub, ext.vision.EventType.LIGHT_LOST,
                              self._onBouyLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              RED_LIGHT_DETECTOR_ON,
                              self._redLightDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              RED_LIGHT_DETECTOR_OFF,
                              self._redLightDetectorOff)
                
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_azimuth', label = 'Azimuth: ')
        self._createDataControl(controlName = '_elevation', 
                                label = 'Elevation: ')
        self._createDataControl(controlName = '_range', label = 'Range: ')
        
    def _onButton(self, event):
        """
        Overwrite the default onButton so this one just turns the detector on
        """
        if self._detector:
            self._vision.redLightDetectorOff()
        else:
            self._vision.redLightDetectorOn()

    def _onBouyFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y    
            self._azimuth.Value = "% 4.2f" % event.azimuth.valueDegrees()
            self._elevation.Value = "% 4.2f" % event.elevation.valueDegrees()
            self._range.Value = "% 4.2f" % event.range
        
            self.enableControls()
    
    def _onBouyLost(self, event):
        self.disableControls()

    def _redLightDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _redLightDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._azimuth.Value = ""
        self._elevation.Value = ""
        self._range.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)

class BuoyPanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, machine, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._r = None
        self._id = None
        self._color = None
        self._detector = False
        self._vision = vision
        self._machine = machine

        # Controls
        self._createControls("Buoy")

        # Store events received
        self._events = {}
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.BUOY_FOUND, 
                              self._onBouyFound)

        self._subscribeToType(eventHub, ext.vision.EventType.BUOY_LOST,
                              self._onBouyLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BUOY_DETECTOR_ON,
                              self._buoyDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BUOY_DETECTOR_OFF,
                              self._buoyDetectorOff)
                
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_azimuth', label = 'Azimuth: ')
        self._createDataControl(controlName = '_elevation', 
                                label = 'Elevation: ')
        self._createDataControl(controlName = '_range', label = 'Range: ')
        self._createDataControl(controlName = '_id', label = 'ID: ')
        self._createDataControl(controlName = '_color', label = 'Color: ')
        
    def _onButton(self, event):
        """
        Overwrite the default onButton so this one just turns the detector on
        """
        if self._detector:
            self._vision.buoyDetectorOff()
        else:
            self._vision.buoyDetectorOn()

    def _findClosest(self):
        assert len(self._events) > 0, 'Buoy list empty'

        closestColor = None
        for color, event in self._events.iteritems():
            if closestColor is None:
                closestColor = color
            else:
                current = self._events[closestColor]
                if ext.math.Vector2(event.x, event.y).squaredLength() < \
                        ext.math.Vector2(current.x, current.y).squaredLength():
                    closestColor = color

        return closestColor

    def _onBouyFound(self, event):
        if self._detector:
            self._events[event.color] = event

            color = None
            if self._machine is not None and \
                    hasattr(self._machine.currentState(), '_desiredColor'):
                color = self._machine.currentState()._desiredColor
            else:
                color = self._findClosest()

            obj = None
            if self._events.has_key(color):
                obj = self._events[color]

            if obj is not None:
                self._x.Value = "% 4.2f" % obj.x
                self._y.Value = "% 4.2f" % obj.y    
                self._azimuth.Value = "% 4.2f" % obj.azimuth.valueDegrees()
                self._elevation.Value = "% 4.2f" % obj.elevation.valueDegrees()
                self._range.Value = "% 4.2f" % obj.range
                self._id.Value = "%d" % obj.id
                self._color.Value = "%s" % obj.color

            self.enableControls()
    
    def _onBouyLost(self, event):
        self.disableControls()

    def _buoyDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _buoyDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._azimuth.Value = ""
        self._elevation.Value = ""
        self._range.Value = ""
        self._id.Value = ""
        self._color.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)

    def _distCompare(self, aID, bID):
        buoyData = self._ai.data['buoyData']['itemData']
        buoyA = buoyData[aID]
        buoyB = buoyData[bID]
        
        aDist = ext.math.Vector2(buoyA.x, buoyA.y).length()
        bDist = ext.math.Vector2(buoyB.x, buoyB.y).length()
        
        if aDist < bDist:
            return -1
        elif aDist > bDist:
            return 1
        return 0
        
class OrangePipePanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._angle = None
        self._detector = False
        self._vision = vision

        # Controls
        self._createControls("Orange Pipe")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.PIPE_FOUND, 
                                        self._onPipeFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.PIPE_LOST, 
                             self._onPipeLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              PIPELINE_DETECTOR_ON, self._pipeLineDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              PIPELINE_DETECTOR_OFF, self._pipeLineDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_angle', label = 'Angle: ')

    def _onButton(self, event):
        if self._detector:
            self._vision.pipeLineDetectorOff()
        else:
            self._vision.pipeLineDetectorOn()
        
    def _onPipeFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y    
            self._angle.Value = "% 4.2f" % event.angle.valueDegrees()
        
            self.enableControls()
    
    def _onPipeLost(self, event):
        self.disableControls()

    def _pipeLineDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _pipeLineDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._angle.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)
        
class SafePanel(BaseVisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._detector = False
        #self._angle = None

        # Controls
        self._createControls("Safe")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.SAFE_FOUND, 
                             self._onSafeFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.SAFE_LOST, 
                             self._onSafeLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              SAFE_DETECTOR_ON, self._safeDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              SAFE_DETECTOR_OFF, self._safeDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        #self._createDataControl(controlName = '_angle', label = 'Angle: ')
        
    def _onSafeFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y    
            #self._angle.Value = "% 4.2f" % event.angle.valueDegrees()
        
            self.enableControls()
    
    def _onSafeLost(self, event):
        self.disableControls()

    def _safeDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True

    def _safeDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        
class BinPanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, ai, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._angle = None
        self._symbol = None
        self._ai = ai
        self._detector = False
        self._vision = vision
        
        if self._ai is not None:
            ram.ai.bin.ensureBinTracking(eventHub, self._ai)
        
        # Controls
        self._createControls("Bin")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.BIN_FOUND, 
                             self._onBinFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.MULTI_BIN_ANGLE, 
                             self._onMultiBinAngle)
        
        self._subscribeToType(eventHub, ext.vision.EventType.BINS_LOST, 
                              self._onBinLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BIN_DETECTOR_ON, self._binDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BIN_DETECTOR_OFF, self._binDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_angle', label = 'Angle: ')
        self._createDataControl(controlName = '_multiAngle', label = 'M-Ang: ')
        self._createDataControl(controlName = '_symbol', label = 'Symbol: ')

    def _onButton(self, event):
        if self._detector:
            self._vision.binDetectorOff()
        else:
            self._vision.binDetectorOn()
        
    def _onMultiBinAngle(self, event):
        if self._detector:
            self._multiAngle.Value = "% 4.2f" % event.angle.valueDegrees()
        
    def _onBinFound(self, event):
        if self._detector:
            obj = event
        
            if self._ai is not None:
                # Use the current bin before anything else if it exists
                if self._ai.data['binData'].has_key('currentID'):
                    id = self._ai.data['binData']['currentID']
                    obj = self._ai.data['binData']['itemData'].get(id, None)
                else:
                    # Sorted closest to farthest
                    currentBinIDs = self._ai.data['binData'].get(
                        'currentIds', set())
                    currentBins = [b for b in currentBinIDs]
                    sortedBins = sorted(currentBins, self._distCompare)
                    obj = self._ai.data['binData']['itemData'][sortedBins[0]]
            
            if obj is not None:
                self._x.Value = "% 4.2f" % obj.x
                self._y.Value = "% 4.2f" % obj.y
                self._angle.Value = "% 4.2f" % obj.angle.valueDegrees()
                self._symbol.Value = "%s" % obj.symbol
            else:
                self._x.Value = "NO ID"
                self._y.Value = "NO ID"
                self._angle.Value = "NO ID"
                self._symbol.Value = "NO ID"
        
            self.enableControls()
    
    def _distCompare(self, aID, bID):
        binData = self._ai.data['binData']['itemData']
        binA = binData[aID]
        binB = binData[bID]
        
        aDist = ext.math.Vector2(binA.x, binA.y).length()
        bDist = ext.math.Vector2(binB.x, binB.y).length()
        
        if aDist < bDist:
            return -1
        elif aDist > bDist:
            return 1
        return 0
    
    def _onBinLost(self, event):
        self.disableControls()

    def _binDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _binDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._angle.Value = ""
        self._symbol.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)

class DuctPanel(BaseVisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._size = None
        self._alignment = None
        self._aligned = None
        self._detector = False

        # Controls
        self._createControls("Target")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.DUCT_FOUND, 
                             self._onTargetFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.DUCT_LOST, 
                             self._onTargetLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              DUCT_DETECTOR_ON, self._ductDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              DUCT_DETECTOR_OFF, self._ductDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_range', label = 'Range: ')
        self._createDataControl(controlName = '_alignment', label = 'Align: ')
        self._createDataControl(controlName = '_aligned', label = 'Aligned: ')
        
    def _onTargetFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y
            self._range.Value = "% 4.2f" % event.range
            self._alignment.Value = "% 4.2f" % event.alignment
            self._aligned.Value = "%s" % event.aligned
        
            self.enableControls()
    
    def _onTargetLost(self, event):
        self.disableControls()

    def _ductDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True

    def _ductDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._range.Value = ""
        self._alignment.Value = ""
        self._aligned.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False

class TargetPanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._size = None
        self._alignment = None
        self._aligned = None
        self._detector = False
        self._vision = vision

        # Controls
        self._createControls("Target")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.TARGET_FOUND, 
                             self._onTargetFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.TARGET_LOST, 
                             self._onTargetLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              TARGET_DETECTOR_ON, self._targetDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              TARGET_DETECTOR_OFF, self._targetDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_range', label = 'Range: ')
        self._createDataControl(controlName = '_squareNess', label = 'SQ-Ns: ')
        
    def _onButton(self, event):
        if self._detector:
            self._vision.targetDetectorOff()
        else:
            self._vision.targetDetectorOn()

    def _onTargetFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y
            self._squareNess.Value = "% 4.2f" % event.squareNess
            self._range.Value = "% 4.2f" % event.range
        
            self.enableControls()
    
    def _onTargetLost(self, event):
        self.disableControls()

    def _targetDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _targetDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._squareNess.Value = ""
        self._range.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)

class WindowPanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, machine, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._size = None
        self._alignment = None
        self._aligned = None
        self._detector = False
        self._vision = vision

        self._machine = machine

        # Create a dictionary for events received
        self._events = {}

        # Controls
        self._createControls("Window")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.WINDOW_FOUND, 
                             self._onWindowFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.WINDOW_LOST, 
                             self._onWindowLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              WINDOW_DETECTOR_ON, self._windowDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              WINDOW_DETECTOR_OFF, self._windowDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X Pos: ')
        self._createDataControl(controlName = '_y', label = 'Y Pos: ')
        self._createDataControl(controlName = '_range', label = 'Range: ')
        self._createDataControl(controlName = '_squareNess', label = 'SQ-Ns: ')
        self._createDataControl(controlName = '_color', label = 'Color: ')
        
    def _onButton(self, event):
        if self._detector:
            self._vision.windowDetectorOff()
        else:
            self._vision.windowDetectorOn()

    def _findClosest(self):
        assert len(self._events) > 0, 'Window list empty'

        closestColor = None
        for color, event in self._events.iteritems():
            if closestColor is None:
                closestColor = color
            else:
                current = self._events[closestColor]
                if ext.math.Vector2(event.x, event.y).squaredLength() < \
                        ext.math.Vector2(current.x, current.y).squaredLength():
                    closestColor = color

        return closestColor

    def _onWindowFound(self, event):
        if self._detector:
            self._events[event.color] = event

            color = None
            if self._machine is not None and \
                    hasattr(self._machine.currentState(), '_desiredColor'):
                color = self._machine.currentState()._desiredColor
            else:
                color = self._findClosest()

            obj = None
            if self._events.has_key(color):
                obj = self._events[color]

            if obj is not None:
                self._x.Value = "% 4.2f" % obj.x
                self._y.Value = "% 4.2f" % obj.y
                self._squareNess.Value = "% 4.2f" % obj.squareNess
                self._range.Value = "% 4.2f" % obj.range
                self._color.Value = "%s" % obj.color
        
            self.enableControls()
    
    def _onWindowLost(self, event):
        if self._events.has_key(event.color):
            del self._events[event.color]
        if len(self._events) == 0:
            self.disableControls()

    def _windowDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _windowDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._squareNess.Value = ""
        self._range.Value = ""
        self._color.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)

class BarbedWirePanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._topX = None
        self._topY = None
        self._topWidth = None
        self._bottomX = None
        self._bottomY = None
        self._bottomWidth = None
        self._detector = False
        self._vision = vision

        # Controls
        self._createControls("BarbedWire")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.BARBED_WIRE_FOUND, 
                             self._onBarbedWireFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.BARBED_WIRE_LOST, 
                             self._onBarbedWireLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BARBED_WIRE_DETECTOR_ON,
                              self._barbedWireDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              BARBED_WIRE_DETECTOR_OFF,
                              self._barbedWireDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_topX', label = 'Top X: ')
        self._createDataControl(controlName = '_topY', label = 'Top Y: ')
        self._createDataControl(controlName = '_topWidth', 
                                label = 'Top Width: ')
        self._createDataControl(controlName = '_bottomX', label = 'Bot X: ')
        self._createDataControl(controlName = '_bottomY', label = 'Bot Y: ')
        self._createDataControl(controlName = '_bottomWidth',
                                label = 'Bot Width: ')
        
    def _onButton(self, event):
        if self._detector:
            self._vision.barbedWireDetectorOff()
        else:
            self._vision.barbedWireDetectorOn()

    def _onBarbedWireFound(self, event):
        if self._detector:
            self._topX.Value = "% 4.2f" % event.topX
            self._topY.Value = "% 4.2f" % event.topY
            self._topWidth.Value = "% 4.2f" % event.topWidth
            self._bottomX.Value = "% 4.2f" % event.bottomX
            self._bottomY.Value = "% 4.2f" % event.bottomY
            self._bottomWidth.Value = "% 4.2f" % event.bottomWidth
        
            self.enableControls()
    
    def _onBarbedWireLost(self, event):
        self.disableControls()

    def _barbedWireDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _barbedWireDetectorOff(self, event):
        self._topX.Value = ""
        self._topY.Value = ""
        self._topWidth.Value = ""
        self._bottomX.Value = ""
        self._bottomY.Value = ""
        self._bottomWidth.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)


class HedgePanel(BaseVisionPanel):
    def __init__(self, parent, buttonHandler, eventHub, vision, *args, **kwargs):
        BaseVisionPanel.__init__(self, parent, buttonHandler, *args, **kwargs)
        self._x = None
        self._y = None
        self._squareNess = None
        self._range = None
        self._detector = False
        self._vision = vision

        # Controls
        self._createControls("Hedge")
        
        # Events
        self._subscribeToType(eventHub, ext.vision.EventType.HEDGE_FOUND, 
                             self._onHedgeFound)
        
        self._subscribeToType(eventHub, ext.vision.EventType.HEDGE_LOST, 
                             self._onHedgeLost)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              HEDGE_DETECTOR_ON,
                              self._hedgeDetectorOn)

        self._subscribeToType(eventHub, ext.vision.EventType.
                              HEDGE_DETECTOR_OFF,
                              self._hedgeDetectorOff)
        
    def _createDataControls(self):
        self._createDataControl(controlName = '_x', label = 'X: ')
        self._createDataControl(controlName = '_y', label = 'Y: ')
        self._createDataControl(controlName = '_squareNess', label = 'SQ-Ns: ')
        self._createDataControl(controlName = '_range', label = 'R: ')
                
    def _onButton(self, event):
        if self._detector:
            self._vision.hedgeDetectorOff()
        else:
            self._vision.hedgeDetectorOn()

    def _onHedgeFound(self, event):
        if self._detector:
            self._x.Value = "% 4.2f" % event.x
            self._y.Value = "% 4.2f" % event.y
            self._squareNess.Value = "% 4.2f" % event.squareNess
            self._range.Value = "% 4.2f" % event.range
        
            self.enableControls()
    
    def _onHedgeLost(self, event):
        self.disableControls()

    def _hedgeDetectorOn(self, event):
        self._bouyLED.SetState(0)
        self._detector = True
        self._toggleSize(True)

    def _hedgeDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._squareNess.Value = ""
        self._range.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        self._toggleSize(False)
