# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>

# Standard python imports
import ctypes as _ctypes

# Library Imports
import wx as _wx

# Load library
_lib = _ctypes.cdll.LoadLibrary('python/libwxogre_util.so')
# Look up function
_get_window_handle_str = _lib.get_window_handle_str
# Set types to match prototype: char* get_window_handle_str(void*)
_get_window_handle_str.restype = _ctypes.c_char_p
_get_window_handle_str.argtypes = [_ctypes.c_void_p]

def get_window_handle_str(window):
    if not isinstance(window, _wx.Window):
        raise TypeError, 'Must be called with a wx.Window or a subclass'
    
    # This automatically performs the int -> void* cast
    return _get_window_handle_str(int(window.this))
