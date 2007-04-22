// Copyright (C) 2007 Maryland Robotics Club
// Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
// All rights reserved.
//
// Author: Joseph Lisee <jlisee@umd.edu>
// File:  utility/wxogre_util/src/wxogre_util.h

extern "C"
{
  /** Returns a window handle string from a wxWindow in Ogre form
     @param window
         Must be a pointer to a wxWindow or its subclass.
     @return
        The window handle in its display:xid form. 
   */
  char* get_window_handle_str(void* window);
}

// std includes
#include <sstream>
#include <string>
#include <cstring>

// wxWidgets includes
#include <wx/window.h>

// Platform specific includes
#ifdef __WXGTK__
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#endif

char* get_window_handle_str(void* _window)
{
  // Assume its a wxObject (can be dangerous)
  wxObject* window_obj = (wxObject*)_window;

  // Preform a dynamic cast to a wxWindow
  wxWindow* window = wxDynamicCast(window_obj, wxWindow);

  if (window)
  {
    std::string handle;
#ifdef __WXGTK__
    std::stringstream handleStream;
    GtkWidget* widget =  window->GetHandle();
  
    // Display
    Display* display = GDK_WINDOW_XDISPLAY( widget->window );
    handleStream << (unsigned long)display << ':';
  
  // Screen (returns ":display.screen")
  /* The new Ogre GLX renderer patch removes this need
     
    std::string screenStr = DisplayString(display);
    int dotpos = screenStr.find(".", 0);
    screenStr = screenStr.substr(dotpos + 1, screenStr.length() - dotpos);
    handleStream << screenStr << ':';*/

    // XID (typedef of an unsigned int)
    Window wid = GDK_WINDOW_XWINDOW( widget->window );
    handleStream << wid;
  
    handle = handleStream.str();
#else
#error External/Parent Window handle not supported on this platform
#endif

    // Allocate string for the return
    char* ret = new char[handle.length() + 1];
    strcpy(ret, handle.c_str());
    return ret;
  }

  return 0;
}
