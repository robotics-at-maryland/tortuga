# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/gui.py

"""
Wraps up the initialization and management of CEGUI and GUI activites
"""

import OIS
import CEGUI
import Ogre

class GUISystem(OIS.MouseListener, OIS.KeyListener):
    """
    This class manages the GUI, currently it just draws the mouse cursor
    """
    
    def __init__(self, config, graphics_sys, input_sys):
        # Call C++ Super class constructor
        OIS.MouseListener.__init__( self)
        OIS.KeyListener.__init__(self)
        
        # Register self as listener
        input_sys.mouse.setEventCallback(self)
        input_sys.keyboard.setEventCallback(self)
        
        # Create the CEGUIOgreRender with are GUI Components and CEGUI System
        self.gui_renderer = \
            CEGUI.OgreCEGUIRenderer( graphics_sys.render_window, 
                                     Ogre.RENDER_QUEUE_OVERLAY, False, 3000, 
                                     graphics_sys.scene_manager)
        self.cegui_sys = CEGUI.System(self.gui_renderer)
        
        ## load up CEGUI stuff...
        CEGUI.Logger.getSingleton().setLoggingLevel(CEGUI.Informative)
        
        CEGUI.SchemeManager.getSingleton().loadScheme(config['scheme']) 
        self.cegui_sys.setDefaultMouseCursor(config['look'],  "MouseArrow")
        
        CEGUI.FontManager.getSingleton().createFont(config['font'] + '.font');   
        self.cegui_sys.setDefaultFont(config['font'])
        
        # Create the base window that everything is rendered in
        sheet = CEGUI.WindowManager.getSingleton().createWindow("DefaultWindow", 
                                                                "root_wnd" )
        CEGUI.System.getSingleton().setGUISheet( sheet )
        
    def __del__(self):
        del self.cegui_sys
        del self.gui_renderer
        
    # OIS MouseListener Methods
    def mouseMoved(self, arg):
        self.cegui_sys.injectMouseMove( arg.get_state().X.rel, 
                                        arg.get_state().Y.rel )
        return True

    def mousePressed(self, arg, id):
        self.cegui_sys.injectMouseButtonDown(self.convertOISMouseButtonToCegui(id))
        return True

    def mouseReleased(self, arg, id):
        self.cegui_sys.injectMouseButtonUp(self.convertOISMouseButtonToCegui(id))
        return True
    
    def convertOISMouseButtonToCegui(self, buttonID):
        if buttonID ==0:
            return CEGUI.LeftButton
        elif buttonID ==1:
            return CEGUI.RightButton
        elif buttonID ==2:
            return CEGUI.MiddleButton
        elif buttonID ==3:
            return CEGUI.X1Button
        else:
            return CEGUI.LeftButton
    
    # OIS KeyListener Methods
    def keyPressed(self, arg):
        self.cegui_sys.injectKeyDown(arg.key)
        self.cegui_sys.injectChar(arg.text )
        return True

    def keyReleased( self, arg ):
        self.cegui_sys.injectKeyUp(arg.key)
        return True
    
    