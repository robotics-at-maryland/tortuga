
import wx
import wx.aui
import DisplayPanels
#----------------------------------------------------------------------


class ParentFrame(wx.aui.AuiMDIParentFrame):
    def __init__(self, parent):
        wx.aui.AuiMDIParentFrame.__init__(self, parent, -1,
                                          title="UMD Robotics",
                                          size=(640,480),
                                          style=wx.DEFAULT_FRAME_STYLE)
        self.OnOpenRot(None)
        self.OnOpenThrust(None)
        mb = self.MakeMenuBar()
        self.SetMenuBar(mb)
        self.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME))
        self.CreateStatusBar()

    def MakeMenuBar(self):
        mb = wx.MenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "Thrusters\tCtrl-T")
        self.Bind(wx.EVT_MENU, self.OnOpenThrust, item)
        item = menu.Append(-1, "Rotation\tCtrl-R")
        self.Bind(wx.EVT_MENU, self.OnOpenRot, item)
        item = menu.Append(-1, "Close parent")
        self.Bind(wx.EVT_MENU, self.OnDoClose, item)
        mb.Append(menu, "&File")
        return mb
        
    def OnOpenThrust(self, evt):
        thrustChild = ChildFrame(self, "Thrust")
        thrustChild.Show()
        
        
    def OnOpenRot(self, evt):
        rotChild = ChildFrame(self, "Rotation")
        rotChild.Show()

    def OnDoClose(self, evt):
        self.Close()
        self.Parent.Close()
        

#----------------------------------------------------------------------

class ChildFrame(wx.aui.AuiMDIChildFrame):
    def __init__(self, parent, name):
       
            
        wx.aui.AuiMDIChildFrame.__init__(self, parent, -1,name)
        mb = parent.MakeMenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "Menu for " + name)
        mb.Append(menu, "&Child")
        self.SetMenuBar(mb)
        
        if name == "Rotation":
            p = DisplayPanels.RotationPanel(self)
            p.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME)) 
            
        else:
            p = DisplayPanels.ThrusterPanel(self)
            p.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWFRAME))

            

        
        sizer = wx.BoxSizer()
        sizer.Add(p, 1, wx.EXPAND)
        self.SetSizer(sizer)
        
        wx.CallAfter(self.Layout)
        
        
#----------------------------------------------------------------------
""" Dummy window launch aui """
class MainWindow(wx.Frame):
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, wx.ID_ANY, title, wx.DefaultPosition)
        pf = ParentFrame(self)
        pf.Show()
        
        
class MyApp(wx.App):
    def OnInit(self):
        frame = MainWindow(None, -1, 'Thruster Levels')
        frame.Show(False)
        return True

app = MyApp(0)
app.MainLoop()


