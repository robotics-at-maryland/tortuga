# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/gui/simview.py

# Library Imports
import wx
import wx.aui

# Project Imports
from core import Component, implements

from sim.simulation import Simulation
from ram.sim.input import OISInputForwarder

from gui.view import IPanelProvider
from gui.wxogre import wxOgre
from gui.input import InputForwarder


class SimPanelProvider(Component):
    implements(IPanelProvider)
    
    def get_panels(self, subsystems, parent):
        # This dummy window holds the rendering context to make sure the user
        # doesn't close the window containing it
        dummy = wxOgre(None, parent, -1, wx.DefaultPosition, wx.DefaultSize, 0, 
                       wx.DefaultValidator, "dummy")
        dummy.Show(False)
        
        # Our real window
        main = wxOgre(None, parent, -1, wx.DefaultPosition, wx.DefaultSize, 0, 
                       wx.DefaultValidator, "main")
        main_info = wx.aui.AuiPaneInfo().Name("main").Caption("Simulation").Center()
        
        main._input_forwarder = InputForwarder(Simulation.get().input_system)
        main._input_forwarder.forward_window(main)

        
        return [(main_info, main)]
