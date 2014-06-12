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
    
    def __init__(self, parent, introText, locals,
                 startup = None, redirect = True):
        wx.py.shell.Shell.__init__(self, parent, locals = locals,
                                   introText = introText,
                                   startupScript = startup,
                                   InterpClass = Intrepreter)

        if redirect:
            self.redirectStdout()
            self.redirectStderr()
        
        self.Bind(wx.EVT_CLOSE, self._onClose)
        
    def _onClose(self, closeEvent):
        # Make the rest of the output go the console
        self.redirectStdout(False)
        self.redirectStderr(False)
        
        # Remove interpreter references
        if hasattr(self, 'interp'):
            del self.interp
        
        closeEvent.Skip()
            
    
