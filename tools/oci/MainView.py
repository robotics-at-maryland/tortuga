# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Jon Speiser <jspeiser@umd.edu>
# All rights reserved.
#
# Author: Jon Speiser <jspeiser@umd.edu>
import wx
import wx.aui
import DisplayPanels
from Thruster import Thruster
import thread,time
#----------------------------------------------------------------------

class ParentFrame(wx.aui.AuiMDIParentFrame):
    def __init__(self, parent):
        wx.aui.AuiMDIParentFrame.__init__(self, parent, -1,
                                          title="UMD Robotics",
                                          size=(640,480),
                                          style=wx.DEFAULT_FRAME_STYLE)
        
        mb = self.MakeMenuBar()
        self.SetMenuBar(mb)
        self.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME))
        self.CreateStatusBar()
        
        self.thrustChild = ChildFrame(self, "Thrust")    
        self.rotChild = ChildFrame(self, "Rotation")

        self.Bind(wx.EVT_CLOSE, self.onCloseWindow)        

    
    def MakeMenuBar(self):
        mb = wx.MenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "Thrusters\tCtrl-T")
        self.Bind(wx.EVT_MENU, self.OnOpenThrust, item)
        item = menu.Append(-1, "Rotation\tCtrl-R")
        self.Bind(wx.EVT_MENU, self.OnOpenRot, item)
        item = menu.Append(-1, "Exit\tCtrl-Q")
        self.Bind(wx.EVT_MENU, self.OnDoClose, item)
        mb.Append(menu, "&File")
        return mb
    
    # --- Events ---
    def onCloseWindow(self, event):
        """ Close the AUI MDI window and the dummy window """
        self.Destroy()
        self.GetParent().Destroy()
        
    def OnOpenThrust(self, evt):
        if self.thrustChild == None:
            self.thrustChild = ChildFrame(self, "Thrust")
        self.thrustChild.Show()
        
    def OnOpenRot(self, evt):
        #rotChild = ChildFrame(self, "Rotation")
        if self.rotChild == None:
            self.rotChild = ChildFrame(self, "Rotation")
        self.rotChild.Show()

    def OnDoClose(self, evt):
        self.Close()
        
#----------------------------------------------------------------------

class ChildFrame(wx.aui.AuiMDIChildFrame):
    def __init__(self, parent, name):
       
        wx.aui.AuiMDIChildFrame.__init__(self, parent, -1,name)
        mb = parent.MakeMenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "Menu for " + name)
        mb.Append(menu, "&Child")
        self.SetMenuBar(mb)
        self.name=name
        self.parent = parent
        
        if name == "Rotation":
            self.panel = DisplayPanels.RotationPanel(self)
            self.panel.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME)) 
        else:
            thrusterList= []
            for i in xrange(1,7):
                thrusterList.append(Thruster(nameIn="Thruster "+str(i)))
            self.panel = DisplayPanels.ThrusterPanel(self,thrusterList)
            self.panel.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME))   
        
        sizer = wx.BoxSizer()
        sizer.Add(self.panel, 1, wx.EXPAND)
        self.SetSizer(sizer)
        
        wx.CallAfter(self.Layout)
        
    def __del__(self):
        if self.name == "Thrust":
            self.parent.thrustChild = None
        else:
            self.parent.rotChild = None
    
        
#----------------------------------------------------------------------
""" Dummy window launch aui """
class MainWindow(wx.Frame):
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, wx.ID_ANY, title, wx.DefaultPosition)
        self.pf = ParentFrame(self)
        self.pf.Show()
        
class MyApp(wx.App):
    def OnInit(self):
        frame = MainWindow(None, -1, 'Thruster Levels')
        frame.Show(False)
        return True

app = MyApp(0)
app.MainLoop()


