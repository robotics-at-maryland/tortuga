import os
import time
import logging

import Ogre
from Ogre import sf_OIS as sf

from vehicle.sim.utility import SimulationError

class GraphicsError(SimulationError):
    """ Error from the graphics system """
    pass

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
        self._setup(config)
        
    def __del__(self):
        # Make sure the C++ based objects are deleted in the proper order
        del self.camera;
        del self.scene_manager
        del self.root
        del self.render_window  
        #del self.logManager
        del self.ogre_log
        
    def update(self, time_since_last_update):
        """
        Renders one frame
        """
        self.root.renderOneFrame()
        return True
    
    def _setup(self, config):
        #    self._reroute_logging(config)  # Broken in Python-Ogre 0.70
        self.root = Ogre.Root(sf.getPluginPath());

        # Add Resource Locations
        self._addResourceLocations(config['Resources']);

        # if we cannot initialise Ogre, just abandon the whole deal         
        self._initOgreCore(config['RenderSystem'])
            
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
    
    def _reroute_logging(self, config):
        #logManager = Ogre.LogManager.getSingletonPtr()
        self.logManager = Ogre.LogManager()
 
        # Create That, doesn't output anything, just calls its listeners
        self.ogre_log = Py2OgreLog(config)
        self.logManager.setDefaultLog(self.ogre_log)
        
    
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
                   
        try:
            type = typemap[config['type'].replace(' ','')]
            # Todo: replace this with proper render system iteration
            #RenderSystemCls = getattr(ogre, type)
        except AttributeError, KeyError:
            raise GraphicsError('Improper Ogre RenderSystem')
        
        #renderSystem = RenderSystemCls()
        # Load our options from the custom config system          
        #for name, value in config[str(RenderSystemCls)]:
        #    renderSystem.setConfigOption(name, value])

        # If we have gotten this far, the we can let Ogre create a default
        # window from the settings loaded above
        #self.root.setRenderSystem(renderSystem)
        
        # Small hack until we can iterater render systems
        ogrecfg = file('ogre.cfg','w')
        if type == 'GLRenderSystem':
            ogrecfg.write("Render System=OpenGL Rendering Subsystem\r\n\r\n"
                          "[OpenGL Rendering Subsystem]\r\n")
        else:
            ogrecfg.write("Render System=Direct3D9 Rendering Subsystem\r\n\r\n"
                          "[Direct3D9 Rendering Subsystem]\r\n")
        for name, value in config[type]:
            ogrecfg.write('%s=%s\r\n'% (name,value)) 
        ogrecfg.close()
        
        restored = self.root.restoreConfig()
        os.remove(ogrecfg.name)
        
        if not restored:
            if not self.root.showConfigDialog():
                raise GraphicsError('Go not initialize render system')
            
        self.render_window = self.root.initialise(True, "MRBC AUV SIM")
        
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
        self.viewport.BackgroundColour = Ogre.ColourValue(0,0,0)
        
        
class Py2OgreLog(Ogre.Log):
    """
    This pipes Ogre's logging into the python logging system and supresses 
    normal console and file output.
    """
    def __init__(self, config):
        # Call to the C++ base class
        Ogre.Log.__init__(self, "python.logging", True, False)
        self.config = config
        self.logger = logging.getLogger('Graphics')
        
        self.log_level_map = {Ogre.LML_CRITICAL : self.logger.critical,
                              Ogre.LML_NORMAL : self.logger.warning,
                              Ogre.LML_TRIVIAL : self.logger.debug}
        
    def logMessage(self, message, level, debug):
        self.log_level_map[level](message)
        
               