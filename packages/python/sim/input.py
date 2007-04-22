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
import decorator

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
    def __init__(self, config):
        self._action_map = {}
        
        self._load_actions(config.get('KeyMap', {}))
    
    def update(self, time):
        pass
    
    def _load_actions(self, config):
        """
        Loads actions from the config file in the following form:
        KeyMap:
            CTRL+TAB+A: THRUST_UP
        
        The key combination can be in any order, and capitalization doesn't 
        matter.  They value is the event that is fired when the key combination
        is recevied.
        """

        for key_combination, event_type in config.iteritems():
            keys = key_combination.split('+')
            mod_keys = {'ctrl' : False, 'alt' : False,  'shift' : False, 
                        'meta' : False }
            key = None
            
            # Determine Mod Keys
            for key_str in keys:
                key_lower = key_str.lower()
                if key_lower in mod_keys:
                    mod_keys[key_lower] = True
                elif key is None:
                    key = self.key_string_to_key(key_str)
                else:
                    raise InputError, 'Invalid key combination: "%s"' % key_combination
            
            self._map_action((key, pack_mod_keys(**mod_keys)), event_type)
            
    
    # Keyboard Inputs
    def key_string_to_key(self, key_string):
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
        self.post_key_event(key, True, ctrl_down, alt_down, shift_down, 
                             meta_down)
        
    def key_released(self, key, ctrl_down, alt_down, shift_down, meta_down):
        self.post_key_event(key, False, ctrl_down, alt_down, shift_down, 
                             meta_down)
        
    def post_key_event(self, key, down, ctrl_down, alt_down, shift_down, 
                        meta_down):
        mod_keys = pack_mod_keys(ctrl_down, alt_down, shift_down, meta_down)

        event_types = self._action_map.get((key, mod_keys), None)
        if event_types is not None:
            for event_type in event_types:
                event.post(event_type, key, down, mod_keys)                         
        
    
    def map_actions(self, action_map, action = None):
        """
        This maps action types to events that will be fired when the action is
        recieved.  So fare the main action type is the button action, this 
        consists tuple which is (button, mod_key_state).
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
            
class OISInputForwarder(OIS.KeyListener, Ogre.WindowEventListener):
    """
    This grabs mouse and keyboard events and sends them through our event 
    system so other parts of the system use the information.
    """
    def __init__(self, config, input_sys, window): #, keyboard):
        self.window = window
        self._input_sys = input_sys
        
        # Call C++ Super class constructor
#       OIS.MouseListener.__init__( self)
        OIS.KeyListener.__init__(self)
        Ogre.WindowEventListener.__init__(self)
#        self._setup_logging(config.get()
#        self.logger.info('* * * Beginning initialization')
        
        self._setup_ois()
        event.register_handlers('SIM_UPDATE', self._update)
        
        #self.logger.info('* * * Initialized')
        
        # Register our self as listener for the input events
#        mouse.setEventCallback(self)
        self.keyboard.setEventCallback(self)
        
    def __del__ (self ):
        #self.logger.info('* * * Beginning shutdown')
        if self.render_window is not None:
            self.windowClosed(self.render_window)
            Ogre.WindowEventUtilities.removeWindowEventListener(self.render_window, self)
        #self.logger.info('* * * Shutdown complete')
        
    def _update(self, time_since_last_update):
        self.keyboard.capture()
        
    def _setup_ois(self):
        # Hook OIS up to the window created by Ogre
        windowHnd = self.window.getCustomAttributeInt("WINDOW")
        
        if 'Linux' == platform.system():
            params = [('WINDOW',windowHnd)]
#            if config.get('debug', False):
#                self.logger.info("OIS Keyboard grab off")
            params = params + [('x11_keyboard_grab', 'false'), 
                               ('x11_mouse_grab','false'),
                                ('XAutoRepeatOn', 'true')]
        elif 'Windows' == platform.system():
            params = windowHnd
        else:
            raise InputSystem, "Platform not supposed"
            
        
        self.input_mgr = OIS.createPythonInputSystem(params)
        # Setup Unbuffered Keyboard and Buffered Mouse Input
        self.keyboard = \
            self.input_mgr.createInputObjectKeyboard(OIS.OISKeyboard, True)
        #self.mouse = \
        #    self.input_mgr.createInputOb
##        keyboard.setEventCallback(self)


    # Ogre.WindowEventListener Methods
    def windowResized(self, render_window):
        """
        Called by Ogre when window changes size
        """
        # Note the wrapped function as default needs unsigned int's
#        [width, height, depth, left, top] = render_window.getMetrics()  
#        ms = self.mouse.getMouseState()
#        ms.width = width
#        ms.height = height
        pass
        
    def windowClosed(self, render_window):
        """
        Called by Ogre when a window is closed
        """
        #Only close for window that created OIS (mWindow)
        if( render_window == self._window ):
            if(self.input_mgr):
                self.logger.info('Shutting down OIS')
                self.input_mgr.destroyInputObjectMouse( self.mouse )
                #self.input_mgr.destroyInputObjectKeyboard( self.keyboard )
                OIS.InputManager.destroyInputSystem(self.input_mgr)
                self.input_mgr = None
                self.render_window = None
                
    # OIS MouseListener Methods
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
    
    # OIS KeyListener Methods
    def keyPressed(self, key_event):
        self._translate_key(key_event, True)
        return True

    def keyReleased(self, key_event):
        self._translate_key(key_event, False)
        return True
    
    def _translate_key(self, key_event, down):
        key = self._input_sys.key_string_to_key(str(key_event.key)[3:])
            
        if key is not None:
            mod_keys = {'ctrl_down' : self.keyboard.isModifierDown(OIS.Keyboard.Ctrl), 
                        'alt_down' : self.keyboard.isModifierDown(OIS.Keyboard.Alt), 
                        'shift_down' : self.keyboard.isModifierDown(OIS.Keyboard.Shift), 
                        'meta_down' : False }
            self._input_sys.post_key_event(key, down, **mod_keys)


class ButtonStateObserver(object):
    """
    This keeps track of the up down state of buttons base the events it is told
    to watch.  Then it makes sure the given attribute on its subject always
    reflects the state of this button.  If it told to handle mutiple events,
    for the same attribute, it makes sure the attribut is true as long as one
    button is pressed down.
    """
    def __init__(self, subject, buttons):
        """
        @type subject: Anything
        @param subject: The object which the attribute is set on.
        
        @type buttons: Dict
        @param buttons: Maps attribute name to a list of events to watch for
                        that attribute.
        """
        
        self._attr_codes = {}
        self._subject = subject
        
        handler_map = {}
        for attr_name, event_types in buttons.iteritems():
            # Ensure the attribute exists on the object
            if not hasattr(subject, attr_name):
                setattr(subject, attr_name, False)
            self._attr_codes[attr_name] = set()
                
            # Generate our handler function
            handler = self.generate_handler(attr_name)
            
            for event_type in event_types:
                event.register_handlers(event_type, handler)
      
    def generate_handler(self, attr_name):
        """
        This generates a handler function to handle the given attribute.  This
        must be a seperate function so that each handler gets its own local 
        scope (ie its a closure !).
        """
        def handler(key, down, mod_keys):
            # Key is being pressed down
            if down:
                # Record every key being pressed down
                self._attr_codes[attr_name].add(key)
                setattr(self._subject, attr_name, True)
            # Key being release
            else:
                # Remove key from the set being pressed down
                self._attr_codes[attr_name].remove(key)
                # If there are no more keys in our set being pressed down
                # We are done
                if len(self._attr_codes[attr_name]) == 0:
                    setattr(self._subject, attr_name, False)
        return handler
    
def toggle(event_type, start_state = False):
    """
    This is debounced toggle, switch.  It will only call the wrapped function
    once per down (ie, repeat down events our ignored)
    """
    def wrapper(func):
        func._switch_state = False
        func._state = start_state
        def debounce_switch(func, self, key, down, mod_keys):
            # Button down, and switch not already down
            if down and not func._switch_state:
                func._switch_state = True
                func._state = not func._state
                func(self, func._state)    
            # toggle off
            elif not down:
                func._switch_state = False
                
        decorated_func = decorator.decorate(func, debounce_switch,
                                            decorator.make_weak_signature(func))
        return decorated_func
    return wrapper