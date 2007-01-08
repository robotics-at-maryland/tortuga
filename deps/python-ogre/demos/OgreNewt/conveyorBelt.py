#include ".\conveyorbelt.h"
import Ogre
import OgreNewt

class conveyorBelt():
    def __init__ (self):
        self.bodies = []    
    def __del__ (self):
        if self.Body:
            pass
            #self.killme()
    
    def init( self, name,  mgr,  world,  size, direction, speed,
                            pos,  orient,  conveyorMat,  conveyorType ):
        ## build a conveyor belt object.  first create the basic visual object.
        self.sceneManager = mgr
        self.mDir = direction
        self.Speed = speed
    
        self.Node = mgr.getRootSceneNode().createChildSceneNode()
    
        ent = self.sceneManager.createEntity(name, "box.mesh")
        ent.setMaterialName(name)
        ent.setNormaliseNormals(True)
        self.Node.attachObject(ent)
        self.Node.setScale( size )
    
        ## create the collision object for the conveyor belt.
        col = OgreNewt.Box( world, size )
        self.Body = OgreNewt.Body( world, col, conveyorType )
        del col
    
        self.Body.setMassMatrix( 0.0, Ogre.Vector3(0,0,0) )
        self.Body.attachToNode( self.Node )
        self.Body.setMaterialGroupID( conveyorMat )
        self.Body.setUserData( self )
        self.Body.setPositionOrientation( pos, orient )
        
        self.bodies.append( self.Body )
        
        self.Node.setPosition( pos )
        self.Node.setOrientation( orient )
    
    
    def getGlobalDir(self):
        ret = self.Node.getWorldOrientation() * self.mDir
        return ret
    
    def killme(self):
        ## first destroy the rigid body.
        del self.Body
        self.Body = None
    
        ##now destroy the scene node and entity.
        while (self.Node.numAttachedObjects() > 0):
            ent = self.Node.getAttachedObject(0)
            self.sceneManager.destroyEntity( ent )
    
        self.sceneManager.getRootSceneNode().removeAndDestroyChild( self.Node.getName() )
    
        self.Node = None
        


    