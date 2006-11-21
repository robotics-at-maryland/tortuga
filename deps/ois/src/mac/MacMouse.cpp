#include "mac/MacMouse.h"
#include "mac/MacInputManager.h"
#include "mac/MacHelpers.h"
#include "OISException.h"
#include "OISEvents.h"

#include <Carbon/Carbon.h>

#include <list>
#include <string>

using namespace OIS;

//-------------------------------------------------------------------//
MacMouse::MacMouse( InputManager* creator, bool buffered ) : mRegainFocus( false )
{
	mCreator = creator;
	mouseButtonEventRef = NULL;
	mouseMoveEventRef = NULL;
	mouseScrollEventRef = NULL;
	
	mBuffered = buffered;
	mType = OISMouse;
	listener = 0;
	
	mouseButtonUPP = NewEventHandlerUPP( MouseButtonWrapper );
	mouseMoveUPP = NewEventHandlerUPP( MouseMoveWrapper );
	mouseScrollUPP = NewEventHandlerUPP( MouseScrollWrapper );
}

MacMouse::~MacMouse()
{
	if( mouseButtonEventRef != NULL ) RemoveEventHandler( mouseButtonEventRef );
	if( mouseMoveEventRef != NULL ) RemoveEventHandler( mouseMoveEventRef );
	if( mouseScrollEventRef != NULL ) RemoveEventHandler( mouseScrollEventRef );
	
	DisposeEventHandlerUPP( mouseButtonUPP );
	DisposeEventHandlerUPP( mouseMoveUPP );
	DisposeEventHandlerUPP( mouseScrollUPP );
}

void MacMouse::_initialize()
{
	mState.clear();
	mRegainFocus = false;

	OSStatus status;
	WindowRef window = (( MacInputManager* )mCreator)->_getWindow();
	
	if( mouseButtonEventRef != NULL ) RemoveEventHandler( mouseButtonEventRef );
	if( mouseMoveEventRef != NULL ) RemoveEventHandler( mouseMoveEventRef );
	if( mouseScrollEventRef != NULL ) RemoveEventHandler( mouseScrollEventRef );

	mouseButtonEventRef = NULL;
	mouseMoveEventRef = NULL;
	mouseScrollEventRef = NULL;

	const EventTypeSpec mouseButtonEvents[] =
	{
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp }
	};

	const EventTypeSpec mouseMoveEvents[] =
	{
		{ kEventClassMouse, kEventMouseMoved }
	};

	const EventTypeSpec mouseScrollEvents[] =
	{
		{ kEventClassMouse, kEventMouseWheelMoved }
	};
	
	status = InstallWindowEventHandler( window, mouseButtonUPP, GetEventTypeCount( mouseButtonEvents ), mouseButtonEvents, this, &mouseButtonEventRef );
	if( status != noErr )
	{
		OIS_EXCEPT( E_General, "MacMouse::_initialize >> Error loading MouseButton event handler" );
	}

	status = InstallWindowEventHandler( window, mouseMoveUPP, GetEventTypeCount( mouseMoveEvents ), mouseMoveEvents, this, &mouseMoveEventRef );
	if( status != noErr )
	{
		OIS_EXCEPT( E_General, "MacMouse::_initialize >> Error loading MouseMove event handler" );
	}

	status = InstallWindowEventHandler( window, mouseScrollUPP, GetEventTypeCount( mouseScrollEvents ), mouseScrollEvents, this, &mouseScrollEventRef );
	if( status != noErr )
	{
		OIS_EXCEPT( E_General, "MacMouse::_initialize >> Error loading MouseScroll event handler" );
	}
}

void MacMouse::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void MacMouse::capture()
{
	if( !mBuffered )
		return;
	
//	eventStack::iterator cur_it;
}

void MacMouse::_mouseButtonCallback( EventRef theEvent )
{

}

void MacMouse::_mouseMoveCallback( EventRef theEvent )
{

}

void MacMouse::_mouseScrollCallback( EventRef theEvent )
{

}
