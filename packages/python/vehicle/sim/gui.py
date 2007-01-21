# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/gui.py

"""
Wraps up the initialization and management of CEGUI and GUI activites
"""

import os
import logging
import shutil

import OIS
import CEGUI
import Ogre

import logloader
import event

class GUISystem(OIS.MouseListener, OIS.KeyListener):
    """
    This class manages the GUI, currently it just draws the mouse cursor
    """
    
    def __init__(self, config, graphics_sys, input_sys):
        self.cegui_sys = None
        self.cegui_log = None
        self.gui_renderer = None
        
        self._setup_logging(config.get('Logging', {'name' : 'Graphics',
                                                   'level': 'INFO'}))
        self.logger.info('* * * Beginning initialization')
        
        # Call C++ Super class constructor
        OIS.MouseListener.__init__( self)
        OIS.KeyListener.__init__(self)
        
        # Create our own logger to reroute the logging
        #self.cegui_log = CEGUI2PyLog(config["Logging"]) # Broken Python-Ogre 0.70
        
        CEGUI.System.setDefaultXMLParserName('TinyXMLParser')
        # Create the CEGUIOgreRender with are GUI Components and CEGUI System
        self.gui_renderer = \
            CEGUI.OgreCEGUIRenderer( graphics_sys.render_window, 
                                     Ogre.RENDER_QUEUE_OVERLAY, False, 3000, 
                                     graphics_sys.scene_manager)
        self.cegui_sys = CEGUI.System(self.gui_renderer)
        
        # Hack to deal with the singleton log issue
        CEGUI.Logger.getSingletonPtr().setLogFilename("../logs/CEGUI_Rest.log")
        shutil.move("CEGUI.log","../logs/CEGUI_Start.log")
        
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
        
        # Setup forwarder so CEGUI gets input events
        self.event_forwarder = CEGUIEventForwarder(self.cegui_sys)
        
        self.logger.info('* * * Initialized')
        
    def __del__(self):
        self.logger.info('* * * Beginning shutdown')
        #del self.cegui_log
        del self.cegui_sys
        del self.gui_renderer
        self.logger.info('* * * Shutdown complete')
        
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)
    
class CEGUI2PyLog(CEGUI.Logger):
    """
    This pipes CEGUI's logging into the python logging system and supresses 
    normal console and file output.
    """
    def __init__(self, config):
        # Call to the C++ base class
        CEGUI.Logger.__init__(self)
        self.logger = logloader.setup_logger(config, config)
        
        self.log_level_map = {CEGUI.LoggingLevel.Errors : self.logger.critical,
                              CEGUI.LoggingLevel.Standard : self.logger.info,
                              CEGUI.LoggingLevel.Informative : self.logger.info,
                              CEGUI.LoggingLevel.Insane : self.logger.debug}
        
    def logEvent(self, message, level):
        self.log_level_map[level](message.c_str())
        
    def setLogFilename(self, filename, append = False):
        pass
 
class CEGUIEventForwarder(object):
    """
    This grabs mouse and keyboard events from the event system and hands them
    off to CEGUI
    """
    def __init__(self, cegui_sys):
        self.cegui_sys = cegui_sys
  
        self.handler_map = {
            'KEY_PRESSED': self.key_pressed,
            'KEY_RELEASED': self.key_released,
            'MOUSE_MOVED': self.mouse_moved,
            'MOUSE_PRESSED': self.mouse_pressed,
            'MOUSE_RELEASED': self.mouse_released}
        
        event.register_handlers(self.handler_map)
    
    def __del__(self):
        event.remove_handlers(self.handler_map)
      
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
        
    # Mouse Events
    def mouse_moved(self, arg):
        #print 'Moved'
        self.cegui_sys.injectMouseMove( arg.get_state().X.rel, 
                                        arg.get_state().Y.rel )

    def mouse_pressed(self, arg, id):
        self.cegui_sys.injectMouseButtonDown(self.convertOISMouseButtonToCegui(id))

    def mouse_released(self, arg, id):
        self.cegui_sys.injectMouseButtonUp(self.convertOISMouseButtonToCegui(id))

    # Keyboard Events
    def key_pressed(self, arg):
        self.cegui_sys.injectKeyDown(arg.key)
        self.cegui_sys.injectChar(arg.text )

    def key_released( self, arg ):
        self.cegui_sys.injectKeyUp(arg.key)
    