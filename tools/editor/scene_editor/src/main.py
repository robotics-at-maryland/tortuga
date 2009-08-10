import wx
import editor

class SceneEditor(wx.Frame):

    ID_OPEN = 0
    ID_SAVE = 1
    ID_EXIT = 2
    
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, title = "Scene Editor")

        menuBar = wx.MenuBar()

        # Create the file menu, open/save/close operations
        fileMenu = wx.Menu()

        # Open menu
        fileMenu.Append(SceneEditor.ID_OPEN, text = "&Open",
                        help = "Open a scene file.")
        wx.EVT_MENU(self, SceneEditor.ID_OPEN, self.OnOpen)

        # Save menu
        fileMenu.Append(SceneEditor.ID_SAVE, text = "&Save",
                        help = "Save the current scene file.")
        wx.EVT_MENU(self, SceneEditor.ID_SAVE, self.OnSave)

        # Exit menu
        fileMenu.Append(SceneEditor.ID_EXIT, text = "E&xit",
                        help = "Quit the program.")
        wx.EVT_MENU(self, SceneEditor.ID_EXIT, self.OnExit)

        menuBar.Append(fileMenu, "&File")

        self.SetMenuBar(menuBar)

        # Create the status bar on the bottom for help
        self.CreateStatusBar()

        # Now create the GUI
        #layout = wx.GridBagSizer(5, 5)

        # Create the Editor
        width, height = self.GetSize()
        self._editor = editor.Editor(self, 10, 10, 1, size = (width, height))

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouse)

        self.Show(True)

    def OnMouse(self, event):
        self.Refresh()

    def OnSize(self, event):
        self._editor.Resize(self.GetSize())
        self.Refresh()

    def OnOpen(self, event):
        print "Not implemented yet."
        #dlg = wx.MessageDialog(self, message = "Are you sure you want to open"
        #                       "a new file? All changes since the last save"
        #                       " will be lost.", caption =  "Confirmation",
        #                       style = wx.YES_NO | wx.CANCEL | wx.STAY_ON_TOP)
        #var = dlg.ShowModal()
        #print var

    def OnSave(self, event):
        print "Not implemented yet."

    def OnExit(self, event):
        self.Destroy()

app = wx.PySimpleApp()
frame = SceneEditor(None)
app.MainLoop()
