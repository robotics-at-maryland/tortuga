# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/oci/view/shell.py

"""
Wraps the wxPython py.shell.Shell class in a more usable package
"""

# Library Imports
import wx.py

class Intrepreter(wx.py.interpreter.Interpreter):
    """
    Wraps the standard wx.py.intrepreter to suppress normal intro text
    """
    
    def __init__(self, *args, **kwargs):
        wx.py.interpreter.Interpreter.__init__(self, *args, **kwargs)
        self.introText = ''
        
class ShellPanel(wx.py.shell.Shell):
    """
    A python interpreter with code completion and history support.
    """
    
    def __init__(self, parent, introText, locals):
        wx.py.shell.Shell.__init__(self, parent, locals = locals,
                                   introText = introText, 
                                   InterpClass = Intrepreter)      
    