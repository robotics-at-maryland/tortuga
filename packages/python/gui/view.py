# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/gui/view.py

# Project Imports
from core import Interface

class IPanelProvider(Interface):

    #@classmethod
    def get_panels(subsystems, parent):
        """
        This is a class method.
        
        @type  subsystems: [ISubsystem]
        @param subsystems: A possible module that these panels will work from
        
        @type  parent: wx.Window
        @param parent: The parent of the newly created panels.
        
        @rtype:  [(wx.aui.AuiPaneInfo, wx.Window)]
        @return: A list of tuples containing wx.Window's and aui pane info for
                 where they should be placed.
        """
        pass
    
        
