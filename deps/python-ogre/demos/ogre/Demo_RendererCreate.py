#
# Python_Ogre Demo
#
# shows how to create a window with a title etc
#

import Ogre as ogre
import SampleFramework as sf
from sys import stdout

class ApplicationFramework(object):
    def __init__(self):
        self.client = None  
        self.root = ogre.Root( sf.getPluginPath() )
        config = ogre.ConfigFile()
        config.load('resources.cfg' ) 
        
        ## this is still ugly as we need to create a real python iterator...
        seci = config.getSectionIterator()
        while (seci.hasMoreElements()):
            secName = seci.peekNextKey()
            settings = seci.getNext()
            settingslist = config.getMultiMapSettings ( settings )
            for typeName, archName in settingslist:
                ogre.ResourceGroupManager.getSingleton().addResourceLocation(archName, typeName, secName)

                
        renList = self.root.getAvailableRenderers()
        miscParams = ogre.NameValuePairList()
        bOpenGL = True
        
        for r in renList:
            print "Renederer",r.getName(), r, type(r)
            if r.getName().startswith( "OpenGL" ) and bOpenGL:
                self.root.setRenderSystem ( r )
                miscParams["FSAA"] = "1"
                #miscParams["Full Screen"] = "Yes"
                miscParams["Full Screen"] = "No"
                
                miscParams["Video Mode"]= "1024 X 768 @ 32-bit colour"
                miscParams["title"]="Python-Ogre is COOL - Check the source to see how to set the title!!"
                stdout.write("Using OpenGL renderer")
            else:
               if r.getName().startswith('Direct') and not bOpenGL:
                    self.root.setRenderSystem ( r )
                    stdout.write("Using DirectX renderer")
                    miscParams["VSYNC"] = "YES"
                    miscParams["Anti aliasing"] = "Level4"
           
        self.root.initialise(False)
        
        self.renderWindow = self.root.createRenderWindow( "Python-Ogre Window", 800, 600, False, miscParams )
        self.sceneManager = self.root.createSceneManager(ogre.ST_GENERIC,"Test")
        
        self.camera = self.sceneManager.createCamera( "RootCam" )
        self.camera.nearClipDistance = 5
        self.camera.setPosition (180, 300, 335)
        self.camera.lookAt(0,0,0)
       
        ogre.TextureManager.getSingleton().defaultNumMipmaps = 5
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        self.viewport = self.renderWindow.addViewport( self.camera )
        self.viewport.setBackgroundColour (ogre.ColourValue(0, 0, 0))
        # create head entity
        entity = self.sceneManager.createEntity('head', 'ogrehead.mesh')
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)
        
        self.camera.setPosition (20, 0, 100)
        self.camera.lookAt(0, 0, 0)     
        self._createFrameListener()

    def __del__(self):
        "Clear variables, this should not actually be needed."
        del self.camera
        del self.sceneManager
        del self.frameListener
        if self.world:
            del self.world
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
        return sf.FrameListener.frameStarted(self, evt)
   
app = ApplicationFramework()
while app.render():
    pass     


