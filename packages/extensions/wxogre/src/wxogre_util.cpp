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
// Needed for random GTK/GDK macros and functions
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

// Needed for crazy GTK_PIZZA stuff
#include <wx/gtk/win_gtk.h>
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
    // Should help reduce flickering
    //SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    std::stringstream handleStream;

    // wxWidgets uses serverl internal GtkWidgets, the GetHandle method returns
    // a different one then this, but wxWidgets GLCanvas uses this one to
    // interact with GLX with, so we will do the same.
    GtkWidget* private_handle =  window->m_wxwindow;

    // Prevents flicker
    gtk_widget_set_double_buffered( private_handle, FALSE );

    // Grabs the window for use in the below macros
    GdkWindow *window = GTK_PIZZA(private_handle)->bin_window;
    Display* display = GDK_WINDOW_XDISPLAY(window);
    Window wid = GDK_WINDOW_XWINDOW(window);

    // Display
    handleStream << (unsigned long)display << ':';

/*#if OGRE_PATCH_VERSION == 0
    // Screen (returns ":display.screen   ")
    std::string screenStr = DisplayString(display);
    int dotpos = screenStr.find(".", 0);
    screenStr = screenStr.substr(dotpos + 1, screenStr.length() - dotpos);
    handleStream << screenStr << ':';
#endif
*/
    // XID (typedef of an unsigned int)
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
