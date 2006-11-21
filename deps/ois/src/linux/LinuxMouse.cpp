/*
The zlib/libpng License

Copyright (c) 2006 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered friosom any source distribution.
*/
#include "linux/LinuxMouse.h"
#include "linux/LinuxInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace OIS;

//-------------------------------------------------------------------//
LinuxMouse::LinuxMouse(InputManager* creator, bool buffered, bool grab, bool hide)
{
	mCreator = creator;
	mBuffered = buffered;
	mType = OISMouse;
	listener = 0;
	display = 0;
	window = 0;
	cursor = 0;

	grabMouse = grab;
	hideMouse = hide;
}

//-------------------------------------------------------------------//
void LinuxMouse::_initialize()
{
	//Clear old state
	mState.clear();
	mMoved  = false;
	mWarped = false;

	oldXMouseX = oldXMouseY = XMouseX = XMouseY = 6;
	oldXMouseZ = 0;

	if( display ) XCloseDisplay(display);
	display = 0;
	window = static_cast<LinuxInputManager*>(mCreator)->_getWindow();

	//Create our local X listener connection
	if( !(display = XOpenDisplay(0)) )
		OIS_EXCEPT(E_General, "LinuxMouse::_initialize >> Error opening X!");

	//Set it to recieve Mouse Input events
	if( XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | PointerMotionMask) == BadWindow )
		OIS_EXCEPT(E_General, "LinuxMouse::_initialize >> X error!");

	//Warp mouse inside window
	XWarpPointer(display,None,window,0,0,0,0, 6,6);

	//Create a blank cursor:
	Pixmap bm_no;
	XColor black, dummy;
	Colormap colormap;
	static char no_data[] = { 0,0,0,0,0,0,0,0 };

	colormap = DefaultColormap( display, DefaultScreen(display) );
	XAllocNamedColor( display, colormap, "black", &black, &dummy );
	bm_no = XCreateBitmapFromData( display, window, no_data, 8, 8 );
	cursor = XCreatePixmapCursor( display, bm_no, bm_no, &black, &black, 0, 0 );

	grab( grabMouse );
	hide( hideMouse );

	mouseFocusLost = false;
}

//-------------------------------------------------------------------//
LinuxMouse::~LinuxMouse()
{
	if( display )
	{
		grab(false);
		hide(false);
		XFreeCursor(display, cursor);
		XCloseDisplay(display);
	}
}

//-------------------------------------------------------------------//
void LinuxMouse::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//-------------------------------------------------------------------//
void LinuxMouse::capture()
{
	//Clear out last frames values
	mState.X.rel = 0;
	mState.Y.rel = 0;
	mState.Z.rel = 0;

	_processXEvents();

	mWarped = false;

	if( mMoved == true )
	{
		//Compute this frames Relative X & Y motion
		mState.X.rel = XMouseX - oldXMouseX;
		mState.Y.rel = XMouseY - oldXMouseY;
		
		//Store old values for next time to compute relative motion
		oldXMouseX = XMouseX;
		oldXMouseY = XMouseY;

		mState.X.abs += mState.X.rel;
		mState.Y.abs += mState.Y.rel;
		mState.Z.abs += mState.Z.rel;

		if( grabMouse )
		{
			//We only need to clip when we are warping mouse around
			if( mState.X.abs < 0 ) mState.X.abs = 0;
			else if( mState.X.abs > mState.width ) mState.X.abs = mState.width;

			if( mState.Y.abs < 0 ) mState.Y.abs = 0;
			else if( mState.Y.abs > mState.height ) mState.Y.abs = mState.height;

			if( mouseFocusLost == false )
			{
				//Keep mouse in window (fudge factor)
				if(XMouseX<5 || XMouseX>mState.width-5 || XMouseY<5 || XMouseY>mState.height-5 )
				{
					oldXMouseX = XMouseX = mState.width >> 1;
					oldXMouseY = XMouseY = mState.height >> 1;
					XWarpPointer(display,None,window,0,0,0,0,XMouseX, XMouseY);
					mWarped = true;
				}
			}
		}

		if( mBuffered && listener )
			listener->mouseMoved( MouseEvent( this, mState ) );

		mMoved = false;
	}

	//Check for losing/gaining mouse grab focus (alt-tab, etc)
	if( grabMouse )	{
		if( static_cast<LinuxInputManager*>(mCreator)->_getGrabState() ) {
			if( mouseFocusLost ) { //We just regained mouse grab focus
				grab( true );
				hide( hideMouse );
				mouseFocusLost = false;
			}
		}
		else {
			if( mouseFocusLost == false ) {//We just lost mouse grab focus
				grab( false );
				hide( false );
				mouseFocusLost = true;
			}
		}
	}
}

//-------------------------------------------------------------------//
void LinuxMouse::_processXEvents()
{
	//X11 Button Events: 1=left 2=middle 3=right; Our Bit Postion: 1=Left 2=Right 3=Middle
	char mask[4] = {0,1,4,2};
	XEvent event;

	//Poll x11 for events mouse events
	while( XPending(display) > 0 ) 
	{

		XNextEvent(display, &event);
		if( event.type == MotionNotify )
		{
			XMouseX = event.xmotion.x;
			XMouseY = event.xmotion.y;

			//Mouse can sometimes get bad data when warping,
			// so compensate (kindof a hack.. happens with long delays between capture's)
			if( mWarped && grabMouse )
			{
				if(XMouseX<5 || XMouseX>mState.width-5) {
					XMouseX = oldXMouseX;
					continue;
				}
				else if(XMouseY<5 || XMouseY>mState.height-5) {
					XMouseY = oldXMouseY;
					continue;
				}
			}

			mMoved = true;
		}
		else if( event.type == ButtonPress )
		{
			static_cast<LinuxInputManager*>(mCreator)->_setGrabState(true);

			if( event.xbutton.button < 4 )
			{
				mState.buttons |= mask[event.xbutton.button];
				if( mBuffered && listener )
					if( listener->mousePressed( MouseEvent( this, mState ),
						(MouseButtonID)(mask[event.xbutton.button] >> 1)) == false )
						return;
			}
		}
		else if( event.type == ButtonRelease )
		{
			if( event.xbutton.button < 4 )
			{
				mState.buttons &= ~mask[event.xbutton.button];
				if( mBuffered && listener )
					if( listener->mouseReleased( MouseEvent( this, mState ),
						(MouseButtonID)(mask[event.xbutton.button] >> 1)) == false )
						return;
			}
			//The Z axis gets pushed/released pair message (this is up)
			else if( event.xbutton.button == 4 )
			{
				mState.Z.rel += 120;
				mMoved = true;
			}
			//The Z axis gets pushed/released pair message (this is down)
			else if( event.xbutton.button == 5 )
			{
				mState.Z.rel -= 120;
				mMoved = true;
			}
		}
	}
}

//-------------------------------------------------------------------//
void LinuxMouse::grab(bool grab)
{
	if( grab )
		XGrabPointer(display, window, True, 0, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
	else
		XUngrabPointer(display, CurrentTime);
}

//-------------------------------------------------------------------//
void LinuxMouse::hide(bool hide)
{
	if( hide )
		XDefineCursor(display, window, cursor);
	else
		XUndefineCursor(display, window);
}
