#Boa:Frame:MainFrame

# Copyright 2004 Roman Yakovenko.
# 2007 Alexander Eisenhuth
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


""" Contains the iplementation of the main frame of the application.
Most of the code was build with BOA"""

import wx

def create(parent):
    return MainFrame(parent)

[wxID_MAINFRAMEMENUEFILEEXIT, wxID_MAINFRAMEMENUEFILENEW, 
 wxID_MAINFRAMEMENUEFILEOPEN, wxID_MAINFRAMEMENUEFILERECENT, 
 wxID_MAINFRAMEMENUEFILESAVE, 
] = [wx.NewId() for _init_coll_menueFile_Items in range(5)]

[wxID_MAINFRAMEMENUINCLUDESADDINC, wxID_MAINFRAMEMENUINCLUDESITEMS1, 
] = [wx.NewId() for _init_coll_menuIncludes_Items in range(2)]

[wxID_MAINFRAMEMENUDEFINESADDDEF, wxID_MAINFRAMEMENUDEFINESREMOVEDEF, 
] = [wx.NewId() for _init_coll_menuDefines_Items in range(2)]

[wxID_MAINFRAME, wxID_MAINFRAMEBUTGCCXML, wxID_MAINFRAMEBUTGENCPP, 
 wxID_MAINFRAMEBUTGENPYPP, wxID_MAINFRAMEBUTGENXML, wxID_MAINFRAMEBUTHEADERS, 
 wxID_MAINFRAMELISTDEFINES, wxID_MAINFRAMELISTINCLUDES, 
 wxID_MAINFRAMENBSETTINGS, wxID_MAINFRAMENOTEBOOK1, 
 wxID_MAINFRAMEPANELBUTTONS, wxID_MAINFRAMEPANELCODE, wxID_MAINFRAMEPANELMAIN, 
 wxID_MAINFRAMEPANELNBSETTINGS, wxID_MAINFRAMEPANELSETTINGS, 
 wxID_MAINFRAMEPANELSHLOW, wxID_MAINFRAMEPANELSHUP, 
 wxID_MAINFRAMESPLITTERHORIZONTAL, wxID_MAINFRAMESPLITTERVERTICAL, 
 wxID_MAINFRAMESTATICTEXT1, wxID_MAINFRAMESTATICTEXT2, 
 wxID_MAINFRAMESTATICTEXT3, wxID_MAINFRAMESTATICTEXT4, 
 wxID_MAINFRAMESTATICTEXT5, wxID_MAINFRAMESTATUSBAR, wxID_MAINFRAMETEXTCTRL2, 
 wxID_MAINFRAMETEXTGCCXML, wxID_MAINFRAMETEXTHEADER, wxID_MAINFRAMETEXTOUTPUT, 
] = [wx.NewId() for _init_ctrls in range(29)]

