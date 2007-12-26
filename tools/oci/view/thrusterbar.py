# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import wx

class ThrusterBar(wx.PyControl):
    RED = wx.Colour(255, 0, 0, 100)
    GREEN = wx.Colour(0, 255, 0, 100)
    def __init__(self, parent, innerBorder=0,barType=None, id=wx.ID_ANY, pos=wx.DefaultPosition,
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
            #self.Refresh() #Refresh the display
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