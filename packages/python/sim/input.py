# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  vehicle/sim/input.py

"""
    Wraps up the initialization and management of OIS and other input releated
activities
"""

# Makes everything much easier, all imports must work from the root
#from __future__ import absolute_import

# Library imports
import Ogre
import OIS
import platform

# Project imports
import event
import logloader

from core import FixedUpdater
from sim.util import Vector
from sim import *
from sim.simulation import Simulation

class InputError(simulation.SimulationError):
    """ Error from the input system """
    pass

# Add the events to the event system
event.add_event_types(['KEY_PRESSED',     # Once fired once per key press
                       'KEY_RELEASED',    # When a key is released
                       'MOUSE_MOVED',     # When the mouse is moved
                       'MOUSE_PRESSED',   # When a mouse button is pressed
                       'MOUSE_RELEASED']) # When the mouse button is released

class InputSystem(FixedUpdater, Ogre.WindowEventListener):
    """
    This handles input from the keyboard and mouse.  It currently just listens
    for the ESCAPE key and quits ogre is needed.
    """
    
    DEFAULT_EVENT_MAP = {
         # Thruster control section 
         'PORT_THRUST_UP' : OIS.KC_U,
         'PORT_THRUST_DOWN' : OIS.KC_J,
         'STARBOARD_THRUST_UP' : OIS.KC_I,
         'STARBOARD_THRUST_DOWN' : OIS.KC_K,
         'FORE_THRUST_UP' : OIS.KC_Y,
         'FORE_THRUST_DOWN' : OIS.KC_H,
         'AFT_THRUST_UP' : OIS.KC_O,
         'AFT_THRUST_DOWN' : OIS.KC_L}
         # Camera control section Not yet done
         #'CAM_FORWARD' : [OIS.KC_LSHIFT, OIS.KC_RSHIFT],
         #'CAM_FORWARD': [OIS.KC_W, OIS.KC_UP], 
         #'CAM_LEFT': [OIS.KC_A, OIS.KC_LEFT], 
         #'CAM_BACK': [OIS.KC_DOWN, OIS.KC_S], 
         #'CAM_RIGHT': [OIS.KC_D, OIS.KC_RIGHT]}
    
    def __init__(self, config):
        self._setup_logging(config.get('Logging', {'name' : 'Input',
                                                   'level': 'INFO'}))
        self.logger.info('* * * Beginning initialization')
        
        # Call constructor of C++ super class
        Ogre.WindowEventListener.__init__(self)
        
        FixedUpdater.__init__(self, 1.0 / config.get('update_rate',60), 1.0)
        
        self.render_window = Simulation.get().graphics_sys.render_window
        
        self._setup_ois(config)
        # Allows buttons to toggle properly    
        self.time_until_next_toggle = 0
        
        # Setup fowarding of events through the system
        self.event_forwarder = EventForwarder(self.mouse, self.keyboard)
        
        self._load_event_map(config.get('Event_Map', 
                                        InputSystem.DEFAULT_EVENT_MAP))
        self.logger.info('* * * Initialized')
        
    def _setup_ois(self, config):
        # Hook OIS up to the window created by Ogre
        windowHnd = self.render_window.getCustomAttributeInt("WINDOW")
        
        if 'Linux' == platform.system():
            params = [('WINDOW',windowHnd)]
            if config.get('debug', False):
                self.logger.info("OIS Keyboard grab off")
                params = params + [('x11_keyboard_grab', 'false'), 
                                   ('x11_mouse_grab','false')]
        elif 'Windows' == platform.system():
            params = windowHnd
            
        
        self.input_mgr = OIS.createPythonInputSystem(params)
        # Setup Unbuffered Keyboard and Buffered Mouse Input
        self.keyboard = \
            self.input_mgr.createInputObjectKeyboard(OIS.OISKeyboard, True)
        self.mouse = \
            self.input_mgr.createInputObjectMouse(OIS.OISMouse, True)
        
        
    def __del__ (self ):
        self.logger.info('* * * Beginning shutdown')
        print "DELETING VEHICLE"
        if self.render_window is not None:
            self.windowClosed(self.render_window)
            Ogre.WindowEventUtilities.removeWindowEventListener(self.render_window, 
                                                          self)
        self.logger.info('* * * Shutdown complete')
         
    def _setup_logging(self, config):
        self.logger = logloader.setup_logger(config, config)   
            
    def _update(self, time_since_last_update):
        # Drop out if the render_window has been closed
        if(self.render_window.isClosed()):
            event.post('SIM_SHUTDOWN')
        
        # Need to capture/update each device - this will also trigger any listeners
        self.keyboard.capture()    
        self.mouse.capture()
        
        # Decrement toggle time if any has build up
        if self.time_until_next_toggle >= 0:
            self.time_until_next_toggle -= time_since_last_update
            
        # Quit simulation if needed
        if self.keyboard.isKeyDown(OIS.KC_ESCAPE):
            event.post('SIM_SHUTDOWN')
        
        self._generate_events()
    
    def _load_event_map(self, config):
        self.key_event_map = {}
        for event,key in config.iteritems():
            # Fail if we already have the even registered
            if self.key_event_map.has_key(event):
                raise InputError('Event "%s" already assinged' % event)
            
            if type(key) is not list:
                self.key_event_map[event] = [key]
            else:
                self.key_event_map[event] = key
                
            
        
    def _generate_events(self):
        # Go through every event and check its keys
        for event_type,keys in self.key_event_map.iteritems():
            send_event = True
            
            # Only send the event if all keys are down
            for key in keys:
                if not self.keyboard.isKeyDown(key):
                    send_event = False
                    break
            if send_event:
                event.post(event_type)
        
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
                print 'WINDOW CLOSED...'
                self.input_mgr.destroyInputObjectMouse( self.mouse )
                self.input_mgr.destroyInputObjectKeyboard( self.keyboard )
                OIS.InputManager.destroyInputSystem(self.input_mgr)
                self.input_mgr = None
                self.render_window = None
                
        # Send shutdown event
        event.post('SIM_SHUTDOWN')
                
