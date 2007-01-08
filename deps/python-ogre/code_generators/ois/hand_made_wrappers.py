import os
import environment

WRAPPER_DEFINITION_InputManager = \
"""
OIS::Keyboard * createInputObjectKeyboard(OIS::InputManager& im, OIS::Type iType, bool bufferMode ) {
	OIS::Keyboard * mKeyboard = (OIS::Keyboard * ) im.createInputObject( iType, bufferMode );
	return mKeyboard;
	}
OIS::Mouse * createInputObjectMouse(OIS::InputManager& im, OIS::Type iType, bool bufferMode ) {
	OIS::Mouse * mMouse = (OIS::Mouse * ) im.createInputObject( iType, bufferMode );
	return mMouse;
	}
OIS::JoyStick * createInputObjectJoyStick(OIS::InputManager& im, OIS::Type iType, bool bufferMode ) {
	OIS::JoyStick * mJoyStick = (OIS::JoyStick * ) im.createInputObject( iType, bufferMode );
	return mJoyStick;
	}
void destroyInputObjectKeyboard( OIS::InputManager& im, OIS::Keyboard* obj ) {
     im.destroyInputObject( (OIS::Object*) obj );
     }
void destroyInputObjectMouse( OIS::InputManager& im, OIS::Mouse* obj ) {
     im.destroyInputObject( (OIS::Object*) obj );
     }
void destroyInputObjectJoyStick( OIS::InputManager& im, OIS::JoyStick* obj ) {
     im.destroyInputObject( (OIS::Object*) obj );
     }
"""
     
WRAPPER_DEFINITION_General = \
"""
OIS::InputManager * createPythonInputSystem( size_t windowHnd ) {
// Wrapper to manage creating input system without using a multimap
// TODO:  Change this to accept a python list and extract this to the Paramlist
		OIS::ParamList pl;	
		std::ostringstream windowHndStr;
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		return OIS::InputManager::createInputSystem( pl );
		//OIS::InputManager &im = *OIS::InputManager::createInputSystem( pl );
		//return im;
		}

"""


WRAPPER_REGISTRATION_InputManager = \
"""
    def( "createInputObjectKeyboard", &createInputObjectKeyboard,  bp::return_value_policy< bp::reference_existing_object, bp::default_call_policies >() );
    InputManager_exposer.def( "createInputObjectMouse", &createInputObjectMouse,  bp::return_value_policy< bp::reference_existing_object, bp::default_call_policies >() );
    InputManager_exposer.def( "createInputObjectJoyStick", &createInputObjectJoyStick,  bp::return_value_policy< bp::reference_existing_object, bp::default_call_policies >() );
    InputManager_exposer.def( "destroyInputObjectKeyboard", &destroyInputObjectKeyboard);
    InputManager_exposer.def( "destroyInputObjectMouse", &destroyInputObjectMouse);
    InputManager_exposer.def( "destroyInputObjectJoyStick", &destroyInputObjectJoyStick);

"""
WRAPPER_REGISTRATION_General = \
"""   
def( "createPythonInputSystem", &createPythonInputSystem, bp::return_value_policy< bp::reference_existing_object, bp::default_call_policies >());
"""

def apply( mb ):
    rt = mb.class_( 'InputManager' )
    rt.add_declaration_code( WRAPPER_DEFINITION_InputManager )
    rt.add_registration_code( WRAPPER_REGISTRATION_InputManager )
    
    mb.add_declaration_code( WRAPPER_DEFINITION_General )
    mb.add_registration_code( WRAPPER_REGISTRATION_General )

