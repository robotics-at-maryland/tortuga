import wx 
import Ogre as ogre 
try : 
    import psyco #use psyco if available (JIT compiler) 
    psyco.full() 
except ImportError: 
    pass 

def getPluginPath():
    """Return the absolute path to a valid plugins.cfg file.""" 
    import sys
    import os
    import os.path
    
    paths = [os.path.join(os.getcwd(), 'plugins.cfg'),
             '/etc/OGRE/plugins.cfg',
             os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              'plugins.cfg')]
    for path in paths:
        if os.path.exists(path):
            return path

    sys.stderr.write("\n"
        "** Warning: Unable to locate a suitable plugins.cfg file.\n"
        "** Warning: Please check your ogre installation and copy a\n"
        "** Warning: working plugins.cfg file to the current directory.\n\n")
    raise ogre.Exception(0, "can't locate the 'plugins.cfg' file", "")

class Struct: 
    "simple dummy class to regroup scene entities in a single parameter" 
    pass 

class OgreWindow(wx.PyWindow): 
    """wx.Frame subclass to embed an Ogre window in wxPython 
    It is somewhat inspired from SampleFramework.py""" 
    
    #all animation states included in this dictionary will be automatically updated at each render 
    animStates = {} #the values have to be (AnimationState, SpeedFactor) tuples 
    
    #namespace for holding a reference to the entities you create. You can comment it out 
    #if you don't like the idea 
    sceneEntities = Struct() 
    
    def __init__(self, parent, ID, size = wx.Size(640,480), renderSystem = "OpenGL",**kwargs): 
        self.parent=parent 
        wx.PyWindow.__init__(self, parent, ID, size = size, **kwargs) 
        #Event bindings 
        self.Bind(wx.EVT_CLOSE, self._OnCloseWindow) 
        self.Bind(wx.EVT_ERASE_BACKGROUND, self._OnEraseBackground) 
        self.Bind(wx.EVT_SIZE, self._OnSize) 
        self.Bind(wx.EVT_TIMER, self.UpdateRender) #Bind the timer events to Ogre rendering 
        #Timer creation 
        self.timer = wx.Timer() 
        self.timer.SetOwner(self) #Sets the timer to notify self: binding the timer event is not enough 
        
        #Ogre Initialisation 
        self._OgreInit(size,renderSystem) 
        self.SceneInitialisation() 
        self.UpdateRender() 
        self.SetFocus()#Gives KeyboardFocus 
        
    def _OgreInit(self,size,renderSystem):    
        #Root creation 
        root = ogre.Root(getPluginPath()) 
        self.ogreRoot = root 

        # setup resources 
        config = ogre.ConfigFile() 
#         config.loadFromFile('resources.cfg') 
#         for section, key, path in config.values: 
#             ogre.ResourceGroupManager.getSingleton().addResourceLocation(path, key, section)  
                      
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/OgreCore.zip", "Zip", "Bootstrap", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/fonts", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/programs", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/scripts", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/textures", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/models", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/overlays", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/particle", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/cubemap.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/cubemapsJS.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/dragon.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/fresneldemo.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/ogretestmap.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/skybox.zip", "Zip", "General", False)

        #The following section tries to avoid showing the configuration dilaog. 