class EventForwarder(OIS.MouseListener, OIS.KeyListener):
    """
    This grabs mouse and keyboard events and sends them through our event 
    system so other parts of the system use the information.
    """
    def __init__(self, mouse, keyboard):
        # Call C++ Super class constructor
        OIS.MouseListener.__init__( self)
        OIS.KeyListener.__init__(self)
        
        # Register our self as listener for the input events
        mouse.setEventCallback(self)
        keyboard.setEventCallback(self)
        
    # OIS MouseListener Methods
    def mouseMoved(self, mouse_event):
        event.send('MOUSE_MOVED', mouse_event)
        return True

    def mousePressed(self, mouse_event, id):
        event.send('MOUSE_PRESSED', mouse_event, id)
        return True

    def mouseReleased(self, mouse_event, id):
        event.send('MOUSE_RELEASED', mouse_event, id)
        return True
    
    # OIS KeyListener Methods
    def keyPressed(self, key_event):
        event.send('KEY_PRESSED', key_event)
        return True

    def keyReleased(self, key_event):
        event.send('KEY_RELEASED', key_event)
        return True

class KeyStateObserver(object):
    """
    This keeps track of the up down state of keys based on key down and up
    events and automatically set attributes on the give object to reflect this.
    For example if it watching KC_LSHIFT, the 'lshift_down' property on the 
    given object will reflect whether or no the left shift key is down or not.
    As long as all key up and down events are passed to the object.
    """
    def __init__(self, subject, keys):
        """
        Subject is the subject to set the attribute on.  Keys is list of keys
        you wish to have set as attributes.  If there are any tuples in the 
        list first element of the tuple will be treated as the attribute and
        second as the list of keys you wished mapped to it.
        """
        self.subject = subject
        self.key_map = {}
     
        for key in keys:
            if type(key) is tuple:
                setattr(self.subject, key[0], False)
                for key_code in key[1]:
                    self.key_map[key_code] = [key[0], 0]
            else:
                attr_name = str(key).lower().split('_')[1] + '_key'
                setattr(self.subject, attr_name, False)
                self.key_map[key] = attr_name
            
            
    
    def key_pressed(self, key_event):
        if self.key_map.has_key(key_event.key):
            attr = self.key_map[key_event.key] 
            # If we have a list that means this multiple keys are mapped to a 
            # single attribute, and we have to key track of how many are down.
            # So that when they come up we don't set the state to false early
            
            if type(attr) is list:
                attribute, count = attr
                count += 1
                self.key_map[key_event.key] = [attribute, count]
                setattr(self.subject, attribute, True)
            else:
                setattr(self.subject, attr, True)
        
    def key_released(self, key_event):
        if self.key_map.has_key(key_event.key):
            attr = self.key_map[key_event.key] 
            # See above for more explanation on this
            if type(attr) is list:
                attribute, count = attr
                count -= 1
                self.key_map[key_event.key] = [attribute, count]
                if 0 == count:
                    setattr(self.subject, attribute, False)
            else:
                setattr(self.subject, attr, False)
        
    