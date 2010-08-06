import sys
import time
import wx

class Minutes(wx.App):

    def __init__(self):
        wx.App.__init__(self, 0)

    def OnInit(self):
        self._frame = wx.Frame(None, title = "Minutes",size=wx.Size(w=302,h=345))       # Create sizer to place everything in
        self._sizer = wx.BoxSizer(wx.VERTICAL)
        self._textctrl = wx.TextCtrl(self._frame, id = 1, value = "Tweet the deets!",size=wx.Size(300,30), style = wx.TE_PROCESS_ENTER)
        wx.EVT_TEXT_ENTER(self, 1, self._onEnter)
        self._label = wx.StaticText(self._frame, label = "", size = wx.Size(200, 16))
        self._entrylist = wx.ListBox(self._frame, id=1, size=wx.Size(300,300))
        self._sizer.Add(self._label, flag = wx.ALIGN_RIGHT)
        self._sizer.Add(self._textctrl, flag = wx.ALIGN_CENTER)
        self._sizer.Add(self._entrylist, flag = wx.ALIGN_CENTER)
        self._frame.SetSizer(self._sizer)
        self._frame.Show(True)
        self.SetTopWindow(self._frame)

        return True

    def _onEnter(self, event):
        FILE = open("log.txt","a")
        #need way to choose where to put files
        FILE.write(" * '''"+ time.strftime("%I:%M:%S %p", time.localtime()) + "''': " + self._textctrl.GetValue()+"\n")
        
        self._label.SetLabel(label = "wrote to log.txt at " + time.strftime("%I:%M:%S %p", time.localtime()))
        self._entrylist.Insert(time.strftime("%I:%M:%S %p: ", time.localtime()) + self._textctrl.GetValue(),0)
        self._textctrl.SetValue("")
        FILE.close()

def main():
    application = Minutes()
    FILE = open("log.txt","a")
    FILE.write("== " + time.strftime("%a, %d %b %Y", time.localtime()) + " ==\n")
    FILE.close()
    application.MainLoop()

if __name__ == '__main__':
    sys.exit(main())
