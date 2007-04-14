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
import ogre.renderer.OGRE as Ogre
import ogre.io.OIS as OIS

# Project imports
import platform
import event
import logloader

from core import fixed_update, cls_property, Enum, Interface, Component, ExtensionPoint
from sim.util import Vector
from sim.util import SimulationError

# Key types
KC = Enum('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 
          'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z') 


class InputError(SimulationError):
    """ Error from the input system """
    pass

def pack_mod_keys(ctrl, alt, shift, meta):
    """
    Turns the mod keys into a unique number based on there state.
    """
    return (int(ctrl) << 3) | (int(alt) << 2) | (int(shift) << 1) | int(meta)

#class InputSource(Interface):
#    def update():
#        """
#        When called the input source will generate
#        """
#        pass

class InputSystem(object):
    def __init__(self):
        self._action_map = {}
    
    def update(self, time):
        pass
    
    # Keyboard Inputs
    def map_key(self, key_string):
        """
        @type  key_string: string
        @param key_string: A string reprsenting the key like 'A', 'Z', or 'BACK'
        
        @retrun: The keycode, or None 
        """
        if hasattr(KC, key_string):
            return getattr(KC, key_string)
        else:
            return None
    
    def key_pressed(self, key, ctrl_down, alt_down, shift_down, meta_down):
        self._post_key_event(key, True, ctrl_down, alt_down, shift_down, 
                             meta_down)
        
    def key_released(self, key, ctrl_down, alt_down, shift_down, meta_down):
        self._post_key_event(key, False, ctrl_down, alt_down, shift_down, 
                             meta_down)
        
    def _post_key_event(self, key, down, ctrl_down, alt_down, shift_down, 
                        meta_down):
        mod_keys = pack_mod_keys(ctrl_down, alt_down, shift_down, meta_down)

        event_type = self._action_map.get((key, mod_keys), None)
        if event_type is not None:
            event.post(event_type, key, down, mod_keys)                         
        
    
    def _map_actions(self, action_map, action = None):
        """
        TODO: Document me
        """
        # Check to see if we have a dict
        if hasattr(action_map, 'iteritems'):
            for input_event, action_list in action_map.iteritems():
                # Make sure we have a list of callbacks
                actions = [action_list] # Assume list single event
                if type(action_list) is list:
                    actions = action_list
                
                for action in actions:
                    self._map_action(input_event, action)
        # Only have a single action
        elif action is not None:
            # Handler_map is our type now
            self._map_action(action_map, action)
        else:
            raise InputError('_map_actions requries a dict or given callback')
    
    def _map_action(self, input_event, action):
        try:
            self._action_map[input_event].append(action)
        except KeyError:
            self._action_map[input_event] = [action]
