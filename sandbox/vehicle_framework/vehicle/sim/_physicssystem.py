import Ogre
import OgreNewt

class PhysicsSystem(object):
    """
    This handles everything need with the Physics system.
    """
    def __init__(self, config, gfx_sys):
        self.gfx_sys = gfx_sys
        
        # The main OgreNewt object
        self.world = OgreNewt.World()
        # Here so we can clean up after outselves
        self.bodies = []
        
        self.update = (1.0 / config['Physics']['update_rate'])
        
        # Start up the debugger so that we can show debugging lines
        OgreNewt.Debugger.getSingleton().init(gfx_sys.scene_manager)
        
    def __del__(self):
        """
        You must delete this before the GraphicsSystem
        """
        OgreNewt.Debugger.getSingleton().deInit()
        
        del self.bodies
        del self.World
        
    def update(self, time_since_last_update):
        """
        Called at a set interval update the physics and there graphical 
        counter parts.  This cannot be running at the same time as update for
        the GraphicsSystem.
        """
        self.elapsed += time_since_last_update
  
        if ((self.elapsed > self.update) and (self.elapsed < (1.0)) ):
            while (self.elapsed > self.update):
                self.world.update(self.update)
                self.elapsed -= self.update
        else:
            if (self.elapsed < self.update):
                # not enough time has passed this loop, so ignore for now.
                pass
            else:
                self.world.update(self.elapsed)
                # reset the elapsed time so we don't become "eternally behind"
                self.elapsed = 0.0