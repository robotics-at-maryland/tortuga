/*
 The zlib/libpng License
 
 Copyright (c) 2006 Chris Snyder 
 
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
#ifndef OIS_MacInputManager_H
#define OIS_MacInputManager_H

#include "OISInputManager.h"

#include "mac/MacPrereqs.h"

#include <Carbon/Carbon.h>

namespace OIS
{
    
    class MacInputManager : public InputManager
    {
    public:
        MacInputManager();
        virtual ~MacInputManager();
        
        virtual const std::string& inputSystemName() { return iName; }
        
        virtual int numJoysticks();
        
        virtual int numMice();
        
        virtual int numKeyBoards();
        
        Object* createInputObject( Type iType, bool bufferMode );
        void destroyInputObject( Object* obj );
        
        // method for getting window
        WindowRef _getWindow() {return window;}
        
    protected:
        void _initialize( ParamList& paramList );
        
        void _parseConfigSettings( ParamList& paramList );
        
        void _enumerateDevices();
        
        static const std::string iName;
        
        // Mac stuff
        WindowRef window;
        
        // settings
        bool hideMouse;
        bool useRepeat;
        
    
    };
}


#endif