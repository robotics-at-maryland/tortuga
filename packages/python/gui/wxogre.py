# Library Includes
import wx
import Ogre

# Projects Include
from sim import Simulation
from core import property

class wxOgre(wx.Control):
    """
    This control creates and Ogre Rendering window relying on the simulation
    to handle the ogre specific part.
    """
    def __init__(self, camera, parent, id = -1, pos = wx.DefaultPosition, 
             size = DefaultSize, style = 0, validator = wx.DefaultValidator, 
             name = wx.ControlNameStr):
        self.Create(camera, parent, id, pos, size, style, validator, name)
    
    def Create(self, camera, parent, id = -1, pos = wx.DefaultPosition, 
             size = DefaultSize, style = 0, validator = wx.DefaultValidator, 
             name = wx.ControlNameStr):
        
        if wx.Control.Create(parent, id, pos, size, style, validator, name):
            if camera is not None:
                self._camera = camera
                self._camera.setAutoAspectRatio(True)
            self._init_ogre()
            
            # Setup our event handlers
            self.Bind(wx.EVT_CLOSE, self._on_close)
            # self.Bind(wx.EVT_IDLE, self._update)
            self.Bind(wx.EVT_ERASE_BACKGROUND, self._update)
            self.Bind(wx.EVT_SIZE, self._update) 
            
        return True
    
    class camera(property):
        """
        The camera attached to the main viewport of the window.
        """
        def fget(self):
            return self._camera
        def fset(self, camera):
            if self._camera is None:
                self._viewport = self._render_window.addViewport(self._camera)
            else:
                self._camera = camera
                self._viewport.setCamera(camera)
    
    def _init_ogre(self):
        """
        Hook ogre up to the control.  On Linux this must be called after your
        top level frame has recived its first activate event, or after its
        contructor has finished. I suggest you do so for all platforms for 
        simplicity.
        """
        self._create_ogre_window()
        self._render_window.update()
            
    def _update(self, event):
        """
        Handles all events that require redrawing
        """
        # Resize the window on resize
        if event.EventType == event.SizeEvent.EventType:
            # On GTK we let Ogre create its own child window, so we have to
            # manually resiz    e it match its parent, this control
            if '__WXGTK__' == wx.Platform:
                size = self.GetClientSize()
                self._render_window.resize(size.width, size.height)
            self._render_window.windowMovedOrResized()
        
        # Redraw the window for every event
        self._render_window.update()
            
    def _on_close(self, event):
        self._render_window.removeAllViewports()
        self._render_window.destroy()
    
    def _create_ogre_window(self):
        size = self.GetClientSize()
        params = self._get_window_params()
        graphics_sys = Simulation.get().graphics_sys
        
        self._render_window = \
            graphics_sys.create_window(self.GetName(), size.width, size.height, 
                                       false, params)
            
        # You can only create a camera after you have made the first render
        # window, so check to see if we are given a camera
        if self._camera is not None:
            self._viewport = self._render_window.addViewport(self._camera)
            
    def _get_window_params(self):
        """
        Encapsulates the platform specific part of the window creation, grabing
        the windows handle and transforming them for Ogre.
        """
        params = Ogre.NamedValuePairList()
        
        if '__WXGTK__' == wx.Platform:
            raise Exception('Support for Linux not yet integrated')
        elif '__WXMSW__' == wx.Platform:
            params['externalWindowHandle'] = str(self.GetHandle())
        else:
            raise Exception('%s no yet supported' % wx.Platform)
        