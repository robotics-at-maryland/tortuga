class OgreSys(object):
    """
    Wrap up all ogre initialization in an Inner class (maybe more it outside)
    """    
    def __init__(self):
        self.root = None # Done
        self.camera = None
        self.renderWindow = None # Done
        self.sceneManager = None # Done
    
    def _setUp(self, config):
        self.ogre.root = ogre.Root();
        ogreConf = config['Ogre']

        # Add Resource Locations
        _addResourceLocations(ogreConf['Resources']);

        # if we cannot initialise Ogre, just abandon the whole deal         
        if not _initOgreCore(ogreConf['RenderSystem']):
            return False
        # Create the Ogre SceneManager
        self.sceneManager = self.root.createSceneManager(ogre.ST_GENERIC,
                                                        'SimSceneMgr')
        
        # Possibly considate/config drive these
        _createCamera();
        _createViewports();

        # Set default mipmap level (NB some APIs ignore this)
        ogre.TextureManager.getSingleton().setDefaultNumMipmaps(5)
        # Initialise resources
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        # Create the scene (override me)
        _createScene();

        return True
    
    def  _addResourceLocations(self, config);
        """
        The Config needs a section like so, this tells ogre where to look
        for all the files to load.
        [Ogre]
            [[Resources]]
                zip = ['../Media/packs/textures.zip',
                    '../Media/packs/models.zip']
                filesystem = ['../Media'
                    '../Media/fonts',
                    '../Media/materials/programs',
                    '../Media/materials/scripts']
        """
        rsrcMrg = ogre.ResourceGrourpManager.getSingleton()
        for group in config:
            for type in group:
                for location in type:
                    rsrcMrg(location, type, group)
                    
    def _initOgreCore(self, config):
        """
        Loads the needed setting ogre settings from the config file, based
        on render system type. Should be of the form:
        [Ogre]
        [[RenderSystem]]
            type = GLRenderSystem

            # Must be of the same name as the type                
            [[GLRenderSystem]]
                Colour Depth=32
                Display Frequency=N/A
                FSAA=0
                Full Screen=No
                RTT Preferred Mode=FBO
                VSync=No
                Video Mode=640 x 480

        """
        typemap = {'GLRenderSystem' : 'GLRenderSystem',
                   'OpenGL' : 'GLRenderSystem',
                   'DirectX' : 'D3D9RenderSystem',
                   'DirectX9' : 'D3D9RenderSystem',
                   'Direct 3D' : 'D3D9RenderSystem'}
                   
        try:
            type = typemap[config['Type'].replace(' ','')]
            renderSystem = getattr(ogre, type)
        except AttributeError:
            print 'Improper Ogre RenderSystem'
            return False

        # Load our options from the custom config system          
        for option in config[renderSystem]:
            renderSystem.setConfigOption(option, config[option])

        # If we have gotten this far, the we can let Ogre create a default
        # window from the setting loaded above
        self.root = ogre.Root()
        self.root.setRenderSystem(renderSystem)
        self.renderWindow = self.root.initialize(True)
        return True

    def _createCamera(self):
        """Creates the camera."""        
        self.camera = self.sceneManager.createCamera('PlayerCam')
        self.camera.setPosition(ogre.Vector3(0, 0, 500))
        self.camera.lookAt(ogre.Vector3(0, 0, -300))
        self.camera.NearClipDistance = 5

    def _createViewports(self):
        """Creates the Viewport."""
        self.viewport = self.renderWindow.addViewport(self.camera)
        self.viewport.BackgroundColour = ogre.ColourValue(0,0,0)        