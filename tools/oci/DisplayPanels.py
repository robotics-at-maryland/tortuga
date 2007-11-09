import wx,Rotation, ThrusterBar
from math import cos, sin, radians
class ThrusterPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        """Create the Control Panel."""
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.parent = parent
        
        """ Implement two Thrusters """
        
        self.thruster1 = ThrusterBar.ThrusterBar(self)
        self.thruster2 = ThrusterBar.ThrusterBar(self)
        self.thruster3 = ThrusterBar.ThrusterBar(self)
        self.thruster4 = ThrusterBar.ThrusterBar(self)

        layout = wx.BoxSizer(wx.HORIZONTAL)
        
        layout.Add(self.thruster1, 1, wx.EXPAND | wx.ALL,10)       
        layout.Add(self.thruster2, 1, wx.EXPAND | wx.ALL,10)
        layout.Add(self.thruster3, 1, wx.EXPAND | wx.ALL,10)       
        layout.Add(self.thruster4, 1, wx.EXPAND | wx.ALL,10)       

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
