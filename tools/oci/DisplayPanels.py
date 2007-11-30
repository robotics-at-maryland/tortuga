# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import wx,Rotation, ThrusterBar
from Thruster import Thruster
from math import cos, sin, radians
import thread,time

class ThrusterPanel(wx.Panel):
    def __init__(self, parent,listIn, *args, **kwargs):
        """Create the Control Panel."""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        self.isOpen = True
        self.thrusterList = listIn
        """ Implement Thrusters Dynamically """

        layout =  wx.GridBagSizer(10, 10)
        pos = 0
        for item in self.thrusterList:
            item.label = wx.StaticText(self,-1,item.getName())
            item.bar = ThrusterBar.ThrusterBar(self,20)
            layout.Add(item.label, (0,pos),flag=wx.ALIGN_CENTER_HORIZONTAL)
            layout.Add(item.bar, (1,pos),flag=wx.EXPAND)
            layout.AddGrowableCol(pos)
            pos+=1

        layout.AddGrowableRow(1)

        self.SetSizerAndFit(layout)
        self.subscribe()

    """ Deconstructor ends the threads """
    def __del__(self):
        self.isOpen = False  
        
    """ Subscribe GUI to thruster events """
    def subscribe(self):
        for thruster in self.thrusterList:
            thruster.subscribe(thruster.PYFORCEUPDATE, thruster.handlerForceUpdate)        
            thread.start_new_thread(self.updateLoop, (thruster,))
            
    def updateLoop(self,t):
        try:
            while self.isOpen:
                t.update()
                self.Refresh()
                time.sleep(1)  
        except Exception,e:
            #Panel was closed
            print "Closed:" , e.message
    

class RotationPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        """Create the Control Panel."""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        
        """ mplement roll, pitch, yaw displays"""
        self.roll = Rotation.Rotation(self,"Roll")
        self.pitch = Rotation.Rotation(self,"Pitch")
        self.yaw = Rotation.Rotation(self,"Yaw")

        """Implement labels"""
        self.label1 = wx.StaticText(self,-1,"Roll")
        self.label2 = wx.StaticText(self,-1,"Pitch")
        self.label3 = wx.StaticText(self,-1,"Yaw")

        #self.thruster1.SetMinSize((50,0))
        #self.thruster2.SetMinSize((50,0))
        
        layout = wx.GridBagSizer(0,0)
        layout.Add(self.label1, (0,0),flag=wx.ALIGN_CENTER_HORIZONTAL | wx.ALIGN_CENTER_VERTICAL)
        layout.Add(self.label2, (0,1), flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self.label3, (0,2) ,flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self.roll, (1,0), flag=wx.EXPAND)
        layout.Add(self.pitch, (1,1), flag=wx.EXPAND)
        layout.Add(self.yaw, (1,2) ,flag=wx.EXPAND)
        
        layout.AddGrowableRow(1)
        
        layout.AddGrowableCol(0)
        layout.AddGrowableCol(1)
        layout.AddGrowableCol(2)
        self.SetSizerAndFit(layout)
