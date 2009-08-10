import wx

class InfoPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        layout = wx.GridBagSizer(5, 5)
