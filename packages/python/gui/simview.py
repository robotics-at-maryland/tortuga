# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/gui/view.py

# Library Imports
import wx
import wx.aui

# Project Imports
from core import Component, implements

from sim.simulation import Simulation
from sim.input import OISInputForwarder

from gui.view import IPanelProvider
from gui.wxogre import wxOgre
from gui.input import InputForwarder


class SimPanelProvider(Component):
    implements(IPanelProvider)
    
    def supports_module(self, mod):
        if isinstance(mod, Simulation):
            return True
        return False
    
    def get_panels(self, mod, parent):
        dummy = wxOgre(None, parent, -1, wx.DefaultPosition, wx.Size(300,300))
        dummy.SetName("dummy")
        dummy._init_ogre()
        dummy_info = wx.aui.AuiPaneInfo().Name("dummy").Caption("Dummy").Left()
        
        main = wxOgre(None, parent, -1)
        main.SetName("main")
        main._init_ogre()
        main_info = wx.aui.AuiPaneInfo().Name("main").Caption("Simulation").Center()
        
#        test = wxOgre(None, parent, -1)
#        test.SetName("test")
#        test._init_ogre()
#        test_info = wx.aui.AuiPaneInfo().Name("test").Caption("test2").Left()
    
        # Setup the input system dependent on platform
#        if wx.Platform == '__WXMSW__':
#            main._input_forwarder = InputForwarder(Simulation.get().input_system)
#            main._input_forwarder.forward_window(main)
#        elif wx.Platform == '__WXGTK__':
#            main._input_forwarder = OISInputForwarder({}, \
#                                              Simulation.get().input_system, 
#                                              main._render_window)
#        else:
#            raise "Error platform not supported"
#        
        return [(main_info, main), (dummy_info, dummy)]
#        main_info = wx.aui.AuiPaneInfo().Name("main").Caption("Simulation").Center()
#        main = wx.TextCtrl(parent, -1, "This is page 1" ,
#                               style=wx.TE_MULTILINE)
#        
#        return [(main_info, main)]
    