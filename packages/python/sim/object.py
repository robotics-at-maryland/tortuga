# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sim/object.py

# Project Imports
import core
import event
from sim.serialization import two_step_init, IKMLStorable
from sim.util import SimulationError

event.add_event_types(['GATE_ENTERED', 'GATE_EXITED'])

class Temp(object):
    def __init__(self):
        # Gate Releated Members
        self._through_gate = False
        self._gate_entered_velocity = None
    
    def entered_gate(self, vehicle):
        self._gate_entered_velicty = vehicle.velocity
    
    def left_gate(self, vehicle):
        if not self._through_gate:
            velA = self._gate_entered_velicty 
            velB = vehicle.velocity
            angle =  math.acos((velA.absDotProduct(velB)) / 
                           (velA.length() * velB.length()))    
            if math.degrees(angle) < 10:
                print 'Straight Through!'
            self._through_gate = True


class IObject(core.Interface):
    """
    A basic object in the simulation it has and can have children.
    """
    
    parent = core.Attribute("""The parent of current object, none if root""")
    name = core.Attribute("""The name of the object""")
    
    def get_child(name):
        """
        @type  name: string
        @param name: The name fo the child you wish to retrieve
        """
        pass
    
    def add_child(child):
        """        
        @type  child: Implementer of IObject
        @param child: The child to add
        """
        pass
    
    def remove_child(child):
        """
        @type  child: IOjbect or string
        @param child: The actual object you wish to remove, or its name
        """
        pass
        
        #if IObject.providedBy(child):
        #    pass
        
    def update(time_since_last_update):
        pass

class Object(core.Component):
    core.implements(IObject, IKMLStorable)
    
    @staticmethod
    def assert_object_implementer(obj):
        if not IObject.providedBy(obj):
            raise SimulationError('Object must implement IObject interface')
    
    @two_step_init
    def __init__(self):
        self._children = {}
        self.parent = None
        self.name = None
    
    def init(self, parent, name):
        Object._create(self, parent, name)
        
    def update(self, time_since_last_update):
        for child in self._children.itervalues():
            child.update(time_since_last_update)
        
    def _create(self, parent, name):
        self._children = {}
        self.parent = parent
        self.name = name
        
        if self.parent is not None:
            self.assert_object_implementer(parent)
            self.parent.add_child(self)
            
    # IStorable Methods
    # IStorable Methods
    def load(self, data_object):
        """
        @type  data_object: tuple
        @param data_object: (parent, kml_node)
        """
        parent, node = data_object
        Object._create(self, parent, node['name'])
        
    def save(self, data_object):
        raise "Not yet implemented"
        
    def get_child(self, name):
        if not self._children.has_key(name):
            raise SimulationError('Object does not have child %s' % name)
        
        return self._children[name]
        
    def add_child(self, child):
        self.assert_object_implementer(child)
        
        self._children[child.name] = child
    
    def remove_child(self, child):
        actual_child = child
        # If its not an implementer of IObject
        if not IObject.providedBy(child):
            actual_child = self.get_child(child)
            
        del self._children[actual_child.name]
