import Ogre
import OIS

class InputSystem(Ogre.WindowEventListener):
    """
    This handles input from the keyboard and mouse.  It currently just listens
    for the ESCAPE key and quits ogre is needed.
    """
    def __init__(self, graphics_sys):
        # Call constructor of C++ super class
        Ogre.WindowEventListener.__init__(self)
        
        self.render_window = graphics_sys.render_window
        
        # Hook OIS up to the window created by Ogre
        windowHnd = self.render_window.getCustomAttributeInt("WINDOW")
        self.input_mgr = OIS.createPythonInputSystem(windowHnd)
        
        # Setup Unbuffered Keyboard and Buffered Mouse Input
        self.keyboard = \
            self.input_mgr.createInputObjectKeyboard(OIS.OISKeyboard, False)
        self.mouse = \
            self.input_mgr.createInputObjectMouse(OIS.OISMouse, True)
        
        # Allows buttons to toggle properly    
        self.time_until_next_toggle = 0
        
    def __del__ (self ):
      Ogre.WindowEventUtilities.removeWindowEventListener(self.render_window, 
                                                          self)
      self.windowClosed(self.render_window)
            
    def update(self, time_since_last_update):
        # Drop out if the render_window has been closed
        if(self.render_window.isClosed()):
            return False
        
        # Need to capture/update each device - this will also trigger any listeners
        self.keyboard.capture()    
        self.mouse.capture()
        
        # Decrement toggle time if any has build up
        if self.time_until_next_toggle >= 0:
            self.time_until_next_toggle -= time_since_last_update
            
        if self.keyboard.isKeyDown(OIS.KC_ESCAPE) or self.keyboard.isKeyDown(OIS.KC_Q):
            return False
        
        return True
        
    # Ogre.WindowEventListener Methods
    def windowResized(self, render_window):
        """
        Called by Ogre when window changes size
        """
        # Note the wrapped function as default needs unsigned int's
        [width, height, depth, left, top] = render_window.getMetrics()  
        ms = self.mouse.getMouseState()
        ms.width = width
        ms.height = height
        
    def windowClosed(self, render_window):
        """
        Called by Ogre when a window is closed
        """
        #Only close for window that created OIS (mWindow)
        if( render_window == self.render_window ):
            if(self.input_mgr):
                self.input_mgr.destroyInputObjectMouse( self.mouse )
                self.input_mgr.destroyInputObjectKeyboard( self.keyboard )
                OIS.InputManager.destroyInputSystem(self.input_mgr)
                self.input_mgr = None