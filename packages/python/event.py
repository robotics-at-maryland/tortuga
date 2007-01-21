# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  event.py

"""
This module is a very simple event system.  It allow you to register, events
types (which can be anything), to callback functions.  It places no restraint 
on the signature of the callback functions and multiple events can be 
registered to one event type.
"""

from collections import deque

class EventError(Exception):
    pass

# Module global variables
event_map = {}
event_queue = deque()

def post(_type, *args, **kwargs):
    event_queue.appendleft((_type, args, kwargs))
    
def process_events():
    while len(event_queue) > 0:
        _type, args, kwargs = event_queue.pop()
        _send(_type, args, kwargs)

def send(_type, *args, **kwargs):
    _send(_type, args, kwargs)

def _send(_type, args, kwargs):
    """
    Calls all registered callbacks and returns a list of the non None return
    values of the callback functions.
    """
    if event_map.has_key(_type):
        ret_values = []
        for callback in event_map[_type]:
            ret = apply(callback, args, kwargs)
            if None is not ret:
                ret_values.append(ret)
        return ret_values
    else:
        raise EventError('Event type: "%s", is not present' % _type)
    
def register_handler(_type, callback):
    """
    Add a callback to the list of callbacks for the given type
    """
    try:
        event_map[_type].append(callback)
    except KeyError:
        raise EventError('Event type: "%s", is not present' % _type)
    
def register_handlers(handler_map):
    if type(handler_map) is not dict:
        raise EventError('register_handler requries a dict')
    else:
        for _type, callback in handler_map.iteritems():
            register_handler(_type, callback)

def remove_handler(_type, callback):
    """
    Remove a callback to the list of callbacks for the given type
    """
    try:
        event_map[_type].remove(callback)
    except KeyError:
        raise EventError('Event type: "%s", is not present' % _type)
    except ValueError:
        raise EventErrot('Event type "%s" does not have callback: %s' % \
                         (_type, callback.__name__))
        
def remove_handlers(handler_map):
    if type(handler_map) is not dict:
        raise EventError('remove_handlers requries a dict')
    else:
        for _type, callback in handler_map.iteritems():
            remove_handler(_type, callback)

def add_event_type(_type):
    """
    Registers an event type, if the type is list or tuple it will treat each 
    element as an event type.  Other wise the passed object will be an event 
    type.
    """
    if type(_type) is list:
        for t in _type:
            _add_event_type(t)
    else:
        _add_event_type(_type)
        
def _add_event_type(_type):
    """
    Registers an event type, can be any python object if the item is a list
    """
    if event_map.has_key(_type):
        raise EventError('Event type: "%s", allready present' % _type)
    else:
        event_map[_type] = []
        
    