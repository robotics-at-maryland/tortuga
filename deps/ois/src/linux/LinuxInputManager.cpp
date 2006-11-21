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

    3. This notice may not be removed or altered from any source distribution.
*/
#include "linux/LinuxInputManager.h"
#include "linux/LinuxKeyboard.h"
#include "linux/LinuxJoyStickEvents.h"
#include "linux/LinuxMouse.h"
#include "OISException.h"

using namespace OIS;

const std::string LinuxInputManager::iName = "X11 Linux Input Manager";
//--------------------------------------------------------------------------------//
LinuxInputManager::LinuxInputManager()
{
	window = 0;

	//Default settings
	grabMouse = true;
	grabKeyboard = true;
	hideMouse = true;
	mGrabs = true;
	useXRepeat = false;
}

//--------------------------------------------------------------------------------//
LinuxInputManager::~LinuxInputManager()
{
	LinuxJoyStick::_clearJoys(unusedJoyStickList);
}

//--------------------------------------------------------------------------------//
void LinuxInputManager::_initialize( ParamList &paramList )
{
	_parseConfigSettings( paramList );

	//Enumerate all devices attached
	_enumerateDevices();
}

//--------------------------------------------------------------------------------//
void LinuxInputManager::_parseConfigSettings( ParamList &paramList )
{
	ParamList::iterator i = paramList.find("WINDOW");
	if( i == paramList.end() ) 
		OIS_EXCEPT( E_InvalidParam, "LinuxInputManager >> No WINDOW!" );

	//TODO 64 bit proof this little conversion xxx wip
	window  = strtoul(i->second.c_str(), 0, 10);

	//--------- Keyboard Settings ------------//
	i = paramList.find("XAutoRepeatOn");
	if( i != paramList.end() )
		if( i->second == "true" )
			useXRepeat = true;

	i = paramList.find("x11_keyboard_grab");
	if( i != paramList.end() )
		if( i->second == "false" )
			grabKeyboard = false;

	//--------- Mouse Settings ------------//
	i = paramList.find("x11_mouse_grab");
	if( i != paramList.end() )
		if( i->second == "false" )
			grabMouse = false;

	i = paramList.find("x11_mouse_hide");
	if( i != paramList.end() )
		if( i->second == "false" )
			hideMouse = false;
}

//--------------------------------------------------------------------------------//
void LinuxInputManager::_enumerateDevices()
{
	//Enumerate all attached devices
	unusedJoyStickList = LinuxJoyStick::_scanJoys();
	joySticks = unusedJoyStickList.size();
}

//--------------------------------------------------------------------------------//
int LinuxInputManager::numJoysticks()
{
	return joySticks;
}

//--------------------------------------------------------------------------------//
int LinuxInputManager::numMice()
{
	return 1;
}

//--------------------------------------------------------------------------------//
int LinuxInputManager::numKeyBoards()
{
	return 1;
}

//----------------------------------------------------------------------------//
Object* LinuxInputManager::createInputObject( Type iType, bool bufferMode )
{
	Object* obj = 0;
	
	switch( iType )
	{
		case OISKeyboard: 
			obj = new LinuxKeyboard(this, bufferMode, grabKeyboard, useXRepeat); 
			break;
		case OISMouse: 
			obj = new LinuxMouse(this, bufferMode, grabMouse, hideMouse); 
			break;
		case OISJoyStick: 
		{
			//Find a JoyStick not in use
			JoyStickInfoList::iterator i = unusedJoyStickList.begin();
			if( i != unusedJoyStickList.end() )
			{
				obj = new LinuxJoyStick(bufferMode, *i);
				unusedJoyStickList.erase(i);
				break;
			}
			OIS_EXCEPT(E_InputDeviceNonExistant, "No JoySticks Available!");
		}
		default: 
			OIS_EXCEPT( E_InputDeviceNotSupported, "Device Not Supported!");
	}

	//Seperate initialization from construction.. as bad things happen when
	//throwing exceptions from constructors
	try
	{
		obj->_initialize();
	}
	catch(...) 
	{
		delete obj;
		throw; //rethrow the exception
	}

	return obj;
}

//----------------------------------------------------------------------------//
void LinuxInputManager::destroyInputObject( Object* obj )
{
	if( obj )
	{
		//We put joys back into unused list
		if( obj->type() == OISJoyStick )
			unusedJoyStickList.push_back(((LinuxJoyStick*)obj)->_getJoyInfo());

		delete obj;
	}
}
