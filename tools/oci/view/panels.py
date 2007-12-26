# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import wx
"""Vehicle Sensors/Thrusters"""
from Thruster import Thruster
from Orientation import Orientation
from Depth import Depth


"""GUI Controls"""
from Rotation import RotationCtrl as RotationCtrl
from ThrusterBar import ThrusterBar
from DepthBar import DepthBar

from math import cos, sin, radians
import thread,time

class ThrusterPanel(wx.Panel):
    def __init__(self, parent,listIn, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.isOpen = True
        self.thrusterList = listIn
        self.Bind(wx.EVT_CLOSE, self.onClose)
        """Implement Thrusters Dynamically """
        layout =  wx.GridBagSizer(10, 10)
        pos = 0
        for item in self.thrusterList:
            label = wx.StaticText(self,-1,item.getName())
            bar = ThrusterBar(self,20)
            layout.Add(label, (0,pos),flag=wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(bar, (1,pos),flag=wx.EXPAND)
            layout.AddGrowableCol(pos)
            pos+=1

            # Subscribe
            item.subscribe(item.PYFORCEUPDATE, self._makeHandler(bar))
            
        layout.AddGrowableRow(1)
        self.SetSizerAndFit(layout)
        self.generateUpdates()

    def _makeHandler(self, bar):
        # Generate event handle
        def handler(event):
            forceVal = event.force
            tmp = int(forceVal*100) # For testing purposes only
            bar.setVal(tmp)
        return handler

    """Deconstructor ends the threads """
    def onClose(self, event):
        self.isOpen = False  
        
    """Subscribe GUI to thruster events """
    def generateUpdates(self):
        for thruster in self.thrusterList:
            thread.start_new_thread(self.updateLoop, (thruster,))
            
    def updateLoop(self,thruster):
        try:
            while self.isOpen:
                thruster.update()
                self.Refresh()
                time.sleep(1)  
        except wx.PyDeadObjectError,e:
            pass # Panel was closed & threads terminated, do nothing

class DepthPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.isOpen = True
        self.Bind(wx.EVT_CLOSE, self.onClose)
        layout =  wx.GridBagSizer(10, 10)
        
        depth = Depth(nameIn="Depth")

        label = wx.StaticText(self,-1,depth.getName())    
        dbar = DepthBar(self)
    
        layout.Add(label, (0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(dbar, (1,0),flag=wx.EXPAND)
        layout.AddGrowableCol(0)

        depth.subscribe(depth.PYFORCEUPDATE, self._makeHandler(dbar))
            
        layout.AddGrowableRow(1)
        self.SetSizerAndFit(layout)
        self.SetSizeHints(0,0,100,-1)
        self.generateUpdates(depth)

    def _makeHandler(self, dbar):
        def handler(event):
            dVal = event.depth
            dbar.setVal(dVal)
        return handler

    """Deconstructor ends the threads """
    def onClose(self, event):
        self.isOpen = False  
        
    """Subscribe GUI to thruster events """
    def generateUpdates(self,depth):
        thread.start_new_thread(self.updateLoop, (depth,))
            
    def updateLoop(self,depth):
        try:
            while self.isOpen:
                depth.update()
                self.Refresh()
                time.sleep(1)  
        except wx.PyDeadObjectError,e:
            pass # Panel was closed & threads terminated, do nothing

class RotationPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        """Create the Control Panel"""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.isOpen = True
        self.Bind(wx.EVT_CLOSE,self.onClose)
        
        r = Orientation(nameIn="Roll")
        p = Orientation(nameIn="Pitch")
        y = Orientation(nameIn="Yaw")
        self.rotationList = [r,p,y]
        
        """Layout the controls"""
        layout = wx.GridBagSizer(0,0)

        pos = 0
        for item in self.rotationList:
            label = wx.StaticText(self,-1,item.getName())
            bar = RotationCtrl(self,item.getName())
            layout.Add(label, (0,pos),flag=wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(bar, (1,pos),flag=wx.EXPAND)
            layout.AddGrowableCol(pos)
            pos+=1
            #Subscribe
            item.subscribe(item.PYORIENTATIONUPDATE, self._makeHandler(bar))

        layout.AddGrowableRow(1) 
        self.SetSizerAndFit(layout)       
        self.generateUpdates()
        
    def onClose(self,event):
        self.isOpen = False
        
    def _makeHandler(self, rotationCtrl):
        # Generate event handler
        def handler(event):
            quat = event.quat
            rotationCtrl.setOrientation(quat)
        return handler
    
    """Have GUI generate thruster events """
    def generateUpdates(self):
        for rotCtrl in self.rotationList:
            thread.start_new_thread(self.updateLoop, (rotCtrl,))
            
    def updateLoop(self,rotCtrl):
        try:
            while self.isOpen:
                rotCtrl.update()
                self.Refresh()
                time.sleep(1)  
        except wx.PyDeadObjectError,e:
            pass # Panel was closed & threads terminated, do nothing

    