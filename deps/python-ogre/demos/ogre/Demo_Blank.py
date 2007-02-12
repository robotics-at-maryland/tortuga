# This code is in the Public Domain
import  Ogre as ogre
import sys

def doit():
    # create the root 
    self.root = ogre.Root(getPluginPath())
    
    # setup the resources 
    config = ogre.ConfigFile()
    config.load('resources.cfg' ) #, '', False )
    seci = config.getSectionIterator()
    while (seci.hasMoreElements()):
        secName = seci.peekNextKey()
        settings = seci.getNext()
        ## Note that getMultiMapSettings is a Python-Ogre extension to return a multimap in a list of tuples
        settingslist = config.getMultiMapSettings ( settings )
        for typeName, archName in settingslist:
            ogre.ResourceGroupManager.getSingleton().addResourceLocation(archName, typeName, secName)
            
            
    # load the resources
    ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
    carryOn = self.root.showConfigDialog()
    if carryOn:
        self.renderWindow = self.root.initialise(True, "OGRE Render Window")
    else:
        sys.exit()
    self.sceneManager = self.root.createSceneManager(ogre.ST_GENERIC,"ExampleSMInstance")
    self.camera = self.sceneManager.createCamera('PlayerCam')
    self.camera.setPosition(ogre.Vector3(0, 0, 500))
    self.camera.lookAt(ogre.Vector3(0, 0, -300))
    
    self.viewport = self.renderWindow.addViewport(self.camera)
    
    sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
    sceneManager.setSkyBox(True, "Examples/SpaceSkyBox")
    light = sceneManager.createLight('MainLight')
    light.setPosition (20, 80, 50)
    
    # need a framelistener
    self.frameListener = BlankListener(self.renderWindow, self.camera, self.sceneManager)
    self.root.addFrameListener(self.frameListener)
    
    self.root.startRendering()
        
        
class BlankListener(ogre.FrameListener, ogre.WindowEventListener):
    def __init__(self, renderWindow, camera, sceneManager):
        ogre.FrameListener.__init__(self)
        ogre.WindowEventListener.__init__(self)
        self.sceneManager = sceneManager
        self.camera = camera
        self.renderWindow = renderWindow
        
        windowHnd = self.renderWindow.getCustomAttributeInt("WINDOW")
        self.InputManager = OIS.createPythonInputSystem([("WINDOW",str(windowHnd))])
        self.Keyboard = self.InputManager.createInputObjectKeyboard( OIS.OISKeyboard, self.bufferedKeys )
        self.Mouse = self.InputManager.createInputObjectMouse( OIS.OISMouse, self.bufferedMouse )
        
        #Set initial mouse clipping size
        self.windowResized(self.renderWindow)
        
        #Register as a Window listener
        ogre.WindowEventUtilities.addWindowEventListener(self.renderWindow, self);
        
    def windowClosed(self, rw):
      if( rw == self.renderWindow ):
         if( self.InputManager ):
            self.InputManager.destroyInputObjectMouse( self.Mouse )
            self.InputManager.destroyInputObjectKeyboard( self.Keyboard )
            OIS.InputManager.destroyInputSystem(self.InputManager)
            self.InputManager=None

        
    def windowResized (self, rw):
         [width, height, depth, left, top] = rw.getMetrics()  # Note the wrapped function as default needs unsigned int's
         ms = self.Mouse.getMouseState()
         ms.width = width
         ms.height = height
    
    def __del__ (self ):
        ogre.WindowEventUtilities.removeWindowEventListener(self.renderWindow, self)
        self.windowClosed(self.renderWindow)
       
    def frameStarted(self, frameEvent):
        if(self.renderWindow.isClosed()):
            return False
        ##Need to capture/update each device - this will also trigger any listeners
        self.Keyboard.capture()    
        self.Mouse.capture()

        if self.Keyboard.isKeyDown(OIS.KC_ESCAPE) or self.Keyboard.isKeyDown(OIS.KC_Q):
            return False
        return sf.FrameListener.frameStarted(self, frameEvent)

