import os
import time

import Ogre
from Ogre import sf_OIS as sf

class GraphicsSystem(object):
    """
    This class handles all initialization of graphics with the Ogre rendering
    engine.  It is driven completely by config file.  Current a bug in 
    Python-Ogre means with have the rendersystem config dialog.
    """    
    def __init__(self, config):
        self.root = None
        self.camera = None
        self.render_window = None
        self.scene_manager = None
        
        # Setups Ogre, step by step
        self._setUp(config)
        
    def __del__(self):
        del self.camera;
        del self.scene_manager
        del self.root
        del self.render_window  
    
    def update(self, time_since_last_update):
        """
        Renders one frame
        """
        self.root.renderOneFrame()
        # Fake rendering for now
        time.sleep(0.1)
        return True
    
    def _setUp(self, config):
        self.root = Ogre.Root(sf.getPluginPath());

        # Add Resource Locations
        self._addResourceLocations(config['Resources']);

        # if we cannot initialise Ogre, just abandon the whole deal         
        if not self._initOgreCore(config['RenderSystem']):
            return False
        # Create the Ogre SceneManager
        self.scene_manager = self.root.createSceneManager(Ogre.ST_GENERIC,
                                                        'SimSceneMgr')
        
        # Possibly considate/config drive these
        self._createCamera();
        self._createViewports();

        # Set default mipmap level (NB some APIs ignore this)
        Ogre.TextureManager.getSingleton().setDefaultNumMipmaps(5)
        # Initialise resources
        Ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        # Create the scene (override me)
        # _createScene();

        return True
    
    def  _addResourceLocations(self, config):
        """
        This replaces Ogre's resources.cfg, Each node under resources
        is a group and each node under that is an archive type, with a 
        list of directories to load for that type. Example:
        
        Ogre:
            Resources:
                General:
                    Zip: ['media/packs/OgreCore.zip',
                          'media/packs/cubemapsJS.zip']
                    FileSystem: ['media/models',
                                 'media/primitives',
                                 'media/materials/textures',
                                 'media/materials/scripts']
        """
        rsrcMrg = Ogre.ResourceGroupManager.getSingleton()
        for group in config:
            group_cfg = config[group];
            for type in group_cfg:
                for location in group_cfg[type]:
                    location = os.path.abspath(location)
                    rsrcMrg.addResourceLocation(location, type, group)
                    
    def _initOgreCore(self, config):
        """
        This secion here takes over for Ogre's ogre.cfg.  The type 
        indicates which Ogre RenderSystem to create and which section to
        load the options from.  Example:
        
        Ogre:
            RenderSystem:
                type: GLRenderSystem
                
                GLRenderSystem:
                    - [Colour Depth, '32']
                    - [Display Frequency, 'N/A']
                    - [FSAA, '0']
                    - [Full Screen, 'No']
                    - [RTT Preferred Mode, 'FBO']
                    - [VSync, 'No']

        """
        
        # Allows looser specification of RenderSystem names
        typemap = {'GLRenderSystem' : 'GLRenderSystem',
                   'OpenGL' : 'GLRenderSystem',
                   'DirectX' : 'D3D9RenderSystem',
                   'DirectX9' : 'D3D9RenderSystem',
                   'Direct 3D' : 'D3D9RenderSystem'}
        # Currently doesn't work           
        #try:
        #    type = typemap[config['type'].replace(' ','')]
        #    RenderSystemCls = getattr(ogre, type)
        #except AttributeError:
        #    print 'Improper Ogre RenderSystem'
        #    return False
        
        #renderSystem = RenderSystemCls()
        # Load our options from the custom config system          
        #for name, value in config[str(RenderSystemCls)]:
        #    renderSystem.setConfigOption(name, value])

        # If we have gotten this far, the we can let Ogre create a default
        # window from the settings loaded above
        #self.root.setRenderSystem(renderSystem)
        
        carryOn = self.root.showConfigDialog()
        if carryOn:
            self.render_window = self.root.initialise(True, "MRBC AUV SIM")
        return carryOn

    def _createCamera(self):
        """
        Creates the camera.
        """        
        self.camera = self.scene_manager.createCamera('PlayerCam')
        self.camera.setPosition(Ogre.Vector3(0, 0, 500))
        self.camera.lookAt(Ogre.Vector3(0, 0, -300))
        self.camera.NearClipDistance = 5

    def _createViewports(self):
        """
        Creates the Viewport.
        """
        self.viewport = self.render_window.addViewport(self.camera)
        self.viewport.BackgroundColour = Ogre.ColourValue(0,20,0)        