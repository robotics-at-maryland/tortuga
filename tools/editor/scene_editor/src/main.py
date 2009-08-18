import wx
import wx.aui

import editor
import panels

class SceneEditor(wx.Frame):

    ID_OPEN = 0
    ID_SAVE = 1
    ID_EXIT = 2
    ID_GRID = 3
    ID_SIZE = 4
    
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, title = "Scene Editor")

        self.CreateMenuBar()

        # Create the status bar on the bottom for help
        self.CreateStatusBar()

        # Now create the GUI
        self._mgr = wx.aui.AuiManager(self)

        self._infoPanel = panels.InfoPanel(self)
        paneInfo = wx.aui.AuiPaneInfo().Name("Info")
        paneInfo = paneInfo.Caption("Info").Left()
        paneInfo.CloseButton(False)
        self._mgr.AddPane(self._infoPanel, paneInfo)

        # Create the Editor
        #width, height = self.GetSize()
        #width, height = width - 100, height - 100
        self._editor = editor.Editor(self, 10, 10, 1)
        paneInfo = wx.aui.AuiPaneInfo().Name("Editor")
        paneInfo = paneInfo.Caption("Editor").Center()
        paneInfo.CloseButton(False)
        self._mgr.AddPane(self._editor, paneInfo)

        self._objectPanel = panels.ObjectPanel(self)
        paneInfo = wx.aui.AuiPaneInfo().Name("Object Info")
        paneInfo = paneInfo.Caption("Object Info").Bottom()
        paneInfo.CloseButton(False)
        self._mgr.AddPane(self._objectPanel, paneInfo)

        self._mgr.Update()

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouse)

        self.Show(True)

    def CreateMenuBar(self):
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

        gridMenu = wx.Menu()

        gridOnMenu = wx.MenuItem(gridMenu, id = SceneEditor.ID_GRID,
                                 text = "&Grid",
                                 help = "Turn on/off the grid.",
                                 kind = wx.ITEM_CHECK)
        wx.EVT_MENU(self, SceneEditor.ID_GRID, self.OnGrid)
        gridMenu.AppendItem(gridOnMenu)
        gridOnMenu.Check()
        
        gridMenu.Append(SceneEditor.ID_SIZE, text = "&Size",
                        help = "Change the grid size.")
        wx.EVT_MENU(self, SceneEditor.ID_SIZE, self.OnGridSize)

        menuBar.Append(gridMenu, "&Graph")

        self.SetMenuBar(menuBar)

    def OnMouse(self, event):
        self.Refresh()

    def OnSize(self, event):
        #width, height = self.GetSize()
        #width -= 100
        #self._editor.Resize((width, height))
        self.Refresh()

    def OnOpen(self, event):
        dlg = wx.MessageDialog(self, message = "Are you sure you want to open"
                               "a new file? All changes since the last save"
                               " will be lost.", caption =  "Confirmation",
                               style = wx.YES_NO | wx.CANCEL | wx.STAY_ON_TOP)
        var = dlg.ShowModal()
        if var == wx.ID_YES:
            dlg = wx.FileDialog(self, style = wx.FD_OPEN)
            var = dlg.ShowModal()
            self._editor.LoadSceneFile(dlg.GetPath())

    def OnSave(self, event):
        print "Not implemented yet."

    def OnGrid(self, event):
        if event.IsChecked():
            self._editor.EnableGrid()
        else:
            self._editor.DisableGrid()
        self.Refresh()

    def OnGridSize(self, event):
        print "Not implemented yet."

    def OnExit(self, event):
        self.Destroy()

app = wx.PySimpleApp()
frame = SceneEditor(None)
app.MainLoop()
