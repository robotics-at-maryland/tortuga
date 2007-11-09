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

        self.thruster1 = ThrusterBar.ThrusterBar(self)
        self.thruster2 = ThrusterBar.ThrusterBar(self)
        self.thruster3 = ThrusterBar.ThrusterBar(self)
        self.thruster4 = ThrusterBar.ThrusterBar(self)

        layout =  wx.GridBagSizer(10, 10)
        layout.Add( self.label1, (1,1),flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add( self.label2, (1,2), flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add( self.label3, (1,3) ,flag=wx.ALIGN_CENTER_HORIZONTAL)
        layout.Add( self.label4, (1,4) ,flag=wx.ALIGN_CENTER_HORIZONTAL)

        layout.Add( self.thruster1, (2,1),flag=wx.EXPAND)
        layout.Add( self.thruster2, (2,2), flag=wx.EXPAND)
        layout.Add( self.thruster3, (2,3) ,flag=wx.EXPAND)
        layout.Add( self.thruster4, (2,4) ,flag=wx.EXPAND)

        
        layout.AddGrowableRow(2)
        
        #for col in range(0,layout.GetCols()):
        #print layout.GetCols(),layout.GetRows()
        
        #layout.AddGrowableCol(0)
        layout.AddGrowableCol(1)
        layout.AddGrowableCol(2)
        layout.AddGrowableCol(3)
        layout.AddGrowableCol(4)


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
        
        """ Implement roll, pitch, yaw displays """
        self.roll = Rotation.Rotation(self,"Roll")
        self.pitch = Rotation.Rotation(self,"Pitch")
        self.yaw = Rotation.Rotation(self,"Yaw")


        #self.thruster1.SetMinSize((50,0))
        #self.thruster2.SetMinSize((50,0))
        
        layout = wx.BoxSizer(wx.HORIZONTAL)
        
        layout.Add(self.roll,2,wx.EXPAND)
        layout.Add(self.pitch,2,wx.EXPAND)
        layout.Add(self.yaw,2,wx.EXPAND)

        self.SetSizerAndFit(layout)
