#ifndef OIS_MacMouse_H
#define OIS_MacMouse_H

#include "OISMouse.h"
#include "mac/MacHelpers.h"
#include "mac/MacPrereqs.h"

#include <Carbon/Carbon.h>

namespace OIS
{
	class MacMouse : public Mouse
    {
	public:
		MacMouse( InputManager* creator, bool buffered );
		virtual ~MacMouse();
		
		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered);

		/** @copydoc Object::capture */
		virtual void capture();

		/** @copydoc Object::queryInterface */
		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		/** @copydoc Object::_initialize */
		virtual void _initialize();
        
	public:
        void _mouseButtonCallback( EventRef theEvent );
        void _mouseMoveCallback( EventRef theEvent );
        void _mouseScrollCallback( EventRef theEvent );

	protected:
		MacMouse() {}

        // "universal procedure pointers" - required reference for callbacks
        EventHandlerUPP mouseButtonUPP;
        EventHandlerUPP mouseMoveUPP;
        EventHandlerUPP mouseScrollUPP;
        
        // so we can delete the handlers on destruction
        EventHandlerRef mouseButtonEventRef;
        EventHandlerRef mouseMoveEventRef;
        EventHandlerRef mouseScrollEventRef;

		bool mRegainFocus;
	};
}


#endif // OIS_MacMouse_H
