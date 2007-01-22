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

This module also provides both standard event models, a standard fire and 
forget type event, and a signals and slot type event system.
"""

from collections import deque
from Queue import Queue

class EventError(Exception):
    pass

EVENT_QUEUE_SIZE = 1000
event_map = {}
event_queue = Queue(EVENT_QUEUE_SIZE)

def post(_type, *args, **kwargs):
    """
    Place a message into the iternal thread safe queue, will be processed when
    process_events is called.  Only EVENT_QUEUE_SIZE messages are allowed in 
    the queue.
    """
    event_queue.put((_type, args, kwargs))
    
def process_events():
    """
    Processeces all pending events in the queue
    """
    while not event_queue.empty():
        _type, args, kwargs = event_queue.get()
        _send(_type, args, kwargs)

def send(_type, *args, **kwargs):
    """
    Calls all registered callbacks and returns a list of the non None return
    values of the callback functions.
    """
    _send(_type, args, kwargs)

def _send(_type, args, kwargs):
    if event_map.has_key(_type):
        ret_values = []
        for callback in event_map[_type]:
            ret = apply(callback, args, kwargs)
            if None is not ret:
                ret_values.append(ret)
        return ret_values
    else:
        raise EventError('Event type: "%s", is not present' % _type)
    
def _register_handler(_type, callback):
    """
    Add a callback to the list of callbacks for the given type
    """
    try:
        event_map[_type].append(callback)
    except KeyError:
        raise EventError('Event type: "%s", is not present' % _type)
    
def register_handlers(handler_map, callback = None):
    """
    Adds the handler functions to the specified events. Can either add multople
    handler functions to an event, or multiple handlers to multiple events.
    
    @type handler_map: Either a dict like object, or a normal python object
    @param handler_map: A map of event types to callback functions, either a 
        list or singlar.
        
    @type callback: a callable python object, or list of callable objects
    @param callback: This is only used when handler_map is not a dict, its the
        callback, or list of callbacks to add.
    """
    if hasattr(handler_map, 'iteritems'):
        for _type, callback in handler_map.iteritems():
            # Make sure we have a list of callbacks
            callbacks = [callback]
            if type(callback) is list:
                callbacks = callback
            
            for callback in callbacks:
                _register_handler(_type, callback)
    elif callback is not None:
        # Handler_map is our type now
        _register_handler(handler_map, callback)
    else:
        raise EventError('remove_handlers requries a dict or given callback')

def _remove_handler(_type, callback):
    """
    Remove a callback to the list of callbacks for the given type
    """
    try:
        event_map[_type].remove(callback)
    except KeyError:
        raise EventError('Event type: "%s", is not present' % _type)
    except ValueError:
        raise EventError('Event type "%s" does not have callback: %s' % \
                         (_type, callback.__name__))
        
def remove_handlers(handler_map, callback = None):
    """
    Removes the handler functions from the specified events.  Can either remove
    a single handler function from an event, or multiple.
    
    @type handler_map: Either a dict like object, or a normal python object
    @param handler_map: A map of event types to callback functions, either a 
        list or singlar.
        
    @type callback: a callable python object, or list of callable objects
    @param callback: This is only used when handler_map is not a dict, its the
        callback, or list of callbacks to remove.
    """
    if hasattr(handler_map, '__getitem__'):
        for _type, callback in handler_map.iteritems():
            # Make sure we have a list of callbacks
            callbacks = [callback]
            if type(callback) is list:
                callbacks = callback
            
            for callback in callbacks:
                _remove_handler(_type, callback)
    elif callback is not None:
        # Handler_map is our type now
        _remove_handler(handler_map, callback)
    else:
        raise EventError('remove_handlers requries a dict or given callback')
        

def add_event_types(_type):
    """
    Registers an event type, or types.
    
    @type _type: A single python object, or list of python object
    @param _type: The event type, or types to add to event system.
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
        
    