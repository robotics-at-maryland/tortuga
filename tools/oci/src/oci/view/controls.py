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
import ext.core as core
import ext.math as math

class ThrusterBar(wx.PyControl):
    RED = wx.Colour(255, 0, 0, 100)
    GREEN = wx.Colour(0, 255, 0, 100)
    def __init__(self, parent, innerBorder=20, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,name="valbar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style, validator, name)
        self.parent = parent
        # Default to 0%
        self.barValue = 50 
        # The max/min values of the bar
        
        self.maxValue = 100
        self.minValue = -100
        self.innerBorder=innerBorder 
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
    def setVal(self,val):
        if val <= self.maxValue and val >= self.minValue:
            self.barValue=val
            self.Refresh() #Refresh the display
        else:
            # return None if we could not process setVal due to constraints
            return None 
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ThrusterBar """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()
        
        # Draw the rectangle to represent the value
        pen = wx.Pen(ThrusterBar.GREEN, 1)
        brush = wx.Brush(ThrusterBar.GREEN) #wx.Brush("green")
    
        if self.barValue > 0:
            gc.SetPen(pen)
            gc.SetBrush(brush)
        else:
            pen.SetColour(ThrusterBar.RED)
            brush.SetColour(ThrusterBar.RED)
            gc.SetPen(pen)
            gc.SetBrush(brush)
            
        path = gc.CreatePath()
        percent = float(self.barValue) / (self.maxValue*2) 
        heightScale = -height*percent
        path.AddRectangle(0+self.innerBorder, height/2, width-(self.innerBorder*2), heightScale) #innerBorder
        gc.DrawPath(path)
        
        pen.SetColour("black")
        pen.SetWidth(2)  
        gc.SetPen(pen)  
        gc.StrokeLine(0,height/2,width,height/2)
        brush.SetStyle(wx.TRANSPARENT)
        gc.SetBrush(brush)
        border = gc.CreatePath()
        border.AddRectangle(0, 0, width, height)
        gc.DrawPath(border)      
             
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
            print val
            # return None if we could not process setVal due to constraints
            return None 
            
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ThrusterBar. """
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
    
    def __init__(self,  parent, label="[Default]", style = 0):
        # Remove our styles from the style
        wx.Panel.__init__(self, parent, wx.ID_ANY, 
                          style = style | wx.TAB_TRAVERSAL | wx.NO_BORDER)
        
        self.rotVal = 0
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        #self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

    def setOrientation(self,quat):
        style = self.GetWindowStyle()
        if style & RotationCtrl.ROLL:
            self.rotVal = (quat.getRoll(True)).valueRadians()
        elif style & RotationCtrl.PITCH:
            self.rotVal = (quat.getPitch(True)).valueRadians()
        elif style & RotationCtrl.YAW:
            self.rotVal = (quat.getYaw(True)).valueRadians()
        self.Refresh()
    
    #def OnEraseBackground(self, event):
    #    pass
    
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.draw(gc)        
           
    def draw(self,gc):
        # TODO: change pen color to correspond to degrees rotated?
        # TODO: clean up the code!
        width,height = self.GetSize()
        gc.SetPen(wx.Pen("black", 3))
        gc.SetBrush(wx.Brush(wx.Colour(0, 0, 0, 98)))
        # Keep track of window width/height for centering
        originalWidth, originalHeight = None, None   
        
        # Ensure shapes are drawn appropriately:
        # If we find the width/height or height/width ratio is skewed,
        # make the algorithm believe that we are dealing with smaller
        # constraints so the shapes are not cut off
        
        if (height/float(width) < .58):
            originalWidth = width
            width = height + float(height * .58)
        if (width/float(height) < .58):
            originalHeight = height
            height = width + float(width * .58)  
        xCenter = width / 4
        yCenter = height / 4     
        # Use the distance formula to get the length of the sides
        a = sqrt( (-xCenter-xCenter)**2 + (-yCenter-yCenter)**2 )
        b = sqrt( (xCenter-0)**2 + (-yCenter-yCenter)**2 )
        
        # Radius Calculation:
        # Calculate the radius for an Isoceles Triangle: ((a^2)*b)/4*K
        # where a and b are two sides and K is the area
        
        radius = (a**2 *b) / (4 * (.5 * a * b)) 
        #Note that for simplicity this triangle has its origin at 0,0 
        #Rounding
        xCenter = int(xCenter+.5)
        yCenter = int(yCenter+.5)
        radius = int(radius+.5)
        
        trianglePath = gc.CreatePath()
        trianglePath.MoveToPoint(-xCenter, -yCenter)
        trianglePath.AddLineToPoint(xCenter, -yCenter)
        #trianglePath.MoveToPoint(xCenter,-yCenter)
        trianglePath.AddLineToPoint(0, radius)
        #trianglePath.MoveToPoint(0, radius)
        trianglePath.AddLineToPoint(-xCenter, -yCenter)
        trianglePath.CloseSubpath()
        
        directionCircle = gc.CreatePath()
        directionCircle.AddCircle(0,radius,3)
        directionCircle.CloseSubpath()

        # Move to the middle
        #if originalWidth <> None:    
        #    gc.Translate(originalWidth/2,yCenter*2)
        #elif originalHeight <> None:
        #    gc.Translate(xCenter*2, originalHeight/2)
        #else:
        #    gc.Translate(xCenter*2,yCenter*2)
        if originalWidth <> None: 
            gc.Translate(originalWidth/2,radius+5)
        else:
            gc.Translate(xCenter*2,radius+5)
        # Rotate on origin triangle
        gc.Rotate(self.rotVal) #Assumes radians!
        gc.DrawPath(trianglePath)
         
        # Draw the circle that circumscribes the triangle
        circle = gc.CreatePath()
        circle.AddCircle(0,0,radius)
        gc.StrokePath(circle) 
         
        # Draw the direction indicator
        gc.SetPen(wx.Pen(wx.Colour(0, 0, 255, 200),3))
        gc.DrawPath(directionCircle)

        
        # Create a copy of the triangle path to display desired orientation
        # work on rotating independently
        desiredPath = gc.CreatePath()
        #linepts = [(-xCenter,-yCenter),(xCenter,-yCenter),(0, radius+20),(-xCenter, -yCenter)]
        #print self.label, linepts
        desiredPath.AddPath(trianglePath)
        desiredPath.AddPath(directionCircle)
        desiredPath.CloseSubpath()
        xform = gc.CreateMatrix()
        xform.Rotate(-self.rotVal)
        desiredPath.Transform(xform)
        
        pen = wx.Pen(wx.Colour(0, 0, 0, 128),3)
        
        #pen.SetWidth(4)
        #pen.SetStyle(wx.DOT)
        gc.SetPen(pen)
        brush = wx.Brush(wx.Colour(0, 255, 0, 90))
        gc.SetBrush(brush)
        gc.DrawPath(desiredPath)
        #gc.StrokePath(desiredPath)
        #gc.FillPath(desiredPath)
