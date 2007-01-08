# main.py
import Ogre as ogre
import SampleFramework as sf
from sys import stdout

class ApplicationFramework(object):
    def __init__(self):
        self.client = None	
        self.root = ogre.Root( sf.getPluginPath() )
        configFile = ogre.ConfigFile()
        configFile.load( "resources.cfg",'',False )
        for section, key, path in configFile.values:
            ogre.ResourceGroupManager.getSingleton().addResourceLocation( path, key, section )
           
        renList = self.root.getAvailableRenderers()
        miscParams = ogre.NameValuePairList()
        bOpenGL = True	
        #Set render system option
        #pRenderSystem->setConfigOption("Full Screen", "No");
        #pRenderSystem->setConfigOption("VSync", "Yes");
        #pRenderSystem->setConfigOption("Anti aliasing", "No");
        #pRenderSystem->setConfigOption("Video Mode", "640 x 480 @ 32-bit colour"); 
        #Properties.insert(
        #Ogre::NameValuePairList::value_type("externalWindowHandle",
        #Ogre::StringConverter::toString((long)hwndRenderTarget)))
		
        for r in renList:
            print "Renederer",r.name
            if r.name.startswith( "OpenGL" ) and bOpenGL:
                self.root.renderSystem = r
                miscParams["FSAA"] = "1"
                miscParams["Full Screen"] = "Yes"
                miscParams["Video Mode"]= "1024 x 768 @ 32-bit colour"
				
                stdout.write("Using OpenGL renderer")
            else:
               if r.name.startswith('Direct') and not bOpenGL:
                    self.root.renderSystem = r
                    stdout.write("Using DirectX renderer")
                    miscParams["VSYNC"] = "YES"
                    miscParams["Anti aliasing"] = "Level4"
					
           
        self.root.initialise(False)
		
        print dir(miscParams)		
        print miscParams	
        print len(miscParams)		
        #mRoot->initialise(false);
        #mWindow = mRoot->createRenderWindow("Sunny Arkanoid", 700, 700, false, &miscParams);
		
        self.renderWindow = self.root.createRenderWindow( "PyOgreWindow", 800, 600, False,miscParams )
        self.sceneManager = self.root.createSceneManager(ogre.ST_GENERIC,"Test")
        self.camera = self.sceneManager.createCamera( "RootCam" )
        self.camera.nearClipDistance = 5
        self.camera.position = (180, 300, 335)
        self.camera.lookAt((0,0,0))
       
        ogre.TextureManager.getSingleton().defaultNumMipmaps = 5
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        self.viewport = self.renderWindow.addViewport( self.camera )
        self.viewport.backgroundColour = (0, 0, 0)
        # create head entity
        entity = self.sceneManager.createEntity('head', 'ogrehead.mesh')
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)
		
        #self.camera.position = (20, 0, 100)
        #self.camera.lookAt(0, 0, 0)		
        self._createFrameListener()

    def __del__(self):
        "Clear variables, this is needed to ensure the correct order of deletion"
        del self.camera
        del self.sceneManager
        del self.frameListener
        del self.system
        del self.guiRenderer
        del self.root
        del self.renderWindow   

    def render(self):
        return self.root.renderOneFrame()
       
    def _createFrameListener(self):
        self.frameListener = ListenerFramework(self.renderWindow, self.camera, self)
        self.frameListener.showDebugOverlay(True)
        self.root.addFrameListener(self.frameListener)

class ListenerFramework(sf.FrameListener):
    def __init__(self, renderWindow, camera, client):
        self.client = client
        sf.FrameListener.__init__(self, renderWindow, camera)

    def frameStarted(self, evt):
        #print "i'm in frameStarted"
        #ShouldProvideTraceback
        return sf.FrameListener.frameStarted(self, evt)
   
app = ApplicationFramework()
while app.render():
    pass     