#         rsList = root.getAvailableRenderers() #Gets the available RenderSystems 
#         rs=None 
#         CouldNotFindRequestedRenderer=False 
#         for i in rsList: 
#             if renderSystem in i.name: #Tries to locate the requested render system 
#                 rs=i 
#         if rs is not None : 
#             root.renderSystem=rs 
#             rs.setConfigOption("Full Screen","No") 
#             try : 
#                 rs.setConfigOption("Video Mode","800 x 600 @ 16-bit colour") 
#             except : 
#                 CouldNotFindRequestedRenderer=True 
#         else : 
#             CouldNotFindRequestedRenderer=True 
#             
#         if CouldNotFindRequestedRenderer:    #Calls Ogre's default Config dialog if failed 
#             carryOn = root.showConfigDialog() 
#             if not carryOn: 
#                 sys.exit('Quit from Config Dialog') 
        carryOn = root.showConfigDialog() 
        if not carryOn: 
            sys.exit('Quit from Config Dialog') 

                
        root.initialise(False) 
        
        renderParameters = ogre.NamedValuePairList() 
        renderParameters['externalWindowHandle'] = str(self.GetHandle()) 
        renderWindow = root.createRenderWindow('wxPython render window', size[0], 
                                               size[1], False, renderParameters) 
        renderWindow.active = True 
        self.renderWindow = renderWindow 
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups() 
        
                

    
    def _OnSize(self, event): 
        
        if getattr(self, 'ogreRoot', None): 
            self.UpdateRender() 
        event.Skip() 

    def _OnEraseBackground(self, event): 
        pass # Do nothing, to avoid flashing on MSW. 

    def _OnCloseWindow(self, event): 
        self.Destroy() 

    def AcceptsFocus(self): 
        return True 
    
        
    def StartRendering(self,interval=33): 
        "Activates the timer. A rendering will be triggered at each interval (default :33ms)" 
        self.timer.Start(interval) 
    
    def OnFrameStarted(self,event): 
        "SubClass and fill in this declaration if you want something done before each rendering" 
        pass 
    
    def OnFrameEnded(self,event): 
        "SubClass and fill in this declaration if you want something done after each rendering" 
        pass 
    
    def UpdateRender(self,event=None): 
        """Method that can be called manually or by a wx.Timer 
        If the method is called by timer, the animation states contained in 
        the self.animStates variable get updated before rendering the frame""" 
        
        #Enables the user to define actions to be undertaken before the frame is started 
        self.OnFrameStarted(event)        
        
        if hasattr(event ,"GetInterval"): #Timer Events have a GetInterval method that returns the time interval between events 
            for anim,speed in self.animStates.itervalues(): 
                anim.addTime(speed*event.GetInterval()/1000.0) #updates the animations 
        
        self.ogreRoot.renderOneFrame()#Asks Ogre to render a frame 
        
        #Enables the user to define actions to be undertaken after the frame has been drawn 
        self.OnFrameEnded(event) 
        
    def SceneInitialisation(self): 
        """This method can be replaced completely to suit your needs or you can 
        subclass only the helper methods such as self._CreateCamera""" 
        self._ChooseSceneManager() 
        self._CreateCamera() 
        self._CreateViewport() 
        self._PopulateScene() 
        self._MouseAndKeysBindings() 
    
    #You will want to subclass the following methods to suit your needs    
        
    def _ChooseSceneManager(self): 
        "choose SceneManager" 
        #sceneManager = self.ogreRoot.getSceneManager(ogre.ST_GENERIC) 
        self.sceneManager = self.ogreRoot.createSceneManager(ogre.ST_GENERIC,"ExampleSMInstance")
        #self.sceneManager = sceneManager 
    
    def _CreateCamera(self):        
        "create a Camera" 
        camera = self.sceneManager.createCamera('Camera') 
        camera.lookAt(ogre.Vector3(0, 0, 0)) 
        camera.nearClipDistance = 5 
        self.sceneEntities.Camera = camera 
        
        # create the camera nodes & attach camera 
        cameraNode = self.sceneManager.getRootSceneNode().createChildSceneNode("CamNode", ogre.Vector3(0, 0, 250)) 
        PitchNode = cameraNode.createChildSceneNode("PitchNode") 
        PitchNode.attachObject(camera) 
        
        self.sceneEntities.CamNode = cameraNode 
        self.sceneEntities.PitchNode = PitchNode 

    def _CreateViewport(self):    
        "create a Viewport" 
        renderWindow=self.renderWindow 
        viewport = renderWindow.addViewport(self.sceneEntities.Camera, 0, 0.0, 0.0, 1.0, 1.0) 
        viewport.backgroundColour = ogre.ColourValue(0, 0, 0) 

    
    
    def _PopulateScene(self): 
        "Implement this method to put entities in your scene" 
        pass 
        
    def _MouseAndKeysBindings(self): 
        "Some Additional mouse and keys bindings" 
        d=10.0 #displacement for key strokes 
        self.ControlKeyDict={wx.WXK_LEFT:ogre.Vector3(-d,0.0,0.0), 
                             wx.WXK_RIGHT:ogre.Vector3(d,0.0,0.0), 
                             wx.WXK_UP:ogre.Vector3(0.0,0.0,-d), 
                             wx.WXK_DOWN:ogre.Vector3(0.0,0.0,d), 
                             wx.WXK_PAGEUP:ogre.Vector3(0.0,d,0.0), 
                             wx.WXK_PAGEDOWN:ogre.Vector3(0.0,-d,0.0)} 
        self.Bind(wx.EVT_KEY_DOWN,self._DefaultKeyDownManager) 
        ##self.Bind(wx.EVT_ENTER_WINDOW,lambda evt : self.SetFocus()) 
        self.Bind(wx.EVT_MOUSE_EVENTS,self._DefaultMouseEventManager) 
        
    def _DefaultKeyDownManager(self,event): 
        "If you want to implement a similar callback function, do not forget the event.Skip() at the end" 
        validMove = self.ControlKeyDict.get(event.m_keyCode,False) 
        if validMove : 
            self.sceneEntities.CamNode.translate(validMove,self.sceneEntities.CamNode.TS_LOCAL  ) 
        event.Skip() 
        
    def _DefaultMouseEventManager(self, event): 
        "If you want to implement a similar callback function, do not forget the event.Skip() at the end" 
        self.SetFocus() #Gives Keyboard focus to the window 
        
        if event.RightDown(): #Precedes dragging 
            self.StartDragX, self.StartDragY = event.GetPosition() #saves position of initial click 
        
        if event.Dragging() and event.RightIsDown(): #Dragging with RMB 
            x,y = event.GetPosition() 
            dx = x-self.StartDragX 
            dy = y-self.StartDragY 
            self.StartDragX, self.StartDragY = x, y 
        
            self.sceneEntities.CamNode.yaw(ogre.Degree(dx/3.0)) 
            self.sceneEntities.PitchNode.pitch(ogre.Degree(dy/3.0)) 
        
        
        event.Skip() 
