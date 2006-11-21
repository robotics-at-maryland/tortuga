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
#include "OISInputManager.h"
#include "OISException.h"
#include <sstream>

//Bring in correct Header / InputManager for current build platform
#if defined OIS_SDL_PLATFORM
#  include "SDL/SDLInputManager.h"
#elif defined OIS_WIN32_PLATFORM
#  include "win32/Win32InputManager.h"
#elif defined OIS_LINUX_PLATFORM
#  include "linux/LinuxInputManager.h"
#elif defined OIS_APPLE_PLATFORM
#  include "mac/MacInputManager.h"
#elif defined OIS_XBOX_PLATFORM
#  include "xbox/XBoxInputManager.h"
#endif

using namespace OIS;

const char* gVersionName = OIS_VERSION_NAME;

//----------------------------------------------------------------------------//
unsigned int InputManager::getVersionNumber()
{
	return OIS_VERSION;
}

//----------------------------------------------------------------------------//
const char* InputManager::getVersionName()
{
	return gVersionName;
}

//----------------------------------------------------------------------------//
InputManager* InputManager::createInputSystem( std::size_t windowhandle )
{
	ParamList pl;
	std::ostringstream wnd;
	wnd << windowhandle;
	pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));

	return createInputSystem( pl );
}

//----------------------------------------------------------------------------//
InputManager* InputManager::createInputSystem( ParamList &paramList )
{
	InputManager* im = 0;

#if defined OIS_SDL_PLATFORM
	im = new SDLInputManager();
#elif defined OIS_WIN32_PLATFORM
	im = new Win32InputManager();
#elif defined OIS_XBOX_PLATFORM
	im = new XBoxInputManager();
#elif defined OIS_LINUX_PLATFORM
	im = new LinuxInputManager();
#elif defined OIS_APPLE_PLATFORM
	im = new MacInputManager();
#else
	OIS_EXCEPT(E_General, "No platform library.. check build platform defines!");
#endif 

	try
	{
		im->_initialize(paramList);
	}
	catch(...)
	{
		delete im;
		throw; //rethrow
	}

	return im;
}

//----------------------------------------------------------------------------//
void InputManager::destroyInputSystem(InputManager* manager)
{
	delete manager;
}