## Add the events to the event system
#event.add_event_types(['KEY_PRESSED',     # Once fired once per key press
#                       'KEY_RELEASED',    # When a key is released
#                       'MOUSE_MOVED',     # When the mouse is moved
#                       'MOUSE_PRESSED',   # When a mouse button is pressed
#                       'MOUSE_RELEASED']) # When the mouse button is released
#
#class InputSystem(Ogre.WindowEventListener):
#    """
#    This handles input from the keyboard and mouse.  It currently just listens
#    for the ESCAPE key and quits ogre is needed.
#    """
#    
#    def __init__(self, config):
#        self._setup_logging(config.get('Logging', {'name' : 'Input',
#                                                   'level': 'INFO'}))
#        self.logger.info('* * * Beginning initialization')
#        
#        self._key_event_map = {}
#        
#        # Call constructor of C++ super class
#        Ogre.WindowEventListener.__init__(self)
#        
#        self._update_interval = 1.0 / config.get('update_rate',60)
#        
#        self.render_window = Simulation.get().graphics_sys.render_window
#        
#        self._setup_ois(config)
#        # Allows buttons to toggle properly    
#        self.time_until_next_toggle = 0
#        
#        # Setup fowarding of events through the system
#        self.event_forwarder = EventForwarder(self.mouse, self.keyboard)
#        
#        self.logger.info('* * * Initialized')
#        
#        # Need to make these local to the class for some reason
#        self.ois_cleanup = OIS.InputManager.destroyInputSystem
#        self.ogre_cleanup = Ogre.WindowEventUtilities.removeWindowEventListener
#        
#    def map_key_events(self, event_map, key = None):
#        if hasattr(event_map, 'iteritems'):
#            for event_type, key_list in event_map.iteritems():
#                # Make sure we have a list of callbacks
#                keys = [key_list] # Assume list single event
#                if type(key_list) is list:
#                    keys = key_list
#                
#                for key in keys:
#                    self._map_key_event(event_type, key)
#        elif key is not None:
#            # Handler_map is our type now
#            self._map_key_event(event_map, key)
#        else:
#            raise InputError('map_event_type requries a dict or given callback')
#    
#    def _map_key_event(self, event, key):
#        try:
#            self._key_event_map[event].append(key)
#        except KeyError:
#            self._key_event_map[event] = [key]
#        
#    def _setup_ois(self, config):
#        # Hook OIS up to the window created by Ogre
#        windowHnd = self.render_window.getCustomAttributeInt("WINDOW")
#        
#        if 'Linux' == platform.system():
#            params = [('WINDOW',windowHnd)]
#            if config.get('debug', False):
#                self.logger.info("OIS Keyboard grab off")
#                params = params + [('x11_keyboard_grab', 'false'), 
#                                   ('x11_mouse_grab','false')]
#        elif 'Windows' == platform.system():
#            params = windowHnd
#            
#        
#        self.input_mgr = OIS.createPythonInputSystem(params)
#        # Setup Unbuffered Keyboard and Buffered Mouse Input
#        self.keyboard = \
#            self.input_mgr.createInputObjectKeyboard(OIS.OISKeyboard, True)
#        self.mouse = \
#            self.input_mgr.createInputObjectMouse(OIS.OISMouse, True)
#        
#        
#    def __del__ (self ):
#        self.logger.info('* * * Beginning shutdown')
#        if self.render_window is not None:
#            self.windowClosed(self.render_window)
#            self.ogre_cleanup(self.render_window, self)
#        self.logger.info('* * * Shutdown complete')
#         
#    def _setup_logging(self, config):
#        self.logger = logloader.setup_logger(config, config)   
#            
#    @fixed_update('_update_interval')
#    def _update(self, time_since_last_update):
#        # Drop out if the render_window has been closed
#        if(self.render_window.isClosed()):
#            event.post('SIM_SHUTDOWN')
#        
#        # Need to capture/update each device - this will also trigger any listeners
#        self.keyboard.capture()    
#        self.mouse.capture()
#        
#        # Decrement toggle time if any has build up
#        if self.time_until_next_toggle >= 0:
#            self.time_until_next_toggle -= time_since_last_update
#            
#        # Quit simulation if needed
#        if self.keyboard.isKeyDown(OIS.KC_ESCAPE):
#            event.post('SIM_SHUTDOWN')
#        
#        self._generate_events()        
#        
#    def _generate_events(self):
#        # Go through every event and check its keys
#        for event_type,keys in self._key_event_map.iteritems():
#            send_event = True
#            
#            # Only send the event if all keys are down
#            for key in keys:
#                if not self.keyboard.isKeyDown(key):
#                    send_event = False
#                    break
#            if send_event:
#                event.post(event_type)
#        
#    # Ogre.WindowEventListener Methods
#    def windowResized(self, render_window):
#        """
#        Called by Ogre when window changes size
#        """
#        # Note the wrapped function as default needs unsigned int's
#        [width, height, depth, left, top] = render_window.getMetrics()  
#        ms = self.mouse.getMouseState()
#        ms.width = width
#        ms.height = height
#        
#    def windowClosed(self, render_window):
#        """
#        Called by Ogre when a window is closed
#        """
#        #Only close for window that created OIS (mWindow)
#        if( render_window == self.render_window ):
#            if(self.input_mgr):
#                self.logger.info('Shutting down OIS')
#                self.input_mgr.destroyInputObjectMouse( self.mouse )
#                self.input_mgr.destroyInputObjectKeyboard( self.keyboard )
#                self.ois_cleanup(self.input_mgr)
#                self.input_mgr = None
#                self.render_window = None
#            
#    def shutdown_ois(self):
#        self.input_mgr.destroyInputObjectMouse( self.mouse )
#        self.input_mgr.destroyInputObjectKeyboard( self.keyboard )
#        OIS.InputManager.destroyInputSystem(self.input_mgr)
#        
#        # Send shutdown event
#        event.post('SIM_SHUTDOWN')
#        
#                
#class EventForwarder(OIS.MouseListener, OIS.KeyListener):
#    """
#    This grabs mouse and keyboard events and sends them through our event 
#    system so other parts of the system use the information.
#    """
#    def __init__(self, mouse, keyboard):
#        # Call C++ Super class constructor
#        OIS.MouseListener.__init__( self)
#        OIS.KeyListener.__init__(self)
#        
#        # Register our self as listener for the input events
#        mouse.setEventCallback(self)
#        keyboard.setEventCallback(self)
#        
#    # OIS MouseListener Methods
#    def mouseMoved(self, mouse_event):
#        event.send('MOUSE_MOVED', mouse_event)
#        return True
#
#    def mousePressed(self, mouse_event, id):
#        event.send('MOUSE_PRESSED', mouse_event, id)
#        return True
#
#    def mouseReleased(self, mouse_event, id):
#        event.send('MOUSE_RELEASED', mouse_event, id)
#        return True
#    
#    # OIS KeyListener Methods
#    def keyPressed(self, key_event):
#        event.send('KEY_PRESSED', key_event)
#        return True
#
#    def keyReleased(self, key_event):
#        event.send('KEY_RELEASED', key_event)
#        return True
#
#class KeyStateObserver(object):
#    """
#    This keeps track of the up down state of keys based on key down and up
#    events and automatically set attributes on the give object to reflect this.
#    For example if it watching KC_LSHIFT, the 'lshift_down' property on the 
#    given object will reflect whether or no the left shift key is down or not.
#    As long as all key up and down events are passed to the object.
#    """
#    def __init__(self, subject, keys):
#        """
#        Subject is the subject to set the attribute on.  Keys is list of keys
#        you wish to have set as attributes.  If there are any tuples in the 
#        list first element of the tuple will be treated as the attribute and
#        second as the list of keys you wished mapped to it.
#        """
#        self.subject = subject
#        self.key_map = {}
#     
#        for key in keys:
#            if type(key) is tuple:
#                setattr(self.subject, key[0], False)
#                for key_code in key[1]:
#                    self.key_map[key_code] = [key[0], 0]
#            else:
#                attr_name = str(key).lower().split('_')[1] + '_key'
#                setattr(self.subject, attr_name, False)
#                self.key_map[key] = attr_name
#            
#            
#    
#    def key_pressed(self, key_event):
#        if self.key_map.has_key(key_event.key):
#            attr = self.key_map[key_event.key] 
#            # If we have a list that means this multiple keys are mapped to a 
#            # single attribute, and we have to key track of how many are down.
#            # So that when they come up we don't set the state to false early
#            
#            if type(attr) is list:
#                attribute, count = attr
#                count += 1
#                self.key_map[key_event.key] = [attribute, count]
#                setattr(self.subject, attribute, True)
#            else:
#                setattr(self.subject, attr, True)
#        
#    def key_released(self, key_event):
#        if self.key_map.has_key(key_event.key):
#            attr = self.key_map[key_event.key] 
#            # See above for more explanation on this
#            if type(attr) is list:
#                attribute, count = attr
#                count -= 1
#                self.key_map[key_event.key] = [attribute, count]
#                if 0 == count:
#                    setattr(self.subject, attribute, False)
#            else:
#                setattr(self.subject, attr, False)
#        
#    