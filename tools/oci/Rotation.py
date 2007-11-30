# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import wx
from math import radians,sqrt

class Rotation(wx.Panel):
    def __init__(self, parent,label="[Default]"):
        wx.Panel.__init__(self, parent, -1)
        width,height = self.GetSize()
        self.label = label
        
        if label == "Roll":
            self.SetRotation(180)
        elif label == "Pitch":
            self.SetRotation(270)
        else:
            self.rotVal = 0
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)


    def SetRotation(self,val):
        self.rotVal=val
        self.Refresh()
        
    def Rotate(self,degrees=15):
        self.rotVal+=degrees
        self.Refresh()
    
    def OnEraseBackground(self, event):
        pass
    
    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
        
    def Draw(self,gc):
        #TODO: change pen color to correspond to degrees rotated?
        width,height = self.GetSize()
        gc.SetPen(wx.Pen("black", 3))
        gc.SetBrush(wx.Brush("black"))
        """ Keep track of window width/height for centering """
        originalWidth, originalHeight = None, None   
        
        """ Ensure shapes are drawn appropriately
        
        If we find the width/height or height/width ratio is skewed,
        make the algorithm believe that we are dealing with smaller
        constraints so the shapes are not cut off
        
        """
        if (height/float(width) < .58):
            originalWidth = width
            width = height + float(height * .58)
        if (width/float(height) < .58):
            originalHeight = height
            height = width + float(width * .58)  
                
        xCenter = width / 4
        yCenter = height / 4     
        
        """ Use the distance formula to get the length of the sides """
        a = sqrt( (-xCenter-xCenter)**2 + (-yCenter-yCenter)**2 )
        b = sqrt( (xCenter-0)**2 + (-yCenter-yCenter)**2 )

        """ Radius Calculation
        
        Calculate the radius for an Isoceles Triangle: ((a^2)*b)/4*K
        where a and b are two unequal sides and K is the area
        
        """
        radius = (a**2 *b) / (4 * (.5 * a * b))
            
        #Note that for simplicity this triangle has its origin at 0,0 
        trianglePath = gc.CreatePath()
        trianglePath.MoveToPoint(-xCenter, -yCenter)
        trianglePath.AddLineToPoint(xCenter, -yCenter)
        trianglePath.MoveToPoint(xCenter,-yCenter)
        trianglePath.AddLineToPoint(0, radius)
        trianglePath.MoveToPoint(0, radius)
        trianglePath.AddLineToPoint(-xCenter, -yCenter)
        trianglePath.CloseSubpath()
        
        """ Move to the middle """
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

            
        """ Rotate on origin triangle """
        gc.Rotate(radians(self.rotVal))
        gc.DrawPath(trianglePath)
    
        """ Draw the circle that circumscribes the triangle """
        circle = gc.CreatePath()
        circle.AddCircle(0,0,radius)
        gc.StrokePath(circle)
