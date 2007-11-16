import wx,Rotation, ThrusterBar
from math import cos, sin, radians
class ThrusterPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        """Create the Control Panel."""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        
        """ Implement two Thrusters """
        self.label1 = wx.StaticText(self,-1,"Thruster 1")
        self.label2 = wx.StaticText(self,-1,"Thruster 2")
        self.label3 = wx.StaticText(self,-1,"Thruster 3")
        self.label4 = wx.StaticText(self,-1,"Thruster 4")

        self.thruster1 = ThrusterBar.ThrusterBar(self,20)
        self.thruster2 = ThrusterBar.ThrusterBar(self)
        self.thruster3 = ThrusterBar.ThrusterBar(self,20)
        self.thruster4 = ThrusterBar.ThrusterBar(self)

        layout =  wx.GridBagSizer(10, 10)
        layout.Add(self.label1, (0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self.label2, (0,1), flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self.label3, (0,2) ,flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add(self.label4, (0,3) ,flag=wx.ALIGN_CENTER_HORIZONTAL)

        layout.Add(self.thruster1, (1,0),flag=wx.EXPAND)
        layout.Add(self.thruster2, (1,1), flag=wx.EXPAND)
        layout.Add(self.thruster3, (1,2) ,flag=wx.EXPAND)
        layout.Add(self.thruster4, (1,3) ,flag=wx.EXPAND)
        
        layout.AddGrowableRow(1)
        #for col in range(0,layout.GetCols()): #<-- doesnt seem to work until after SetSizerAndFit is called
        #print layout.GetCols(),layout.GetRows()
        layout.AddGrowableCol(0)
        layout.AddGrowableCol(1)
        layout.AddGrowableCol(2)
        layout.AddGrowableCol(3)

        #layout.Add(self.thruster1, wx.EXPAND | wx.ALL,10)       
        #layout.Add(self.thruster2, wx.EXPAND | wx.ALL,10)
        #layout.Add(self.thruster3, wx.EXPAND | wx.ALL,10)       
        #layout.Add(self.thruster4, wx.EXPAND | wx.ALL,10)       

        self.SetSizerAndFit(layout)


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
        
        #layout.Add(self.roll,2,wx.EXPAND)
        #layout.Add(self.pitch,2,wx.EXPAND)
        #layout.Add(self.yaw,2,wx.EXPAND)
        
        layout.AddGrowableRow(1)
        
        layout.AddGrowableCol(0)
        layout.AddGrowableCol(1)
        layout.AddGrowableCol(2)
        self.SetSizerAndFit(layout)
