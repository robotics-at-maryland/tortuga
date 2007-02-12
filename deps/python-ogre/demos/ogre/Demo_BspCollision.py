
## -----------------------------------------------------------------------------
## Filename:    BspCollision.py
## Description: Test the OgreRefApp module
## -----------------------------------------------------------------------------

import Ogre as ogre
import SampleFramework as sf     ## note that we are forcing the OIS sample framework
import OgreRefApp as OgreRefApp

## As this demo does it's own key input, AND we want to support Ogre 1.2.x we need to cope with the 
## change to OIS in ogre 1.3.x (current CVS version)

if ogre.OgreVersionString[:2]=="12":
    KC_SPACE = ogre.KC_SPACE
else:
    import OIS as OIS
    KC_SPACE = OIS.KC_SPACE


## Hacky globals
gBall = None
gTargetNode = None
gRsq = 0


class BspCollisionListener (sf.FrameListener):

    def __init__(self, renderWindow, camera):
        """ Initialise the frame listener"""    
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.camera = camera
        self.timeUntilNextToggle = 0
        self.MAX_TIME_INCREMENT = 0.5
        
    def _UpdateSimulation ( self, Event ):
         OgreRefApp.World.getSingleton().simulationStep(Event.timeSinceLastFrame)
           
    def frameEnded(self, evt):
        global gBall, gTargetNode, gRsq
        ## Deal with time delays that are too large
        ## If we exceed this limit, we ignore
        if evt.timeSinceLastEvent > self.MAX_TIME_INCREMENT:
            return True
        
        if self.timeUntilNextToggle >= 0: 
            self.timeUntilNextToggle -= evt.timeSinceLastFrame

        ## Call superclass
        ret = sf.FrameListener.frameEnded(self, evt)        

        if self._isToggleKeyDown(KC_SPACE):  
            self.timeUntilNextToggle = 2
            gBall.setPosition(self.camera.getPosition() + 
                self.camera.getDirection() * self.camera.getNearClipDistance() * 2)
            gBall.setLinearVelocity(self.camera.getDirection() * 200)
            gBall.setAngularVelocity(ogre.Vector3.ZERO)

        ## Move the targeter
        gRsq.setRay(self.camera.getRealCamera().getCameraToViewportRay(0.5, 0.5))
        for queryResult in gRsq.execute():
             gTargetNode.setPosition(gRsq.getRay().getPoint(queryResult.distance))
        return ret
 
class BspCollisionApplication (sf.Application):
    def __init__(self):
        "Init Render Application"
        sf.Application.__init__(self)
        self.BOX_SIZE = 15.0
        self.num_rows = 3
        
    def __del__(self):
        global gBall, gTargetNode, gRsq
        "Close Render Application"
        del ( gRsq )
        
    # Override scene manager (use indoor instead of generic)
    def _chooseSceneManager(self):
        self.sceneManager = self.root.createSceneManager("BspSceneManager")

    def _createWorld(self):
        ## Create BSP-specific world
        self.world = OgreRefApp.World(self.sceneManager, OgreRefApp.World.WT_REFAPP_BSP)
        
        
    def _createCamera(self):
        """ when doing collisions with OgreRefApp Framework the camera is linked to the 'WORLD'
        not to the scene manager"""
        self.camera = self.world.createCamera('PlayerCam')
        self.camera.setPosition( (0, 0, 500) )
        self.camera.lookAt( (0, 0, -300) )
        self.camera.NearClipDistance = 5

        
    def _createScene(self):
        global gBall, gTargetNode, gRsq
        self.sceneManager.setShadowTechnique(ogre.SHADOWTYPE_STENCIL_MODULATIVE)
        ## Set ambient light
        self.sceneManager.setAmbientLight( (0.2, 0.2, 0.2) )
        
        ## Create a point light
        l = self.sceneManager.createLight("MainLight")
        l.setPosition(-100,50,100)
        l.setAttenuation(8000,1,0,0)

        ## Setup World
        self.world.setGravity( (0, 0, -60) )
        self.world.getSceneManager().setWorldGeometry("ogretestmap.bsp")

        ## modify camera for close work
        self.camera.setNearClipDistance(10)
        self.camera.setFarClipDistance(20000)

        ## Also change position, and set Quake-type orientation
        ## Get random player start point
        vp = self.sceneManager.getSuggestedViewpoint(True)
        self.camera.setPosition(vp.position)
        self.camera.pitch(ogre.Degree(d=90)) ## Quake uses X/Y horizon, Z up
        self.camera.rotate(vp.orientation)
        ## Don't yaw along variable axis, causes leaning
        self.camera.setFixedYawAxis(True, ogre.Vector3.UNIT_Z)
        ## Look at the boxes
        self.camera.lookAt(-150,40,30)

        gBall = self.world.createBall("Ball", 7, vp.position + ogre.Vector3(0,0,80))
        gBall.setDynamicsEnabled(True)
        gBall.getEntity().setMaterialName("Ogre/Eyes")

        box = self.world.createBox("shelf", 75, 125, 5, ogre.Vector3(-150, 40, 30))
        box.getEntity().setMaterialName("Examples/Rocky")

       

        for row in range (self.num_rows):
            for i in range (self.num_rows-row):
                row_size = (self.num_rows - row) * self.BOX_SIZE * 1.25
                name = "box"
                name += str((row*self.num_rows) + i)
                box = self.world.createBox(name, self.BOX_SIZE,self.BOX_SIZE,self.BOX_SIZE , 
                    ogre.Vector3(-150, 
                        40 - (row_size * 0.5) + (i * self.BOX_SIZE * 1.25) , 
                        32.5 + (self.BOX_SIZE / 2) + (row * self.BOX_SIZE)))
                box.setDynamicsEnabled(False, True)
                box.getEntity().setMaterialName("Examples/10PointBlock")
 
        self.camera.setCollisionEnabled(False)
        self.camera.getRealCamera().setQueryFlags(0)

        ## Create the targeting sphere
        targetEnt = self.sceneManager.createEntity("testray", "sphere.mesh")
        mat = ogre.MaterialManager.getSingleton().create("targeter", 
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)
        Pass = mat.getTechnique(0).getPass(0)
        tex = Pass.createTextureUnitState()
        tex.setColourOperationEx(ogre.LBX_SOURCE1, ogre.LBS_MANUAL, ogre.LBS_CURRENT, 
            ogre.ColourValue.Red)
        Pass.setLightingEnabled(False)
        Pass.setSceneBlending(ogre.SBT_ADD)
        Pass.setDepthWriteEnabled(False)


        targetEnt.setMaterialName("targeter")
        targetEnt.setCastShadows(False)
        targetEnt.setQueryFlags(0)
        gTargetNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
        gTargetNode.setScale(0.025, 0.025, 0.025)
        gTargetNode.attachObject(targetEnt)

        gRsq = self.sceneManager.createRayQuery(ogre.Ray())
        gRsq.setSortByDistance(True, 1)
        gRsq.setWorldFragmentType(ogre.SceneQuery.WFT_SINGLE_INTERSECTION)
        
    ## Create new frame listener
    def _createFrameListener(self):
        self.frameListener = BspCollisionListener(self.renderWindow, self.camera)
        self.root.addFrameListener(self.frameListener)

    
if __name__ == '__main__':
    try:
        application = BspCollisionApplication()
        application.go()
    except ogre.Exception, e:
        print e
    







