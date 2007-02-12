import Ogre as ogre
import SampleFramework as sf
import SampleLoadingBar as slb

class BspApplication (sf.Application):

    def __init__(self):
        sf.Application.__init__(self)
        self.mLoadingBar = slb.ExampleLoadingBar()

    def _loadResources(self):
        self.mLoadingBar.start(self.renderWindow, 1, 1, 0.75)
       
        # Turn off rendering of everything except overlays
        self.sceneManager.clearSpecialCaseRenderQueues()
        self.sceneManager.addSpecialCaseRenderQueue(ogre.RENDER_QUEUE_OVERLAY)
        self.sceneManager.setSpecialCaseRenderQueueMode(ogre.SceneManager.SCRQM_INCLUDE)

        # Set up the world geometry link
        ogre.ResourceGroupManager.getSingleton().linkWorldGeometryToResourceGroup(
            ogre.ResourceGroupManager.getSingleton().getWorldResourceGroupName(), 
            self.mQuakeLevel, self.sceneManager)

#         # Initialise the rest of the resource groups, parse scripts etc
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        ogre.ResourceGroupManager.getSingleton().loadResourceGroup(
            ogre.ResourceGroupManager.getSingleton().getWorldResourceGroupName(),
            False, True)

        # Back to full rendering
        self.sceneManager.clearSpecialCaseRenderQueues()
        self.sceneManager.setSpecialCaseRenderQueueMode(ogre.SceneManager.SCRQM_EXCLUDE)

        self.mLoadingBar.finish()

    # Override resource sources (include Quake3 archives)
    def _setUpResources(self):
        # Load Quake3 locations from a file
        cf = ogre.ConfigFile()

        cf.load("quake3settings.cfg")

        self.mQuakePk3 = cf.getSetting("Pak0Location")
        self.mQuakeLevel = cf.getSetting("Map")

        sf.Application._setUpResources(self)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation(
            self.mQuakePk3, "Zip", ogre.ResourceGroupManager.getSingleton().getWorldResourceGroupName())

    # Override scene manager (use indoor instead of generic)
    def _chooseSceneManager(self):
        self.sceneManager = self.root.createSceneManager("BspSceneManager")

        
    # Scene creation
    def _createScene(self):
        # modify camera for close work
        
        self.camera.setNearClipDistance(4)
        self.camera.setFarClipDistance(0)
        self.sceneManager.setAmbientLight ( (0.7, 0.7, 0.7) )

        # Also change position, and set Quake-type orientation
        # Get random player start point
        vp = self.sceneManager.getSuggestedViewpoint(True)
        self.camera.setPosition(vp.position)
        self.camera.pitch(ogre.Degree(90)) # Quake uses X/Y horizon, Z up
        self.camera.rotate(vp.orientation)
        # Don't yaw along variable axis, causes leaning
        self.camera.setFixedYawAxis(True, ogre.Vector3.UNIT_Z)

        ## Look at a nice place :)
        self.camera.lookAt(-150,40,30)



if __name__ == '__main__':
    try:
        application = BspApplication()
        application.go()
    except ogre.Exception, e:
        print e
    