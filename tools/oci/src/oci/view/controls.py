# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>

import wx


class ThrusterBar(wx.PyControl):
    RED = wx.Colour(255, 0, 0, 100)
    GREEN = wx.Colour(0, 255, 0, 100)
    def __init__(self, parent, innerBorder=20, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,name="valbar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style, validator, name)
        self.parent = parent
        """ Default to 0% """
        self.barValue = 50 
        """ The max/min values of the bar """
        
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
            return None 
            """return None if we could not process setVal due to constraints"""
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ThrusterBar. """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()
        """ Draw the rectangle to represent the value """
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
        """ Default to 0% """
        self.barValue = 50 
        self.desiredValue = 20
        """ The max/min values of the bar """
        
        self.maxValue = 0
        self.minValue = 100
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        
    def setVal(self,val):
        if val >= self.maxValue and val <= self.minValue:
            self.barValue=val
            self.Refresh() #Refresh the display
        else:
            print val
            return None 
            """return None if we could not process setVal due to constraints"""
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ThrusterBar. """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()
        """ Draw the rectangle to represent the value """
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
                    
             
    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event. """
        pass