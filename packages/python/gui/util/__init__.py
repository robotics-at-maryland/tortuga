# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>

# Standard python imports
import ctypes as __ctypes
import os.path as __path
import os as __os



# Library Imports
import wx as __wx

# Load library
__lib_path = __path.join(__os.environ['MRBC_SVN_ROOT'],'packages','build',
                         'extensions','wxogre','lib_wxogre.so')
__lib = __ctypes.cdll.LoadLibrary(__path.abspath(__lib_path))
# Look up function
__get_window_handle_str = __lib.get_window_handle_str
# Set types to match prototype: char* get_window_handle_str(void*)
__get_window_handle_str.restype = __ctypes.c_char_p
__get_window_handle_str.argtypes = [__ctypes.c_void_p]

def get_window_handle_str(window):
    """
    Returns the Ogre friendly window format.  It will not allow itself to be
    called with an invalid type, but calling the underlying C wrapped C++
    function with anything but a wxWindow or a subclass of it, will result in
    a hard crash.
    """
    if not isinstance(window, __wx.Window):
        raise TypeError, 'Must be called with a wx.Window or a subclass'
    
    # This automatically performs the int -> void* cast
    return __get_window_handle_str(int(window.this))
