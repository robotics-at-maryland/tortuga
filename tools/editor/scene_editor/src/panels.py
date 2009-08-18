import wx

class InfoPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        layout = wx.GridBagSizer(5, 5)

        self._heightLabel = wx.StaticText(self, label = "H:")
        layout.Add(self._heightLabel, pos = (0, 0))
        self._height = wx.TextCtrl(self)
        self._height.SetMaxLength(4)
        layout.Add(self._height, pos = (0, 1))

        self._widthLabel = wx.StaticText(self, label = "W:")
        layout.Add(self._widthLabel, pos = (1, 0))
        self._width = wx.TextCtrl(self)
        self._width.SetMaxLength(4)
        layout.Add(self._width, pos = (1, 1))

        self._depthLabel = wx.StaticText(self, label = "D:")
        layout.Add(self._depthLabel, pos = (2, 0))
        self._depth = wx.TextCtrl(self)
        self._depth.SetMaxLength(4)
        layout.Add(self._depth, pos = (2, 1))

        self._rotationLabel = wx.StaticText(self, label = "R:")
        layout.Add(self._rotationLabel, pos = (3, 0))
        self._rotation = wx.TextCtrl(self)
        self._rotation.SetMaxLength(4)
        layout.Add(self._rotation, pos = (3, 1))

        self.SetSizerAndFit(layout)

class ObjectPanel(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        layout = wx.GridBagSizer(5, 5)

        self._heightLabel = wx.StaticText(self, label = "H:")
        layout.Add(self._heightLabel, pos = (0, 0))
        self._height = wx.TextCtrl(self)
        layout.Add(self._height, pos = (0, 1))

        self._widthLabel = wx.StaticText(self, label = "W:")
        layout.Add(self._widthLabel, pos = (1, 0))
        self._width = wx.TextCtrl(self)
        layout.Add(self._width, pos = (1, 1))

        self._depthLabel = wx.StaticText(self, label = "D:")
        layout.Add(self._depthLabel, pos = (0, 2))
        self._depth = wx.TextCtrl(self)
        layout.Add(self._depth, pos = (0, 3))

        self._rotationLabel = wx.StaticText(self, label = "R:")
        layout.Add(self._rotationLabel, pos = (1, 2))
        self._rotation = wx.TextCtrl(self)
        layout.Add(self._rotation, pos = (1, 3))

        self.SetSizerAndFit(layout)

