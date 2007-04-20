#Boa:Dialog:MacroDialog

import wx

def create(parent):
    return MacroDialog(parent)

[wxID_MACRODIALOG, wxID_MACRODIALOGBUTTONCANCEL, wxID_MACRODIALOGBUTTONOK, 
 wxID_MACRODIALOGPANELDOWN, wxID_MACRODIALOGPANELUP, 
 wxID_MACRODIALOGSTATICLINE1, wxID_MACRODIALOGSTATICTEXT1, 
 wxID_MACRODIALOGTEXTMACRO, 
] = [wx.NewId() for _init_ctrls in range(8)]

class MacroDialog(wx.Dialog):
    def _init_ctrls(self, prnt):
        # generated method, don't edit
        wx.Dialog.__init__(self, id=wxID_MACRODIALOG, name=u'MacroDialog',
              parent=prnt, pos=wx.Point(127, 171), size=wx.Size(376, 231),
              style=wx.DEFAULT_DIALOG_STYLE, title=u'Enter new Macro')
        self.SetClientSize(wx.Size(368, 197))
        self.SetMaxSize(wx.Size(376, 231))
        self.SetMinSize(wx.Size(376, 231))

        self.panelUp = wx.Panel(id=wxID_MACRODIALOGPANELUP, name=u'panelUp',
              parent=self, pos=wx.Point(0, -8), size=wx.Size(368, 152),
              style=wx.TAB_TRAVERSAL)

        self.panelDown = wx.Panel(id=wxID_MACRODIALOGPANELDOWN,
              name=u'panelDown', parent=self, pos=wx.Point(0, 144),
              size=wx.Size(368, 74), style=wx.TAB_TRAVERSAL)

        self.buttonOk = wx.Button(id=wxID_MACRODIALOGBUTTONOK, label=u'Ok',
              name=u'buttonOk', parent=self.panelDown, pos=wx.Point(40, 17),
              size=wx.Size(75, 23), style=0)
        self.buttonOk.Bind(wx.EVT_BUTTON, self.OnButtonOk,
              id=wxID_MACRODIALOGBUTTONOK)

        self.buttonCancel = wx.Button(id=wxID_MACRODIALOGBUTTONCANCEL,
              label=u'Cancel', name=u'buttonCancel', parent=self.panelDown,
              pos=wx.Point(256, 16), size=wx.Size(75, 23), style=0)
        self.buttonCancel.Bind(wx.EVT_BUTTON, self.OnButtonCancel,
              id=wxID_MACRODIALOGBUTTONCANCEL)

        self.staticLine1 = wx.StaticLine(id=wxID_MACRODIALOGSTATICLINE1,
              name='staticLine1', parent=self.panelDown, pos=wx.Point(9, 0),
              size=wx.Size(343, 2), style=0)

        self.textMacro = wx.TextCtrl(id=wxID_MACRODIALOGTEXTMACRO,
              name=u'textMacro', parent=self.panelUp, pos=wx.Point(32, 40),
              size=wx.Size(304, 88), style=wx.TE_MULTILINE, value=u'')

        self.staticText1 = wx.StaticText(id=wxID_MACRODIALOGSTATICTEXT1,
              label=u'Definition of macro (ABS=12)', name='staticText1',
              parent=self.panelUp, pos=wx.Point(32, 24), size=wx.Size(140, 13),
              style=0)

    def __init__(self, parent):
        self._init_ctrls(parent)

    def OnButtonOk(self, event):
        """End modal dialog with True"""
        self.EndModal(wx.OK)
        event.Skip()

    def OnButtonCancel(self, event):
        """End modal dialog with False"""
        self.EndModal(wx.CANCEL)
        event.Skip()
        
