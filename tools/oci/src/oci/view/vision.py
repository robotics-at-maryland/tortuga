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
from oci.view.panels import BasePanel

class VisionPanel(BasePanel):
    core.implements(view.IPanelProvider)
    
    def __init__(self, parent, *args, **kwargs):
        BasePanel.__init__(self, parent, *args, **kwargs)
        self._controlsShowing = True
        self._hide = None
        self._bouyLED = None
        
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
        # Creat box around controls
        box = wx.StaticBox(parent = self, label = name)
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
        
        # Create controls\
        self._createDataControls()

        # Start off greyed out
        for control in self._generatedControls:
            control.Enable(False)
        
        self.SetSizerAndFit(topSizer)

    def _createDataControls(self):
        pass

    def _onButton(self, event):
        if self._controlsShowing:
            self._hide.Label = "Show"
        else:
            self._hide.Label = "Hide"
        
        self._controlsShowing = not self._controlsShowing
        for i in xrange(2, (len(self._generatedControls) + 1) * 2):
            self.sizer.Show(i, self._controlsShowing)
        self.sizer.Layout()
        
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

    @staticmethod
    def getPanels(subsystems, parent):
        eventHub = ext.core.Subsystem.getSubsystemOfType(
            ext.core.QueuedEventHub, subsystems)
        
        vision = ext.core.Subsystem.getSubsystemOfType(ext.vision.VisionSystem, 
                                                       subsystems)
        ai = ext.core.Subsystem.getSubsystemOfType(
            ram.ai.subsystem.AI, subsystems)
        
        if vision is not None:
            buoyPaneInfo = wx.aui.AuiPaneInfo().Name("Red Light")
            buoyPaneInfo = buoyPaneInfo.Caption("Red Light").Left()
            buoyPanel = RedLightPanel(parent, eventHub, vision)
            
            pipePaneInfo = wx.aui.AuiPaneInfo().Name("Orange Pipe")
            pipePaneInfo = pipePaneInfo.Caption("Orange Pipe").Left()
            pipePanel = OrangePipePanel(parent, eventHub, vision)
            
            binPaneInfo = wx.aui.AuiPaneInfo().Name("Bin")
            binPaneInfo = binPaneInfo.Caption("Bin").Left()
            binPanel = BinPanel(parent, eventHub, vision, ai = ai)

            ductPaneInfo = wx.aui.AuiPaneInfo().Name("Duct")
            ductPaneInfo = ductPaneInfo.Caption("Duct").Left()
            ductPanel = DuctPanel(parent, eventHub, vision)
            
            safePaneInfo = wx.aui.AuiPaneInfo().Name("Safe")
            safePaneInfo = safePaneInfo.Caption("Safe").Left()
            safePanel = SafePanel(parent, eventHub, vision)
            
            targetPaneInfo = wx.aui.AuiPaneInfo().Name("Target")
            targetPaneInfo = targetPaneInfo.Caption("Target").Left()
            targetPanel = TargetPanel(parent, eventHub, vision)

            barbedWirePaneInfo = wx.aui.AuiPaneInfo().Name("BarbedWire")
            barbedWirePaneInfo = \
                barbedWirePaneInfo.Caption("BarbedWire").Left()
            barbedWirePanel = BarbedWirePanel(parent, eventHub, vision)

            return [(buoyPaneInfo, buoyPanel, [vision]), 
                    (pipePaneInfo, pipePanel, [vision]), 
                    (binPaneInfo, binPanel, [vision]),
                    (ductPaneInfo, ductPanel, [vision]),
                    (safePaneInfo, safePanel, [vision]),
                    (barbedWirePaneInfo, barbedWirePanel, [vision]),
                    (targetPaneInfo, targetPanel, [vision])]
        
        return []

class RedLightPanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._azimuth = None
        self._elevation = None
        self._range = None
        self._detector = False

        # Controls
        self._createControls("Bouy")
        
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

    def _redLightDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._azimuth.Value = ""
        self._elevation.Value = ""
        self._range.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        
class OrangePipePanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._angle = None
        self._detector = False

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

    def _pipeLineDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._angle.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False
        
        
class SafePanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
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
        
class BinPanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, ai, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._angle = None
        self._symbol = None
        self._ai = ai
        self._detector = False
        
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
        
    def _onMultiBinAngle(self, event):
        if self._detector:
            self._multiAngle.Value = "% 4.2f" % event.angle.valueDegrees()
        
    def _onBinFound(self, event):
        if self._detector:
            obj = event
        
            if self._ai is not None:
                # Sorted closest to farthest
                currentBinIDs = self._ai.data['binData'].get('currentIds', set())
                currentBins = [b for b in currentBinIDs]
                sortedBins = sorted(currentBins, self._distCompare)
                obj = self._ai.data['binData']['itemData'][sortedBins[0]]
            
            self._x.Value = "% 4.2f" % obj.x
            self._y.Value = "% 4.2f" % obj.y
            self._angle.Value = "% 4.2f" % obj.angle.valueDegrees()
            self._symbol.Value = "%s" % obj.symbol
        
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

    def _binDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._angle.Value = ""
        self._symbol.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False

class DuctPanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
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

class TargetPanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
        self._x = None
        self._y = None
        self._size = None
        self._alignment = None
        self._aligned = None
        self._detector = False

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

    def _targetDetectorOff(self, event):
        self._x.Value = ""
        self._y.Value = ""
        self._squareNess.Value = ""
        self._range.Value = ""
        self.disableControls()
        self._bouyLED.SetState(3)
        self._detector = False

class BarbedWirePanel(VisionPanel):
    def __init__(self, parent, eventHub, vision, *args, **kwargs):
        VisionPanel.__init__(self, parent, *args, **kwargs)
        self._topX = None
        self._topY = None
        self._topWidth = None
        self._bottomX = None
        self._bottomY = None
        self._bottomWidth = None
        self._detector = False

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
