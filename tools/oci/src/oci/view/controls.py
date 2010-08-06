# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>

# STD Imports
from math import radians,sqrt,pi

# Library Imports
import wx

# Project Imports
import math as pmath
import ext.core as core
import ext.math as math
import ext.vehicle.device as device

import oci.model.subsystem as subsystemMod
import ram.gui.led


class MultiBar(wx.PyControl):
    RED = wx.Colour(255, 0, 0, 100)
    GREEN = wx.Colour(0, 255, 0, 100)

    def __init__(self, parent, innerBorder=15, barType=wx.VERTICAL, colorList=None, 
                 valueList=None, max=100,min=-100, id=wx.ID_ANY, 
                 pos=wx.DefaultPosition, size=wx.DefaultSize, 
                 style=wx.NO_BORDER, 
                 validator=wx.DefaultValidator,name="valbar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style, validator, name)
        if self.GetSize() != wx.DefaultSize:
            self.SetMinSize(self.GetSize())
            
        if colorList is None:
            colorList = []
        if valueList is None:
            valueList = []
        
        self.parent = parent
        # Default to 0%
        self.barValue = 0

        # The max/min values of the bar
        self.maxValue = max
        self.minValue = min
        self.innerBorder = innerBorder 
        self.barType = barType
        self.colorList = colorList
        self.valueList = valueList
        self.multiDirectional = False
        
        # If a value list is defined, grab the first and last element for min/max values
        if len(self.valueList) > 0:
            self.minValue = self.valueList[0]
            self.maxValue = self.valueList[-1]
        
        # Check if the values indicate a mutlidirectional control
        if self.minValue < 0 and self.maxValue > 0:
            self.multiDirectional = True
        
        # If we are going to be working with color coded value range, ensure 
        # the conditions are met 
        if len(colorList) > 0 or len(valueList) > 0:
            assert(len(colorList) == len(valueList) - 1) # This MUST be true for proper functionality

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
    def setVal(self,val):
        if val <= self.maxValue and val >= self.minValue:
            self.barValue=val
            self.Refresh() #Refresh the display
        else:
            # return None if we could not process setVal due to constraints
            return None
        
    SetValue = setVal
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ThrusterBar """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()

        # Draw the rectangle to represent the value
        pen = wx.Pen(MultiBar.GREEN, 1)
        brush = wx.Brush(MultiBar.GREEN) #wx.Brush("green")
        
        # Rotate the graphics context if we're drawing a horizontal box
        if self.barType == wx.HORIZONTAL:  
            gc.Rotate((pmath.pi / 2))
            gc.Translate(0,-width)
            width,height = height,width
        
        if self.multiDirectional:
            # Set the brush according to the sign of the barValue
            if self.barValue > 0:
                gc.SetPen(pen)
                gc.SetBrush(brush)
            else:
                pen.SetColour(MultiBar.RED)
                brush.SetColour(MultiBar.RED)
                gc.SetPen(pen)
                gc.SetBrush(brush)
            # Create a path to represent the percent to be filled
            progressPath = gc.CreatePath()
            percent = float(self.barValue) / (self.maxValue + abs(self.minValue)) 
            heightScale = -height*percent
            innerBorder = (self.innerBorder / 100.0) * width
            progressPath.AddRectangle(0+innerBorder, height/2, width-(innerBorder*2), heightScale) #innerBorder
            gc.DrawPath(progressPath)
            
            pen.SetColour("black")
            pen.SetWidth(2)  
            gc.SetPen(pen)  
            gc.StrokeLine(0,height/2,width,height/2)
            brush.SetStyle(wx.TRANSPARENT)
            gc.SetBrush(brush)
            border = gc.CreatePath()
            border.AddRectangle(0, 0, width, height)
            gc.DrawPath(border)   
        else: # Unidirectional control 
            progressPath = gc.CreatePath()
            percent = float(self.barValue - self.minValue) / \
                float(self.maxValue - self.minValue) 
            heightScale = -height*percent
            for i in xrange(0,len(self.colorList)):
                if self.barValue <= self.valueList[i+1]:
                    brush.SetColour(self.colorList[i])
                    pen.SetColour(self.colorList[i])
                    break
            gc.SetBrush(brush)
            gc.SetPen(pen)
            innerBorder = (self.innerBorder / 100.0) * width
            progressPath.AddRectangle(0+innerBorder, height, width-(innerBorder*2), heightScale) # we've rotated x & y
            gc.DrawPath(progressPath)
            # Draw the containing rectangle
            pen.SetColour("black")
            pen.SetWidth(2)  
            gc.SetPen(pen)  
            border = gc.CreatePath()
            border.AddRectangle(0, 0, width, height)
            gc.StrokePath(border) 
                        
    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event. """
        pass


class DepthBar(wx.PyControl):
    BLACK = wx.Colour(0, 0, 0, 98)
    BLUE = wx.Colour(0, 0, 255, 98)

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,name="valbar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style, validator, name)
        self.parent = parent
        # Default to 0%
        self.barValue = 50 
        self.desiredValue = 20
        # The max/min values of the bar
        
        self.maxValue = 0
        self.minValue = 100
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        #self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
    def setVal(self,val):
        if val >= self.maxValue and val <= self.minValue:
            self.barValue=val
            self.Refresh() #Refresh the display
        else:
            # return None if we could not process setVal due to constraints
            return None 
            
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for MultiBar. """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()
        
        # Draw the rectangle to represent the value
        pen = wx.Pen(DepthBar.BLUE, 1)
        brush = wx.Brush(DepthBar.BLUE) 
            
        percent = float(self.barValue) / (self.maxValue+self.minValue) 
        heightScale = (height*percent) #-?
        
        gc.SetPen(pen)
        gc.SetBrush(brush)
        
        path = gc.CreatePath()
        path.AddRectangle(0, 0, width, heightScale) #innerBorder
        gc.DrawPath(path)
        
        percent = float(self.desiredValue) / (self.maxValue+self.minValue) 
        desiredScale = (height*percent)
        pen.SetColour(DepthBar.BLACK)
        pen.SetWidth(3)
        pen.SetStyle(wx.DOT)
        gc.SetPen(pen)
        gc.StrokeLine(0,desiredScale,width,desiredScale)
                    
             
    #def OnEraseBackground(self, event):
    #    """ Handles the wx.EVT_ERASE_BACKGROUND event. """
    #    pass

class RotationCtrl(wx.Panel):
    ROLL = 1
    PITCH = 2
    YAW = 4
    
    def __init__(self,  parent, label="[Default]", offset=0, direction=1,
                 style = 0):
        """
        @type  offset: float
        @param offest: Radians to offset the given rotation images
        
        @type  direction: float
        @param direction: 1 or -1, change the direction of the images rotation
        """
        wx.Panel.__init__(self, parent, wx.ID_ANY, 
                          style = style | wx.TAB_TRAVERSAL | wx.NO_BORDER)
        
        self.rotVal = 0
        self.desiredRotVal = 0
        self.offset = offset
        self.direction = direction
        self.auxRotations = []
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        #self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

    def setOrientation(self, quat, desiredQuat = math.Quaternion.IDENTITY):
        style = self.GetWindowStyle()
        
        if style & RotationCtrl.ROLL:
            self.rotVal = (quat.getRoll(True)).valueRadians()
            self.desiredRotVal = (desiredQuat.getRoll(True)).valueRadians()
        elif style & RotationCtrl.PITCH:
            self.rotVal = (quat.getPitch(True)).valueRadians()
            self.desiredRotVal = (desiredQuat.getPitch(True)).valueRadians()
        elif style & RotationCtrl.YAW:
            self.rotVal = (quat.getYaw(True)).valueRadians()
            self.desiredRotVal = (desiredQuat.getYaw(True)).valueRadians()

        self.Refresh()

    def setMultipleOrientations(self, orientations):
        assert(len(orientations) >= 2)
        
        # Normal updates
        quat = orientations[0]
        desiredQuat = orientations[1]
        self.setOrientation(quat, desiredQuat)

        # Update the rest
        style = self.GetWindowStyle()
        self.auxRotations = []
        for quat in orientations[2:]:
            if style & RotationCtrl.ROLL:
                self.auxRotations.append((quat.getRoll(True)).valueRadians())
            elif style & RotationCtrl.PITCH:
                self.auxRotations.append((quat.getPitch(True)).valueRadians())
            elif style & RotationCtrl.YAW:
                self.auxRotations.append((quat.getYaw(True)).valueRadians())

        self.Refresh()
    
    #def OnEraseBackground(self, event):
    #    pass
    
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        
        gc.PushState()
        self.draw(gc) 
        gc.PopState()
        
        gc.PushState()
        self.drawText(gc, dc)       
        gc.PopState()
           
    def draw(self,gc):
        # All angles are in radians
        width,height = self.GetSize()
        penSize = 3.0
        gc.SetPen(wx.Pen("black", penSize))
        gc.SetBrush(wx.Brush(wx.Colour(0, 0, 0, 98)))
 
        xCenter = width / 2.0
        yCenter = height / 2.0

        # Take the radius as half of the smaller of either width or height  
        if width <= height:
            radius = width / 2.0
        else:
            radius = height / 2.0
        radius -= penSize
        
        # Find the x,y positions of a 55 degrees angle
        xCoord = pmath.cos(0.959931089) * radius
        yCoord = pmath.sin(0.959931089) * radius
        
        # The 3 points of our triangle
        p1 = wx.Point2D(-xCoord,yCoord)
        p2 = wx.Point2D(xCoord,yCoord)
        p3 = wx.Point2D(0,-radius)
        
        trianglePath = gc.CreatePath()
        trianglePath.MoveToPoint(p1)
        trianglePath.AddLineToPoint(p2)
        trianglePath.AddLineToPoint(p3)
        trianglePath.AddLineToPoint(p1)
        trianglePath.CloseSubpath()
        
        gc.Translate(xCenter,yCenter)
        
        # Draw the circle that circumscribes the triangle
        circle = gc.CreatePath()
        circle.AddCircle(0,0,radius)
        gc.StrokePath(circle)

        # Determin rotation values for triangles (in radians)
        rotVal = (self.rotVal + self.offset) * self.direction
        desiredRotVal = (self.desiredRotVal + self.offset) * self.direction

        # Draw the current triangle        
        self._drawMainTriangle(gc, trianglePath, rotVal)

        # Draw the desired path
        self._drawAuxTriangle(gc = gc, trianglePath = trianglePath,
                              rotVal = desiredRotVal,
                              penColour = wx.Colour(0, 0, 0, 128),
                              brushColour = wx.Colour(0, 255, 0, 90))

        # Draw the extra triangles
        auxRots = [((r + self.offset) * self.direction) \
                   for r in self.auxRotations]
        for rot in auxRots:
            self._drawAuxTriangle(gc = gc, trianglePath = trianglePath,
                                  rotVal = rot,
                                  penColour = wx.Colour(0, 0, 0, 128),
                                  brushColour = wx.Colour(0, 0, 255, 90))

    def _drawMainTriangle(self, gc, trianglePath, rotVal):
        # Rotate into place
        gc.Rotate(rotVal) 

        # Actually do the draw
        gc.DrawPath(trianglePath)

        # Reset rotation to draw the desired rotation triangle
        gc.Rotate(-rotVal)
    
    def _drawAuxTriangle(self, gc, trianglePath, rotVal, penColour,
                         brushColour):
        # Create our path for the triangle
        auxPath = gc.CreatePath()
        auxPath.AddPath(trianglePath)        
        auxPath.CloseSubpath()
        
        # Rotate the path by the desired ammount
        xform = gc.CreateMatrix()
        xform.Rotate(rotVal)
        auxPath.Transform(xform)
        
        # Create pen and brush for the path then draw it
        pen = wx.Pen(penColour, 3)
        gc.SetPen(pen)
        brush = wx.Brush(brushColour)
        gc.SetBrush(brush)
        gc.DrawPath(auxPath)        


    def getTextExtent(self, gc, dc, text):
        if "wxGTK" in wx.PlatformInfo:
            # NYI in Cairo context
            return dc.GetTextExtent(text)
        else:
            return gc.GetTextExtent(text)
        
    def drawText(self, gc, dc):
        """
        Draws the currrent rotation in the middle of the circle
        """
        # Determin center
        width, height = self.GetSize()
        xCenter = width/2.0
        yCenter = height/2.0
        
        # Set Font
        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        #font.SetWeight(wx.BOLD)
        font.SetPointSize(int(float(font.GetPointSize()) * 1.5))
        gc.SetFont(font)
        
        # Draw Background marker
        defaultText = "-000.0"
        textAreaWidth, textAreaHeight = self.getTextExtent(gc, dc, defaultText)
        
        rectHeight = textAreaHeight * 1.25
        rectWidth = textAreaWidth * 1.5
        
        gc.SetPen(wx.Pen("black", 3))
        gc.SetBrush(wx.Brush("white"))
        gc.DrawRectangle(xCenter - rectWidth/2, yCenter - rectHeight/2, 
                         rectWidth, rectHeight)
        
        # Draw text
        rotStr = "% 4.1f" % (self.rotVal * 180/pmath.pi)
        textWidth, textHeight = self.getTextExtent(gc, dc, rotStr)
        leftOffset = textAreaWidth - textWidth
        
        gc.DrawText(rotStr, xCenter - textAreaWidth / 2 + leftOffset, 
                            yCenter - textAreaHeight / 2)
        
class BarDisplay(object):
    """
    Wrapper class which inserts a sequence of controls which represents a 
    numeric value.  It uses both a text box, and a color bar 
    """
    def __init__(self, parent, eventHub, sensor, sizer, lineNum,
                 format = "%5.2f"):
        self._connections = []
        self._format = format

        # Create controls and add them to sizer
        pos = 0
        for control, flags in self._createControls(sensor, parent):
            sizer.Add(control, (lineNum, pos), flag = flags)
            pos += 1
        
        # Subscribe to events
        self._connections.extend(self._subscribeToEvents(sensor, eventHub))
    
    def _createControls(self, sensor, parent, colorList = None, 
                        valueList = None, textReference = "+00.00"):
        """
        Creates the controls and then returns them with sizer info.  This 
        creates the textbox and guage update by the default _update method
        
        @rtype: [(control, flags)]
        @return: A list of tuples containing the control and its sizer flags
        """
        textWidth, textHeight = wx.ClientDC(parent).GetTextExtent(textReference)
        
        label = wx.StaticText(parent, wx.ID_ANY, sensor.getName())

        yellow = wx.Color(255, 255, 0)
        
        if colorList is None:
            colorList = [wx.GREEN, yellow, wx.RED]
        if valueList is None:
            valueList = [0, 50, 55, 65]
            
        self._gauge = MultiBar(parent, colorList = colorList, 
                               size = (textWidth, textHeight),
                               innerBorder = 1,
                               valueList = valueList, barType=wx.HORIZONTAL)
        
        size = (textWidth, wx.DefaultSize.height)
        self._textBox = wx.TextCtrl(parent, size = size)
        
        return [(label,  wx.ALIGN_CENTER_VERTICAL),
                (self._textBox, wx.ALIGN_CENTER_VERTICAL),
                (self._gauge, wx.EXPAND)]
        
    
    def _subscribeToEvents(self, sensor, eventHub):
        """
        Subscribes to events, then returns the list of created connections
        
        @rtype: [ext.core.EventConnection]
        @return: The connections which were created from event subscriptions
        """
        return []
    
    def _updateControls(self, number):
        """
        Sets the text box and guage to represent the same value
        """
        self._textBox.Value = self._format % number
        self._gauge.setVal(number)
    
    def disconnect(self):
        for conn in self._connections:
            conn.disconnect()
        
class TempSensorDisplay(BarDisplay):
    """
    Customizes BarDisplay to display temp data properly and links it the events
    from the ITempSensor.
    """
    def __init__(self, parent, eventHub, tempSensor, sizer, lineNum):
        BarDisplay.__init__(self, parent, eventHub, tempSensor, sizer, lineNum,
                            format = "%2d")
            
    def _createControls(self, tempSensor, parent):
        return BarDisplay._createControls(self, tempSensor, parent,
                                          textReference = '+00')
            
    def _subscribeToEvents(self, sensor, eventHub):
        return [eventHub.subscribe(device.ITempSensor.UPDATE, sensor,
                                   self._update)]
        
    def _update(self, event):
        self._updateControls(event.number)
                    
class ThrusterCurrentDisplay(BarDisplay):
    """
    Customizes the BarDisplay to add an 'Enabled' LED.  It does this by
    subscribing to the proper IThruster events.
    """
    
    def __init__(self, parent, eventHub, thruster, sizer, lineNum):
        BarDisplay.__init__(self, parent, eventHub, thruster, sizer, lineNum,
                            format = '%4.2f')
            
    def _createControls(self, thruster, parent):
        # Define custom color ranges
        valueList = [0, 5, 7, 8]

        # Create base controls
        controls = BarDisplay._createControls(self, thruster, parent, 
                                              colorList = None, 
                                              valueList = valueList,
                                              textReference = '+0.00')
        
        # Update the label on the control to make it more consise
        label, flags = controls[0]
        label.SetLabel(label.GetLabel().replace('Thruster',''))
        label.SetLabel(label.GetLabel().replace('Starboard','Star'))
        
        # Create enabled LED
        self._enableLED = ram.gui.led.LED(parent, state = 3)#, size = size)
        controls.append((self._enableLED, wx.ALIGN_CENTER_VERTICAL))
        
        # Set initial state
        if thruster.isEnabled():
            self._enableLED.SetState(2)
        else:
            self._enableLED.SetState(0)

        return controls
            
    def _subscribeToEvents(self, sensor, eventHub):
        connections = []
        def subscribe(_type, handler):
            conn = eventHub.subscribe(_type, sensor, handler)
            connections.append(conn) 
        
        subscribe(device.ICurrentProvider.UPDATE, self._update)
        subscribe(device.IThruster.ENABLED, 
                  lambda event: self._enableLED.SetState(2))
        subscribe(device.IThruster.DISABLED, 
                  lambda event: self._enableLED.SetState(0))
        
        return connections
        
        
    def _update(self, event):
        self._updateControls(event.number)
                    
class PowerSourceDisplay(BarDisplay):
    """
    A class which customizes the BarDisplay to add 'Enabled' and 'In Use' LEDs
    to each channel displayed.  To do this it subscribes to the proper
    IPowerSource events, and customizes the display for the standard LED class.
    """
    
    VOLTAGE = 1
    CURRENT = 2
    
    def __init__(self, parent, eventHub, powerSource, sizer, lineNum, 
                 mode = VOLTAGE):
        
        # Determine mode
        modes = set([PowerSourceDisplay.VOLTAGE, PowerSourceDisplay.CURRENT])
        if not (mode in modes):
            raise Exception, "Error, invalid mode"    
        self._mode = mode
        
        format = '%5.2f'
        if self._mode == PowerSourceDisplay.CURRENT:
            format = '%4.2f'
        
        # Create entire control
        BarDisplay.__init__(self, parent, eventHub, powerSource, sizer, 
                            lineNum, format = format)
        
    def _createControls(self, powerSource, parent):
        # Define custom color ranges
        yellow = wx.Color(255, 255, 0)
        colorList = []
        valueList = []
        if self._mode == PowerSourceDisplay.VOLTAGE:
            colorList = [wx.RED, yellow, wx.GREEN, yellow, wx.RED]
            valueList = [22, 25.5, 26.5, 29.5, 30, 31]
        elif self._mode == PowerSourceDisplay.CURRENT:
            colorList = [wx.GREEN, yellow, wx.RED]
            valueList = [0, 7, 9, 10]

        # Create base controls
        textReference = '+00.00'
        if self._mode == PowerSourceDisplay.CURRENT:
            textReference = '+0.00'
            
        controls = BarDisplay._createControls(self, powerSource, parent, 
                                              colorList, valueList,
                                              textReference = textReference)
        
        # Create custom LEDs which stand in as "Enabled" or "In Use" indicators
	self._enableLED = ram.gui.led.LED(parent, state = 0)#, size = size)
        self._enableLED.SetAsciiPattern('''
        000000-----000000      
        0000---------0000
        000-----------000
        00-----XXX----=00
        0----XX**XXX-===0
        0---X***XXXXX===0
        ----*000000XX====
        ---X*0XXXXXXXX===
        ---XX0000XXXXX===
        ---XX0XXXXXXXX===
        ----X000000XX====
        0---XXXXXXXXX===0
        0---=XXXXXXX====0
        00=====XXX=====00
        000===========000
        0000=========0000
        000000=====000000
        '''.strip())
        self._enableLED.SetState(0)

        self._inUseLED = ram.gui.led.LED(parent, state = 0)#, size = size)
        self._inUseLED.SetAsciiPattern('''
        000000-----000000      
        0000---------0000
        000-----------000
        00-----XXX----=00
        0----XX**XXX-===0
        0---X*0*XX0XX===0
        ----X*0XXX0XX====
        ---X*X0XXX0XXX===
        ---XXX0XXX0XXX===
        ---XXX0XXX0XXX===
        ----XX00000XX====
        0---XXXXXXXXX===0
        0---=XXXXXXX====0
        00=====XXX=====00
        000===========000
        0000=========0000
        000000=====000000
        '''.strip())
	self._inUseLED.SetState(0)

        controls.extend([(self._enableLED, wx.ALIGN_CENTER_VERTICAL),
                         (self._inUseLED, wx.ALIGN_CENTER_VERTICAL)])
        
        # Set LED state based on current control state
        
        if powerSource.isEnabled():
            self._enableLED.SetState(2)
        else:
            self._enableLED.SetState(0)
            
        if powerSource.inUse():
            self._inUseLED.SetState(2)
        else:
            self._inUseLED.SetState(0)

        return controls
    
    def _subscribeToEvents(self, powerSource, eventHub):
        connections = []
        def subscribe(_type, handler):
            conn = eventHub.subscribe(_type, powerSource, handler)
            connections.append(conn) 
            
        if self._mode == PowerSourceDisplay.VOLTAGE:
            subscribe(device.IVoltageProvider.UPDATE, self._update)
        elif self._mode == PowerSourceDisplay.CURRENT:           
            subscribe(device.ICurrentProvider.UPDATE, self._update)
        
        subscribe(device.IPowerSource.ENABLED, 
                  lambda event: self._enableLED.SetState(2))
        subscribe(device.IPowerSource.DISABLED, 
                  lambda event: self._enableLED.SetState(0))
        subscribe(device.IPowerSource.USING, 
                  lambda event: self._inUseLED.SetState(2))
        subscribe(device.IPowerSource.NOT_USING, 
                  lambda event: self._inUseLED.SetState(0))
        return connections
    
    def _update(self, event):
        self._updateControls(event.number)
