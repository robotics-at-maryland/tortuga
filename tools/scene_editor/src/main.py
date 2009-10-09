import sys
import wx

import editor.app

class Application(wx.App):

    def __init__(self):
        wx.App.__init__(self, 0)

    def OnInit(self):
        # Create a main frame for the program
        self._frame = editor.app.SceneEditor(None)

        # Displays the frame and sets it as the top window
        self._frame.Show(True)
        self.SetTopWindow(self._frame)

        return True

# Main function
def main():
    app = Application()
    app.MainLoop()

if __name__ == '__main__':
    sys.exit(main())
