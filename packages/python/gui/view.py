# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/gui/view.py

# Project Imports
from ram.core import Interface

class IPanelProvider(Interface):

    #@staticmethod
    def getPanels(subsystems, parent):
        """
        This is a class method.
        
        @type  subsystems: [ISubsystem]
        @param subsystems: A possible module that these panels will work from
        
        @type  parent: wx.Windows returning function
        @param parent: Returns the parent of the newly created panels.
        
        @rtype:  [(wx.aui.AuiPaneInfo, wx.Window, [core.Subsystem])]
        @return: A list of tuples containing wx.Window's, aui pane info for
                 where they should be placed, and the Subsystems used by that
                 panel
        """
        pass
    
        
