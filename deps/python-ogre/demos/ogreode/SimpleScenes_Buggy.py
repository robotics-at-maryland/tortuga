# /*
# SimpleScenes_Buggy.h
# -------------------
# A reimplementation of the ODE test Buggy application using
# the OgreOde wrapper for Ogre
# */
from SimpleScenes import *
import Ogre as ogre
import OgreOde
import OIS


# ##------------------------------------------------------------------------------------------------



# /*
# Buggy test extends from the base test class
# */
class SimpleScenes_Buggy ( SimpleScenes ):
    ## Constructor also needs to create the scene
    def __init__ ( self, world):
        SimpleScenes.__init__(self, world)
        self.sSelectedcar = 1
        self.maxNumCar = 3
        ## Keep track of the things we need to delete manually or change according to user input
        self.vehicle = None # OgreOde.Vehicle()
        self._drive = ""
        self.carNames = ["Jeep","JeepSway", "Subaru"]
        self.carFileNames = ["jeep.ogreode","jeep.ogreode", "subaru.ogreode"]
        ## Create the self.vehicle from the config file
        self.changecar()
    
        ## Move the self.vehicle
        self.vehicle.setPosition(ogre.Vector3(0,0.82898,0))

        ## Initially (i.e. in the config file) it's rear wheel drive
        self._drive = 'R'
        self.setInfoText(self.carNames[self.sSelectedcar] + "Rear wheel drive")

        ## Create a box to jump over, the visual version
        entity = self._mgr.createEntity("Jump", "crate.mesh")
        entity.setNormaliseNormals(True)
        entity.setCastShadows(True)

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())
        node.attachObject(entity)
        node.setPosition(ogre.Vector3(0,0.3,-5))
        node.setOrientation(ogre.Quaternion(ogre.Radian(0.4),ogre.Vector3(1,0,0)))
        node.setScale(0.3,0.1,0.4)

        ## Create the physical version (just static geometry, it can't move so
        ## it doesn't need a body) and keep track of it
        ei = OgreOde.EntityInformer(entity,ogre.Matrix4.getScale(node.getScale()))
        geom = ei.createSingleStaticBox(self._world, self._space)
        entity.setUserObject(geom)
        self._geoms.append(geom)

        ## The self.car is what we'll want to look at
        self._last_node = self.vehicle.getSceneNode()

    ##------------------------------------------------------------------------------------------------
    ## Override the destructor since there's some special deleting to do
    def __del__ ( self ):
        del self.vehicle

        ## Destroy the jump manually since it's not associated with 
        ## any body it won't get deleted automatically
        self._mgr.destroySceneNode("Jump")
        self._mgr.destroyEntity("Jump")
        
        ## Geometries and Joints will get deleted by the base class
        SimpleScenes.__del__(self)

    ##------------------------------------------------------------------------------------------------
    ## Return our name
    def getName(self):
        return "Test Buggy"

    ##------------------------------------------------------------------------------------------------
    ## Tell the user what keys they can use
    def getKeys(self):
        return "I/K - Accelerate/Brake, J/L - Turn, X - Change drive mode, N - Change car"


    ##------------------------------------------------------------------------------------------------
    def changecar( self ):
        self.sSelectedcar = (self.sSelectedcar + 1) % self.maxNumCar

        del self.vehicle
        self.vehicle = self.dotOgreOdeLoader.loadObject (self.carFileNames[self.sSelectedcar], 
                                        self.carNames[self.sSelectedcar])


        ## Move the self.vehicle
        self.vehicle.setPosition(ogre.Vector3(0,0.82898,0))

    def frameEnded(self, time,  input,  mouse):
        ## Do default processing
        SimpleScenes.frameEnded(self,time, input, mouse)
        ## Tell the self.vehicle what digital inputs are being pressed left, right, power and brake
        ## There are equivalent methods for analogue controls, current you can't change gear so
        ## you can't reverse!
        self.vehicle.setInputs(input.isKeyDown(OIS.KC_J),input.isKeyDown(OIS.KC_L),
                    input.isKeyDown(OIS.KC_I),input.isKeyDown(OIS.KC_K))

        ## Update the self.vehicle, you need to do this every time step
        self.vehicle.update(time)
        if (self._key_delay > SimpleScenes.KEY_DELAY):

            if ((input.isKeyDown(OIS.KC_N))):
                self.changecar()
                self._key_delay = 0.0

            ## Change the drive mode between front, rear and 4wd
            if ((input.isKeyDown(OIS.KC_X))):
                if (self._drive == 'R'):
                    ## Switch from rear to front
                    self._drive = 'F'
                    self.vehicle.getWheel(0).setPowerFactor(1)
                    self.vehicle.getWheel(1).setPowerFactor(1)
                    self.vehicle.getWheel(2).setPowerFactor(0)
                    self.vehicle.getWheel(3).setPowerFactor(0)

                    self.setInfoText(self.carNames[self.sSelectedcar] + " Front wheel drive")

                    ## Switch from front to all
                elif (self._drive == 'F'):
                    self._drive = '4'

                    self.vehicle.getWheel(0).setPowerFactor(0.6)
                    self.vehicle.getWheel(1).setPowerFactor(0.6)
                    self.vehicle.getWheel(2).setPowerFactor(0.4)
                    self.vehicle.getWheel(3).setPowerFactor(0.4)

                    self.setInfoText(self.carNames[self.sSelectedcar] + " All wheel drive")
                
                    ## Switch from all to rear
                elif (self._drive == '4'):
                    self._drive = 'R'

                    self.vehicle.getWheel(0).setPowerFactor(0)
                    self.vehicle.getWheel(1).setPowerFactor(0)
                    self.vehicle.getWheel(2).setPowerFactor(1)
                    self.vehicle.getWheel(3).setPowerFactor(1)

                    self.setInfoText(self.carNames[self.sSelectedcar] + " Rear wheel drive")
                self._key_delay = 0.0

    ##------------------------------------------------------------------------------------------------
    ## Override the collision callback to set our own parameters
    def collision(self,  contact):
        ## If the base class doesn't think the collision needs to 
        ## happen then we won't do anything either
        if (not SimpleScenes.collision(self, contact)): 
            return False

        #if ( not OgreOde_Prefab.self.vehicle.handleTyreCollision(contact)):
        if ( not self.vehicle.handleTyreCollision(contact)):
            ## Set the floor to be a bit slippy
            contact.setCoulombFriction(12.0)
        return True


