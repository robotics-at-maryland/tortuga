#include ".\conveyormatcallback.h"
#include ".\conveyorBelt.h"
import Ogre
import OgreNewt

class conveyorMatCallback ( OgreNewt.ContactCallback ):

    def __init__ ( self, conveyorID ) :
        OgreNewt.ContactCallback.__init__(self) 
        self.ConveyorID = conveyorID
    
            
    def userProcess(self):
        ## first, find which body represents the conveyor belt!
        if (self.m_body0.getType() == self.ConveyorID):
            belt = self.m_body0.getUserData()
            object = self.m_body1
    
        if (self.m_body1.getType() == self.ConveyorID):
            belt = self.m_body1.getUserData()
            object = self.m_body0
        if not belt:
            return 0
        ## okay, found the belt... let's adjust the collision based on this.
        thedir = belt.getGlobalDir()
        
        self.rotateTangentDirections( thedir )
        result_accel = (thedir * belt.Speed) - object.getVelocity()
        
        self.setContactTangentAcceleration( result_accel.length(), 0 )
    
        return 1