class MainFrame(wx.Frame):
    """ Main frame class. Part of MVC """

    def _init_coll_bsGccXml_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText2, 0, border=5,
              flag=wx.ALIGN_CENTER_VERTICAL | wx.LEFT)
        parent.AddWindow(self.textGccXml, 1, border=10,
              flag=wx.ALIGN_CENTER_VERTICAL | wx.TOP | wx.BOTTOM)
        parent.AddWindow(self.butGccXml, 0, border=10,
              flag=wx.LEFT | wx.RIGHT | wx.ALIGN_CENTER_VERTICAL)

    def _init_coll_bsMURCompile_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.textCtrl2, 1, border=0, flag=wx.EXPAND)

    def _init_coll_bsMain_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.splitterHorizontal, 1, border=5,
              flag=wx.ALL | wx.EXPAND)

    def _init_coll_bsIncPath_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText3, 0, border=5,
              flag=wx.ALIGN_CENTER_VERTICAL | wx.LEFT)
        parent.AddWindow(self.listIncludes, 1, border=10,
              flag=wx.RIGHT | wx.BOTTOM | wx.TOP | wx.EXPAND | wx.ALIGN_CENTER_VERTICAL)

    def _init_coll_bsHeader_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText1, 0, border=5,
              flag=wx.LEFT | wx.ALIGN_CENTER_VERTICAL)
        parent.AddWindow(self.textHeader, 1, border=10,
              flag=wx.ALIGN_CENTER_VERTICAL | wx.BOTTOM | wx.TOP)
        parent.AddWindow(self.butHeaders, 0, border=10,
              flag=wx.LEFT | wx.RIGHT | wx.ALIGN_CENTER_VERTICAL)

    def _init_coll_bsUpLeft_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.nbSettings, 1, border=0, flag=wx.ALL | wx.EXPAND)

    def _init_coll_bsMURButtons_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.butGenXml, 0, border=10,
              flag=wx.RIGHT | wx.LEFT | wx.ALIGN_CENTER_VERTICAL)
        parent.AddWindow(self.butGenCpp, 0, border=10,
              flag=wx.LEFT | wx.RIGHT | wx.ALIGN_CENTER | wx.ALIGN_CENTER_VERTICAL)
        parent.AddWindow(self.butGenPyPP, 0, border=10,
              flag=wx.RIGHT | wx.LEFT | wx.ALIGN_RIGHT | wx.ALIGN_CENTER_VERTICAL)

    def _init_coll_bsMainUpper_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.splitterVertical, 1, border=5,
              flag=wx.BOTTOM | wx.EXPAND)

    def _init_coll_bsMainLow_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.notebook1, 1, border=0, flag=wx.EXPAND)

    def _init_coll_bsUpRight_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText5, 0, border=0, flag=0)
        parent.AddSizer(self.bsMURCompile, 1, border=4, flag=wx.TOP | wx.EXPAND)
        parent.AddWindow(self.panelButtons, 0, border=10,
              flag=wx.ALIGN_CENTER_HORIZONTAL | wx.TOP)

    def _init_coll_bsMULSettings_Items(self, parent):
        # generated method, don't edit

        parent.AddSpacer((10, 10), border=0, flag=0)
        parent.AddSizer(self.bsHeader, 0, border=5, flag=wx.RIGHT | wx.EXPAND)
        parent.AddSizer(self.bsGccXml, 0, border=5, flag=wx.RIGHT | wx.EXPAND)
        parent.AddSizer(self.bsIncPath, 0, border=5, flag=wx.RIGHT | wx.EXPAND)
        parent.AddSizer(self.bsDefines, 0, border=5, flag=wx.RIGHT | wx.EXPAND)

    def _init_coll_bsDefines_Items(self, parent):
        # generated method, don't edit

        parent.AddWindow(self.staticText4, 0, border=5,
              flag=wx.ALIGN_CENTER_VERTICAL | wx.LEFT)
        parent.AddWindow(self.listDefines, 1, border=10,
              flag=wx.RIGHT | wx.BOTTOM | wx.TOP | wx.ALIGN_CENTER_VERTICAL | wx.EXPAND)

    def _init_coll_menuBar_Menus(self, parent):
        # generated method, don't edit

        parent.Append(menu=self.menueFile, title=u'&File')

    def _init_coll_menuIncludes_Items(self, parent):
        # generated method, don't edit

        parent.Append(help=u'Add new include directory for gcc',
              id=wxID_MAINFRAMEMENUINCLUDESADDINC, kind=wx.ITEM_NORMAL,
              text=u'Add ...')
        parent.Append(help=u'Remove selected include directory',
              id=wxID_MAINFRAMEMENUINCLUDESITEMS1, kind=wx.ITEM_NORMAL,
              text=u'Remove')
        self.Bind(wx.EVT_MENU, self.OnMenueIncludesAdd,
              id=wxID_MAINFRAMEMENUINCLUDESADDINC)
        self.Bind(wx.EVT_MENU, self.OnMenueIncludesRemove,
              id=wxID_MAINFRAMEMENUINCLUDESITEMS1)

    def _init_coll_menueFile_Items(self, parent):
        # generated method, don't edit

        parent.Append(help=u'Create new Project', id=wxID_MAINFRAMEMENUEFILENEW,
              kind=wx.ITEM_NORMAL, text=u'&New Project')
        parent.Append(help=u'Open existing Project',
              id=wxID_MAINFRAMEMENUEFILEOPEN, kind=wx.ITEM_NORMAL,
              text=u'&Open  Project')
        parent.Append(help=u'Save current Project',
              id=wxID_MAINFRAMEMENUEFILESAVE, kind=wx.ITEM_NORMAL,
              text=u'&Save  Project')
        parent.AppendSeparator()
        parent.AppendMenu(help=u'Open recently used Project',
              id=wxID_MAINFRAMEMENUEFILERECENT, submenu=wx.Menu(),
              text=u'Recent  Projects')
        parent.AppendSeparator()
        parent.Append(help='', id=wxID_MAINFRAMEMENUEFILEEXIT,
              kind=wx.ITEM_NORMAL, text=u'&Exit')
        self.Bind(wx.EVT_MENU, self.OnMenueFileNewMenu,
              id=wxID_MAINFRAMEMENUEFILENEW)
        self.Bind(wx.EVT_MENU, self.OnMenueFileOpenMenu,
              id=wxID_MAINFRAMEMENUEFILEOPEN)
        self.Bind(wx.EVT_MENU, self.OnMenueFileSaveMenu,
              id=wxID_MAINFRAMEMENUEFILESAVE)
        self.Bind(wx.EVT_MENU, self.OnMenueFileExitMenu,
              id=wxID_MAINFRAMEMENUEFILEEXIT)

    def _init_coll_menuDefines_Items(self, parent):
        # generated method, don't edit

        parent.Append(help=u'Add new define for gcc',
              id=wxID_MAINFRAMEMENUDEFINESADDDEF, kind=wx.ITEM_NORMAL,
              text=u'Add ...')
        parent.Append(help=u'Remove selected define',
              id=wxID_MAINFRAMEMENUDEFINESREMOVEDEF, kind=wx.ITEM_NORMAL,
              text=u'Remove')
        self.Bind(wx.EVT_MENU, self.OnMenueDefinesAddDefine,
              id=wxID_MAINFRAMEMENUDEFINESADDDEF)
        self.Bind(wx.EVT_MENU, self.OnMenueDefinesRemoveDefine,
              id=wxID_MAINFRAMEMENUDEFINESREMOVEDEF)

    def _init_coll_nbSettings_Pages(self, parent):
        # generated method, don't edit

        parent.AddPage(imageId=-1, page=self.panelNbSettings, select=True,
              text=u'Settings')

    def _init_coll_notebook1_Pages(self, parent):
        # generated method, don't edit

        parent.AddPage(imageId=-1, page=self.textOutput, select=True,
              text=u'Output')

    def _init_coll_statusBar_Fields(self, parent):
        # generated method, don't edit
        parent.SetFieldsCount(3)

        parent.SetStatusText(number=0, text=u'<helptextOrStatus>')
        parent.SetStatusText(number=1, text=u'<parseTime>')
        parent.SetStatusText(number=2, text=u'<compileTime>')

        parent.SetStatusWidths([-1, -1, -1])

    def _init_sizers(self):
        # generated method, don't edit
        self.bsMain = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsMainUpper = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsMainLow = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsUpLeft = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsUpRight = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsMULSettings = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsMURCompile = wx.BoxSizer(orient=wx.VERTICAL)

        self.bsMURButtons = wx.BoxSizer(orient=wx.HORIZONTAL)

        self.bsHeader = wx.BoxSizer(orient=wx.HORIZONTAL)

        self.bsGccXml = wx.BoxSizer(orient=wx.HORIZONTAL)

        self.bsIncPath = wx.BoxSizer(orient=wx.HORIZONTAL)

        self.bsDefines = wx.BoxSizer(orient=wx.HORIZONTAL)

        self._init_coll_bsMain_Items(self.bsMain)
        self._init_coll_bsMainUpper_Items(self.bsMainUpper)
        self._init_coll_bsMainLow_Items(self.bsMainLow)
        self._init_coll_bsUpLeft_Items(self.bsUpLeft)
        self._init_coll_bsUpRight_Items(self.bsUpRight)
        self._init_coll_bsMULSettings_Items(self.bsMULSettings)
        self._init_coll_bsMURCompile_Items(self.bsMURCompile)
        self._init_coll_bsMURButtons_Items(self.bsMURButtons)
        self._init_coll_bsHeader_Items(self.bsHeader)
        self._init_coll_bsGccXml_Items(self.bsGccXml)
        self._init_coll_bsIncPath_Items(self.bsIncPath)
        self._init_coll_bsDefines_Items(self.bsDefines)

        self.panelSHUp.SetSizer(self.bsMainUpper)
        self.panelSettings.SetSizer(self.bsUpLeft)
        self.panelButtons.SetSizer(self.bsMURButtons)
        self.panelSHLow.SetSizer(self.bsMainLow)
        self.panelMain.SetSizer(self.bsMain)
        self.panelCode.SetSizer(self.bsUpRight)
        self.panelNbSettings.SetSizer(self.bsMULSettings)

    def _init_utils(self):
        # generated method, don't edit
        self.menueFile = wx.Menu(title='')

        self.menuBar = wx.MenuBar()

        self.menuIncludes = wx.Menu(title='')

        self.menuDefines = wx.Menu(title='')

        self._init_coll_menueFile_Items(self.menueFile)
        self._init_coll_menuBar_Menus(self.menuBar)
        self._init_coll_menuIncludes_Items(self.menuIncludes)
        self._init_coll_menuDefines_Items(self.menuDefines)

    def _init_ctrls(self, prnt):
        # generated method, don't edit
        wx.Frame.__init__(self, id=wxID_MAINFRAME, name=u'MainFrame',
              parent=prnt, pos=wx.Point(0, -2), size=wx.Size(851, 620),
              style=wx.DEFAULT_FRAME_STYLE, title=u'Py++ Code generator')
        self._init_utils()
        self.SetClientSize(wx.Size(843, 586))
        self.SetMenuBar(self.menuBar)

        self.statusBar = wx.StatusBar(id=wxID_MAINFRAMESTATUSBAR,
              name=u'statusBar', parent=self, style=0)
        self._init_coll_statusBar_Fields(self.statusBar)
        self.SetStatusBar(self.statusBar)

        self.panelMain = wx.Panel(id=wxID_MAINFRAMEPANELMAIN, name=u'panelMain',
              parent=self, pos=wx.Point(0, 0), size=wx.Size(843, 543),
              style=wx.TAB_TRAVERSAL)

        self.splitterHorizontal = wx.SplitterWindow(id=wxID_MAINFRAMESPLITTERHORIZONTAL,
              name=u'splitterHorizontal', parent=self.panelMain, pos=wx.Point(5,
              5), size=wx.Size(833, 533), style=0)
        self.splitterHorizontal.SetNeedUpdating(False)
        self.splitterHorizontal.SetMinimumPaneSize(0)

        self.panelSHUp = wx.Panel(id=wxID_MAINFRAMEPANELSHUP, name=u'panelSHUp',
              parent=self.splitterHorizontal, pos=wx.Point(0, 0),
              size=wx.Size(833, 10), style=wx.TAB_TRAVERSAL)

        self.panelSHLow = wx.Panel(id=wxID_MAINFRAMEPANELSHLOW,
              name=u'panelSHLow', parent=self.splitterHorizontal,
              pos=wx.Point(0, 14), size=wx.Size(833, 519),
              style=wx.TAB_TRAVERSAL)
        self.splitterHorizontal.SplitHorizontally(self.panelSHUp,
              self.panelSHLow, 300)

        self.notebook1 = wx.Notebook(id=wxID_MAINFRAMENOTEBOOK1,
              name='notebook1', parent=self.panelSHLow, pos=wx.Point(0, 0),
              size=wx.Size(833, 519), style=0)
        self.notebook1.SetLabel(u'Label')

        self.textOutput = wx.TextCtrl(id=wxID_MAINFRAMETEXTOUTPUT,
              name=u'textOutput', parent=self.notebook1, pos=wx.Point(0, 0),
              size=wx.Size(825, 493),
              style=wx.TE_RICH | wx.TE_READONLY | wx.TE_MULTILINE, value=u'')

        self.splitterVertical = wx.SplitterWindow(id=wxID_MAINFRAMESPLITTERVERTICAL,
              name=u'splitterVertical', parent=self.panelSHUp, pos=wx.Point(0,
              0), size=wx.Size(833, 5), style=wx.SP_3D)
        self.splitterVertical.SetNeedUpdating(True)
        self.splitterVertical.SetMinimumPaneSize(0)

        self.panelSettings = wx.Panel(id=wxID_MAINFRAMEPANELSETTINGS,
              name=u'panelSettings', parent=self.splitterVertical,
              pos=wx.Point(0, 0), size=wx.Size(10, 5), style=wx.TAB_TRAVERSAL)

        self.panelCode = wx.Panel(id=wxID_MAINFRAMEPANELCODE, name=u'panelCode',
              parent=self.splitterVertical, pos=wx.Point(14, 0),
              size=wx.Size(819, 5), style=wx.TAB_TRAVERSAL)
        self.splitterVertical.SplitVertically(self.panelSettings,
              self.panelCode, 300)

        self.nbSettings = wx.Notebook(id=wxID_MAINFRAMENBSETTINGS,
              name=u'nbSettings', parent=self.panelSettings, pos=wx.Point(0, 0),
              size=wx.Size(10, 5), style=0)
        self.nbSettings.SetLabel(u'Label')
        self.nbSettings.SetHelpText(u'')

        self.panelNbSettings = wx.Panel(id=wxID_MAINFRAMEPANELNBSETTINGS,
              name=u'panelNbSettings', parent=self.nbSettings, pos=wx.Point(0,
              0), size=wx.Size(2, 0), style=wx.TAB_TRAVERSAL)
        self.panelNbSettings.Show(True)
        self.panelNbSettings.SetMinSize(wx.Size(100, 100))

        self.textCtrl2 = wx.TextCtrl(id=wxID_MAINFRAMETEXTCTRL2,
              name='textCtrl2', parent=self.panelCode, pos=wx.Point(0, 17),
              size=wx.Size(819, 0), style=wx.TE_MULTILINE, value=u'')

        self.panelButtons = wx.Panel(id=wxID_MAINFRAMEPANELBUTTONS,
              name=u'panelButtons', parent=self.panelCode, pos=wx.Point(166,
              -18), size=wx.Size(486, 23), style=wx.TAB_TRAVERSAL)

        self.butGenXml = wx.Button(id=wxID_MAINFRAMEBUTGENXML,
              label=u'Generate XML code', name=u'butGenXml',
              parent=self.panelButtons, pos=wx.Point(10, 0), size=wx.Size(140,
              23), style=0)
        self.butGenXml.Bind(wx.EVT_BUTTON, self.OnButGenXmlButton,
              id=wxID_MAINFRAMEBUTGENXML)

        self.butGenCpp = wx.Button(id=wxID_MAINFRAMEBUTGENCPP,
              label=u'Generate C++ code', name=u'butGenCpp',
              parent=self.panelButtons, pos=wx.Point(170, 0), size=wx.Size(142,
              23), style=0)
        self.butGenCpp.Bind(wx.EVT_BUTTON, self.OnButGenCppButton,
              id=wxID_MAINFRAMEBUTGENCPP)

        self.butGenPyPP = wx.Button(id=wxID_MAINFRAMEBUTGENPYPP,
              label=u'Generate Py++ code', name=u'butGenPyPP',
              parent=self.panelButtons, pos=wx.Point(332, 0), size=wx.Size(144,
              23), style=0)
        self.butGenPyPP.Bind(wx.EVT_BUTTON, self.OnButGenPyPPButton,
              id=wxID_MAINFRAMEBUTGENPYPP)

        self.textHeader = wx.TextCtrl(id=wxID_MAINFRAMETEXTHEADER,
              name=u'textHeader', parent=self.panelNbSettings, pos=wx.Point(56,
              20), size=wx.Size(0, 21), style=0, value=u'')

        self.butHeaders = wx.Button(id=wxID_MAINFRAMEBUTHEADERS, label=u'...',
              name=u'butHeaders', parent=self.panelNbSettings, pos=wx.Point(-38,
              19), size=wx.Size(28, 23), style=0)
        self.butHeaders.Bind(wx.EVT_BUTTON, self.OnButHeadersButton,
              id=wxID_MAINFRAMEBUTHEADERS)

        self.staticText1 = wx.StaticText(id=wxID_MAINFRAMESTATICTEXT1,
              label=u'Header\nFile', name='staticText1',
              parent=self.panelNbSettings, pos=wx.Point(5, 15), size=wx.Size(51,
              30), style=wx.ALIGN_CENTRE)

        self.staticText2 = wx.StaticText(id=wxID_MAINFRAMESTATICTEXT2,
              label=u'GccXml', name='staticText2', parent=self.panelNbSettings,
              pos=wx.Point(5, 60), size=wx.Size(51, 23), style=wx.ALIGN_CENTRE)

        self.textGccXml = wx.TextCtrl(id=wxID_MAINFRAMETEXTGCCXML,
              name=u'textGccXml', parent=self.panelNbSettings, pos=wx.Point(56,
              61), size=wx.Size(0, 21), style=0, value=u'')

        self.butGccXml = wx.Button(id=wxID_MAINFRAMEBUTGCCXML, label=u'...',
              name=u'butGccXml', parent=self.panelNbSettings, pos=wx.Point(-38,
              60), size=wx.Size(28, 23), style=0)
        self.butGccXml.Bind(wx.EVT_BUTTON, self.OnButGccXmlButton,
              id=wxID_MAINFRAMEBUTGCCXML)

        self.staticText3 = wx.StaticText(id=wxID_MAINFRAMESTATICTEXT3,
              label=u'Include\nPath', name='staticText3',
              parent=self.panelNbSettings, pos=wx.Point(5, 116),
              size=wx.Size(51, 37), style=wx.ALIGN_CENTRE)

        self.listIncludes = wx.ListCtrl(id=wxID_MAINFRAMELISTINCLUDES,
              name=u'listIncludes', parent=self.panelNbSettings,
              pos=wx.Point(56, 102), size=wx.Size(0, 66),
              style=wx.LC_HRULES | wx.LC_NO_HEADER | wx.LC_REPORT)
        self.listIncludes.Bind(wx.EVT_RIGHT_DOWN, self.OnListIncludesRightDown)
        self.listIncludes.Bind(wx.EVT_SIZE, self.OnListIncludesSize)

        self.staticText4 = wx.StaticText(id=wxID_MAINFRAMESTATICTEXT4,
              label=u'Defines', name='staticText4', parent=self.panelNbSettings,
              pos=wx.Point(5, 209), size=wx.Size(51, 25),
              style=wx.ALIGN_CENTRE)

        self.listDefines = wx.ListCtrl(id=wxID_MAINFRAMELISTDEFINES,
              name=u'listDefines', parent=self.panelNbSettings, pos=wx.Point(56,
              188), size=wx.Size(0, 68),
              style=wx.LC_HRULES | wx.LC_NO_HEADER | wx.LC_REPORT)
        self.listDefines.Bind(wx.EVT_RIGHT_DOWN, self.OnListDefinesRightDown)
        self.listDefines.Bind(wx.EVT_SIZE, self.OnListDefinesSize)

        self.staticText5 = wx.StaticText(id=wxID_MAINFRAMESTATICTEXT5,
              label=u'Code', name='staticText5', parent=self.panelCode,
              pos=wx.Point(0, 0), size=wx.Size(25, 13), style=0)

        self._init_coll_notebook1_Pages(self.notebook1)
        self._init_coll_nbSettings_Pages(self.nbSettings)

        self._init_sizers()

    def __init__(self, parent):
        self._init_ctrls(parent)
        self._setup_ide_ctrls()
        self.SetSize((self.GetSize()[0]+1,self.GetSize()[1]+1))
        
    def OnMenueFileNewMenu(self, event):
        event.Skip()

    def OnMenueFileOpenMenu(self, event):
        event.Skip()

    def OnMenueFileSaveMenu(self, event):
        event.Skip()

    def OnMenueFileRecentMenu(self, event):
        event.Skip()

    def OnMenueFileExitMenu(self, event):
        event.Skip()

    def OnTextGenCodeRightDown(self, event):
        event.Skip()

    def OnListIncludesRightDown(self, event):
        self.PopupMenu(self.menuIncludes)

    def OnListDefinesRightDown(self, event):
        self.PopupMenu(self.menuDefines)

    def OnButGenXmlButton(self, event):
        self._controller.GenXmlCode()

    def OnButGenCppButton(self, event):
        self._controller.GenCppCode()

    def OnButGenPyPPButton(self, event):
        self._controller.GenPyPPCode()

    def OnButHeadersButton(self, event):
        """Callback for button event"""
        self._controller.OpenDlgHeader()
        event.Skip()

    def OnButGccXmlButton(self, event):
        """Callback for button event"""
        self._controller.OpenDlgGccXml()
        event.Skip()
        
    def set_controller(self, controller):
        """Set controller of MVC"""
        self._controller = controller
        
    def _setup_ide_ctrls(self):
        """Do ide related settings in ctrls"""
        list_inc = self.listIncludes
        list_def = self.listDefines
        
        # Init list controls
        for list_ctrl in (list_inc, list_def):
            list_ctrl.InsertColumn(0, "Path")

    def OnListIncludesSize(self, event):
        """Handle resize of listIncludes"""
        list_ctrl = self.listIncludes
        list_ctrl.SetColumnWidth(0, list_ctrl.GetSize().GetWidth() - 30 )
        event.Skip()

    def OnListDefinesSize(self, event):
        """Handle resize of listDefines"""
        list_ctrl = self.listDefines
        list_ctrl.SetColumnWidth(0, list_ctrl.GetSize().GetWidth() - 30 )
        event.Skip()

    def OnMenueIncludesAdd(self, event):
        self._controller.OpenDlgAddInclude()
        event.Skip()

    def OnMenueIncludesRemove(self, event):
        event.Skip()

    def OnMenueDefinesAddDefine(self, event):
        self._controller.OpenDlgAddMacro()
        event.Skip()
        
    def OnMenueDefinesRemoveDefine(self, event):
        event.Skip()

        
        
        
    
