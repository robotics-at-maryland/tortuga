import Ogre
import OgreNewt as OgreNewt
import SampleFramework as sf

class BasicFrameListener (Ogre.FrameListener ): ##, Ogre.WindowEventListener ): # sf.FrameListener):

    def __init__(  self, win,  SceneManager,  World , update_framerate): 
        Ogre.FrameListener.__init__(self)
        self.World = World
        self.desired_framerate = update_framerate
        self.renderWindow = win
        self.sceneManager = SceneManager
        self.update = (1.0 / self.desired_framerate)
        self.debugflag = False  ## turn on and off debugging here to stop windows memory crashes
        self.elapsed = 0.0;
        #OgreNewt.Debugger.getSingleton().init(self.sceneManager) 

    
    def debug ( self , state):
        if state :  # user want to turn it on..
            if not self.debugflag:
                self.debugflag = True
        else:   # turn it off
            if self.debugflag:
                self.debugflag=False
                ##OgreNewt.Debugger.getSingleton().hideLines() 
                
    def __del__ ( self ):
        self.debug ( False )
        #OgreNewt.Debugger.getSingleton().deInit() 

    def frameStarted(self, evt):
        Ogre.FrameListener.frameStarted(self, evt)
        self.elapsed += evt.timeSinceLastFrame
        if self.debugflag == True:
            pass
            ##OgreNewt.Debugger.getSingleton().showLines(self.World)
        if ((self.elapsed > self.update) and (self.elapsed < (1.0)) ):
            while (self.elapsed > self.update):
                self.World.update( self.update )
                self.elapsed -= self.update
        else:
            if (self.elapsed < self.update):
                ## not enough time has passed this loop, so ignore for now.
                pass
            else:
                self.World.update( self.elapsed )
                self.elapsed = 0.0 ## reset the elapsed time so we don't become "eternally behind"
        return True
        