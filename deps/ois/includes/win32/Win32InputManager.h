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
#ifndef OIS_Win32InputManager_H
#define OIS_Win32InputManager_H

#include "OISInputManager.h"
#include "Win32/Win32Prereqs.h"

namespace OIS
{
	/**
		"Win32" InputManager specialization - Using DirectInput8
	*/
	class Win32InputManager : public InputManager
	{
	public:
		Win32InputManager();
		virtual ~Win32InputManager();

		/** @copydoc InputManager::inputSystemName */
		virtual const std::string& inputSystemName() { return iName; }
		
		/** @copydoc InputManager::numJoysticks */
		virtual int numJoysticks();
		/** @copydoc InputManager::numMice */
		virtual int numMice();
		/** @copydoc InputManager::numKeyBoards */
		virtual int numKeyBoards();
		
		/** @copydoc InputManager::createInputObject */
		Object* createInputObject( Type iType, bool bufferMode );
		/** @copydoc InputManager::destroyInputObject */
		void destroyInputObject( Object* obj );

		//! Returns HWND needed by DirectInput Device Object
		HWND getWindowHandle() { return hWnd; }

		/** @copydoc InputManager::_initialize */
		void _initialize( ParamList &paramList );

	protected:
		//! internal class method for dealing with param list
		void _parseConfigSettings( ParamList &paramList );
		//! internal class method for finding attached devices
		void _enumerateDevices();
		static BOOL CALLBACK _DIEnumKbdCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

		JoyStickInfoList unusedJoyStickList;

		static const std::string iName;
		HWND hWnd;

		//Direct Input Interface
		IDirectInput8* mDirectInput;

		//Used for setting device properties - as read in from paramlist
		DWORD kbSettings;
		DWORD mouseSettings;
		DWORD joySettings;

		char joySticks;
	};
}
#endif
