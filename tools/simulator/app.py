# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/app.py

# Python Imports
import sys
import os.path

# Library Imports
import wx
import yaml

# Project Imports
import event
import frame
from module import ModuleManager
from baseapp import AppBase

            
class Application(wx.App, AppBase):
    
    def OnInit(self):
        # Load main application
        AppBase.__init__(self, os.path.join('data', 'config','sim.yml'), False)
        
        # Setup GUI and bind to main frames activate event, we can't start the 
        # simulation until the activate event is finished, so the rest of the
        # initialization is done there.
        self._frame = frame.MainFrame(self._config)
        self.Bind(wx.EVT_ACTIVATE, self._finish_init, self._frame)
        
        # Show the main frame
        self._frame.Show(True)
        self.SetTopWindow(self._frame)
        
        return True
        
    def _finish_init(self, evt):
        # Unbind from the event so we don't get called twice
        self.Bind(wx.EVT_ACTIVATE, None, self._frame)
        
        print 'activated'
        # Load all the modules
        ModuleManager.get().load_modules(self._config)
        
        # TODO: Load GUI Settings
        
def main():            
    app = Application(0)
    app.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)