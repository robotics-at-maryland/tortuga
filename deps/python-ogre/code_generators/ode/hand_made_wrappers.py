import os
import environment

WRAPPER_DEFINITION_dBody=\
"""
::dReal
dBody_getPosRelPoint ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.getPosRelPoint( x, y, z, &returnValue);
    return returnValue;
}

::dReal
dBody_getRelPointPos ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.getRelPointPos( x, y, z, &returnValue);
    return returnValue;
}

::dReal
dBody_getPointVel ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.getPointVel( x, y, z, &returnValue);
    return returnValue;
}

::dReal
dBody_vectorFromWorld ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.vectorFromWorld( x, y, z, &returnValue);
    return returnValue;
}

::dReal
dBody_vectorToWorld ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.vectorToWorld( x, y, z, &returnValue);
    return returnValue;
}
::dReal
dBody_getRelPointVel ( ::dBody & body, ::dReal x,::dReal y,::dReal z ) {
    static ::dReal returnValue;
    body.getRelPointVel( x, y, z, &returnValue);
    return returnValue;
}
        
::dReal
dBody_getFiniteRotationAxis ( ::dBody & body ) {
    static ::dReal returnValue;
    body.getFiniteRotationAxis( &returnValue);
    return returnValue;
}


boost::python::tuple
dBody_getPosition(::dBody & body) {
  const ::dReal * ret = body.getPosition();
   
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
}

boost::python::tuple
dBody_getRotation(::dBody & body){
    const ::dReal * ret = body.getRotation(); 
    return boost::python::make_tuple(
                ret[0], ret[1], ret[2], ret[3], 
                ret[4], ret[5], ret[6], ret[7],
                ret[8], ret[9], ret[10], ret[11]); 
    
    }
    
boost::python::tuple
dBody_getQuaternion(::dBody & body){
  const ::dReal * ret = body.getQuaternion (); 
  return boost::python::make_tuple(ret[0], ret[1], ret[2], ret[3]); 
    }
    
boost::python::tuple
dBody_getLinearVel(::dBody & body){
  const ::dReal * ret = body.getLinearVel (); 
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
    }
boost::python::tuple
dBody_getAngularVel(::dBody & body) {
  const ::dReal * ret = body.getAngularVel (); 
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
    }
boost::python::tuple
dBody_getForce(::dBody & body){
  const ::dReal * ret = body.getForce(); 
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
    }
boost::python::tuple
dBody_getTorque(::dBody & body){
  const ::dReal * ret = body.getTorque(); 
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
    }

"""        


WRAPPER_DEFINITION_dGeom=\
"""
boost::python::tuple
dBody_getPosition(::dGeom & geom) {
  const ::dReal * ret = geom.getPosition();
   
  return boost::python::make_tuple(ret[0], ret[1], ret[2]); 
}

boost::python::tuple
dBody_getRotation(::dGeom & geom){
    const ::dReal * ret = geom.getRotation(); 
    return boost::python::make_tuple(
                ret[0], ret[1], ret[2], ret[3], 
                ret[4], ret[5], ret[6], ret[7],
                ret[8], ret[9], ret[10], ret[11]); 
    
    }            
"""

WRAPPER_REGISTRATION_dBody = \
"""
    def( "getPosRelPoint", &::dBody_getPosRelPoint );
    dBody_exposer.def( "getRelPointPos", &::dBody_getRelPointPos );
    dBody_exposer.def( "getPointVel", &::dBody_getPointVel );
    dBody_exposer.def( "getRelPointVel", &::dBody_getRelPointVel );
    dBody_exposer.def( "getFiniteRotationAxis", &::dBody_getFiniteRotationAxis );
    dBody_exposer.def( "vectorFromWorld", &::dBody_vectorFromWorld );
    dBody_exposer.def( "vectorToWorld", &::dBody_vectorToWorld );
    
    dBody_exposer.def( "getPosition", &::dBody_getPosition);
    dBody_exposer.def( "getRotation", &::dBody_getRotation);
    dBody_exposer.def( "getQuaternion", &::dBody_getQuaternion);
    dBody_exposer.def( "getLinearVel", &::dBody_getLinearVel);
    dBody_exposer.def( "getAngularVel", &::dBody_getAngularVel);
    dBody_exposer.def( "getForce", &::dBody_getForce);
    dBody_exposer.def( "getTorque", &::dBody_getTorque)
"""
WRAPPER_REGISTRATION_dGeom = \
"""
    def( "getPosition", &::dBody_getPosition);
    dGeom_exposer.def( "getRotation", &::dBody_getRotation);
"""


def apply( mb ):
#     pass
    cs = mb.class_( 'dBody' )
    cs.add_declaration_code( WRAPPER_DEFINITION_dBody )
    cs.add_registration_code( WRAPPER_REGISTRATION_dBody )
    cs = mb.class_( 'dGeom' )
    cs.add_declaration_code( WRAPPER_DEFINITION_dGeom )
    cs.add_registration_code( WRAPPER_REGISTRATION_dGeom )
    