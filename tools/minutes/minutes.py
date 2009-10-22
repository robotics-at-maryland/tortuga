import sys
import wx

class SimpleApp(wx.App):

    def __init__(self):
        wx.App.__init__(self, 0)

    def OnInit(self):
        # Create a main frame for the program
        self._frame = wx.Frame(None, title = "Minutes")

        # Create sizer to place everything in
        self._sizer = wx.BoxSizer(wx.VERTICAL)

        # Create a label and add it to the sizer
        self._label = wx.StaticText(self._frame)
        self._sizer.Add(self._label, flag = wx.ALIGN_CENTER)

        self._textctrl = wx.TextCtrl(self._frame, id = 1, value = "Tweet the deets!",style = wx.TE_PROCESS_ENTER)
        wx.EVT_TEXT_ENTER(self, 1, self._onEnter)

        # Sets the sizer for the frame
        self._frame.SetSizer(self._sizer)

        # Changes the window size to the minimum needed for the window
        #self._sizer.Fit(self._frame)

        # Displays the frame and sets it as the top window
        self._frame.Show(True)
        self.SetTopWindow(self._frame)

        # Tells the program that initiation of the window was successful
        return True

    def _onEnter(self, event):
        FILE = open("log.txt","a")
        FILE.write(self._textctrl.GetValue()+"\n")
        self._textctrl.SetValue("")
        FILE.close()

# Main function
def main():
    # Creates an instance of the application and puts it into a loop until the program is closed
    application = SimpleApp()
    application.MainLoop()

# Since this is on the global scope, it get run any time that the file is referenced
# This keeps it so it only runs the main function when this is the main program
if __name__ == '__main__':
    sys.exit(main())
