import os
import environment

WRAPPER_REGISTRATION_General=\
"""
    boost::python::def( "System_Create", &::General_FMOD_System_Create,
         bp::return_value_policy< bp::reference_existing_object >());
         
    boost::python::def( "CastVoidPtr", &Utility_CastVoidPtr,
                bp::return_value_policy< bp::return_opaque_pointer >());
"""



WRAPPER_DEFINITION_General=\
"""
// lets try this as a global static
FMOD::System   static  * Main_system;

//Create the System
FMOD::System * General_FMOD_System_Create( void ) 
    {
    FMOD_RESULT       result;
    result = FMOD::System_Create(&Main_system);
    return Main_system;
    }

// sometimes we need to take the ctypess addressof(), an int, and recast it as a general void *
void *
Utility_CastVoidPtr ( int address )
    {
    return (void *) address;
    }
"""
WRAPPER_REGISTRATION_Sound=\
"""
    def( "release", &::System_FMOD_Sound_release );    
"""
WRAPPER_DEFINITION_Sound=\
"""
void System_FMOD_Sound_release( FMOD::Sound & me ) {
    me.release();
    }
"""

WRAPPER_REGISTRATION_System=\
"""
    def( "update", &::System_FMOD_System_update );    
    System_exposer.def( "init", &::System_FMOD_System_init );    
    System_exposer.def( "release", &::System_FMOD_System_release );    
    System_exposer.def( "close", &::System_FMOD_System_close );    
"""
WRAPPER_DEFINITION_System=\
"""
void System_FMOD_System_update( FMOD::System & me ) {
    me.update();
    }
void System_FMOD_System_init ( FMOD::System & me, int channels, FMOD_INITFLAGS flags, void * extra ) {
    me.init(channels, flags, extra);
} 
void System_FMOD_System_release( FMOD::System & me ) { 
    me.release();
    }
void System_FMOD_System_close( FMOD::System & me ) {
    me.close();
    }
"""
def apply( mb ):

    mb.add_declaration_code( WRAPPER_DEFINITION_General )
    mb.add_registration_code( WRAPPER_REGISTRATION_General )
    ns = mb.class_( 'System' )
    ns.add_declaration_code( WRAPPER_DEFINITION_System )
    ns.add_registration_code( WRAPPER_REGISTRATION_System )
    ns = mb.class_( 'Sound' )
    ns.add_declaration_code( WRAPPER_DEFINITION_Sound )
    ns.add_registration_code( WRAPPER_REGISTRATION_Sound )
