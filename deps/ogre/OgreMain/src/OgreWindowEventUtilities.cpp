/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
#include "OgreStableHeaders.h"
#include "OgreWindowEventUtilities.h"
#include "OgreRenderWindow.h"
#include "OgreLogManager.h"
#include "OgreRoot.h"
#include "OgreException.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <X11/Xlib.h>
void GLXProc( const XEvent &event );
#endif

using namespace Ogre;

WindowEventUtilities::WindowEventListeners WindowEventUtilities::_msListeners;
WindowEventUtilities::Windows WindowEventUtilities::_msWindows;

//--------------------------------------------------------------------------------//
void WindowEventUtilities::messagePump()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	// Windows Message Loop (NULL means check all HWNDs belonging to this context)
	MSG  msg;
	while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	//GLX Message Pump (Display is probably Common for all RenderWindows.. But, to be safe loop
	//through all)
	Windows::iterator i = _msWindows.begin(), e = _msWindows.end();
	for( ; i != e; ++i )
	{
		::Display* display;
		(*i)->getCustomAttribute("DISPLAY", &display);
		while(XPending(display) > 0)
		{
			XEvent event;
			XNextEvent(display, &event);
			GLXProc(event);
		}
	}
#endif
}

//--------------------------------------------------------------------------------//
void WindowEventUtilities::addWindowEventListener( RenderWindow* window, WindowEventListener* listener )
{
	_msListeners.insert(std::make_pair(window, listener));
}

//--------------------------------------------------------------------------------//
void WindowEventUtilities::removeWindowEventListener( RenderWindow* window, WindowEventListener* listener )
{
	WindowEventListeners::iterator i = _msListeners.begin(), e = _msListeners.end();

	for( ; i != e; ++i )
	{
		if( i->first == window && i->second == listener )
		{
			_msListeners.erase(i);
			break;
		}
	}
}

//--------------------------------------------------------------------------------//
void WindowEventUtilities::_addRenderWindow(RenderWindow* window)
{
	_msWindows.push_back(window);
}

//--------------------------------------------------------------------------------//
void WindowEventUtilities::_removeRenderWindow(RenderWindow* window)
{
	Windows::iterator i = std::find(_msWindows.begin(), _msWindows.end(), window);
	if( i != _msWindows.end() )
		_msWindows.erase( i );
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//--------------------------------------------------------------------------------//
LRESULT CALLBACK WindowEventUtilities::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{	// Store pointer to Win32Window in user data area
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(((LPCREATESTRUCT)lParam)->lpCreateParams));
		return 0;
	}

	// look up window instance
	// note: it is possible to get a WM_SIZE before WM_CREATE
	RenderWindow* win = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!win)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	//LogManager* log = LogManager::getSingletonPtr();
	//Iterator of all listeners registered to this RenderWindow
	WindowEventListeners::iterator start = _msListeners.lower_bound(win),
						end = _msListeners.upper_bound(win);

	switch( uMsg )
	{
	case WM_ACTIVATE:
	{
		bool active = (LOWORD(wParam) != WA_INACTIVE);
		win->setActive( active );
		for( ; start != end; ++start )
			(start->second)->windowFocusChange(win);
		break;
	}
	case WM_SYSKEYDOWN:
		switch( wParam )
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU: //ALT
			//return zero to bypass defProc and signal we processed the message
			return 0;
		}
		break;
	case WM_SYSKEYUP:
		switch( wParam )
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU: //ALT
			//return zero to bypass defProc and signal we processed the message
			return 0;
		}
		break;
	case WM_ENTERSIZEMOVE:
		//log->logMessage("WM_ENTERSIZEMOVE");
		break;
	case WM_EXITSIZEMOVE:
		//log->logMessage("WM_EXITSIZEMOVE");
		break;
	case WM_MOVE:
		//log->logMessage("WM_MOVE");
		win->windowMovedOrResized();
		for( ; start != end; ++start )
			(start->second)->windowMoved(win);
		break;
	case WM_SIZE:
		//log->logMessage("WM_SIZE");
		win->windowMovedOrResized();
		for( ; start != end; ++start )
			(start->second)->windowResized(win);
		break;
	case WM_GETMINMAXINFO:
		// Prevent the window from going smaller than some minimu size
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
		break;
	case WM_CLOSE:
		//log->logMessage("WM_CLOSE");
		win->destroy();
		for( ; start != end; ++start )
			(start->second)->windowClosed(win);
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
//--------------------------------------------------------------------------------//
void GLXProc( const XEvent &event )
{
	//We have to find appropriate window based on window id ( kindof hackish :/,
	//but at least this only happens when there is a Window's event - and not that often
	WindowEventUtilities::Windows::iterator i = WindowEventUtilities::_msWindows.begin(),
						e = WindowEventUtilities::_msWindows.end();
	RenderWindow* win = 0;
	for(; i != e; ++i )
	{
		std::size_t wind = 0;
		(*i)->getCustomAttribute("WINDOW", &wind);
		if( event.xany.window == wind )
		{
			win = *i;
			break;
		}
	}

	//Sometimes, seems we get other windows, so just ignore
	if( win == 0 ) return;

	//Now that we have the correct RenderWindow for the generated Event, get an iterator for the listeners
	WindowEventUtilities::WindowEventListeners::iterator 
		start = WindowEventUtilities::_msListeners.lower_bound(win),
		end   = WindowEventUtilities::_msListeners.upper_bound(win);

	switch(event.type)
	{
	case ClientMessage:
	{
		::Atom atom;
		win->getCustomAttribute("ATOM", &atom);
		if(event.xclient.format == 32 && event.xclient.data.l[0] == (long)atom)
		{	//Window Closed (via X button)
			//Send message first, to allow app chance to unregister things that need done before
			//window is shutdown
			for( ; start != end; ++start )
				(start->second)->windowClosed(win);
			win->destroy();
		}
		break;
	}
	case ConfigureNotify:	//Moving or Resizing
		unsigned int width, height, depth;
		int left, top;
		win->getMetrics(width, height, depth, left, top);

		//determine if moving or sizing:
		if( left == event.xconfigure.x && top == event.xconfigure.y )
		{	//Resize width, height
			win->windowMovedOrResized();
			for( ; start != end; ++start )
				(start->second)->windowResized(win);
		}
		else if( width == event.xconfigure.width && height == event.xconfigure.height )
		{	//Moving x, y
			win->windowMovedOrResized();
			for( ; start != end; ++start )
				(start->second)->windowMoved(win);
		}
		break;
	case MapNotify:   //Restored
		win->setActive( true );
		for( ; start != end; ++start )
			(start->second)->windowFocusChange(win);
		break;
	case UnmapNotify: //Minimised
		win->setActive( false );
		win->setVisible( false );
		for( ; start != end; ++start )
			(start->second)->windowFocusChange(win);
		break;
	case VisibilityNotify:
		switch(event.xvisibility.state)
		{
		case VisibilityUnobscured:
			win->setActive( true );
			win->setVisible( true );
			break;
		case VisibilityPartiallyObscured:
			win->setActive( true );
			win->setVisible( true );
			break;
		case VisibilityFullyObscured:
			win->setActive( false );
			win->setVisible( false );
			break;
		}

		//Notify Listeners that focus of window has changed in some way
		for( ; start != end; ++start )
			(start->second)->windowFocusChange(win);
		break;
	default:
		break;
	} //End switch event.type
}
#endif
