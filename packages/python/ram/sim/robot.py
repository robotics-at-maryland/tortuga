# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/sim/robot.py

# Library Imports
import ogre.renderer.OGRE as Ogre
import yaml

# Project Impports
import core
from ram.sim.util import Vector, Quat, SimulationError
from ram.sim.serialization import IKMLStorable, KMLLoader, two_step_init, parse_position_orientation
from ram.sim.physics import IBody, Body
from ram.sim.graphics import IVisual, Visual
from ram.sim.object import Object

class IPart(IBody, IVisual):
    """
    A part of a robot
    """
    
    robot = core.Attribute("""The robot this part belongs to""")

class IThruster(IVisual):
#    def __init__(part):
#        """
#        @type part: implements IBody
#        @param part: The part to apply the force to
#        """
    force = core.Attribute("""The force the thruster is producing in Newtons""")
    direction = core.Attribute("""The direction the thruster is applied""")
    max_force = core.Attribute("""The maximum force the thruster can produce in Newtons""")
    min_force = core.Attribute("""The minimum force the thruster can produce in Newtons""")

class IRobot(core.Interface):
    pass
    
# TODO: Improve the documentation of this class and join it with a common 
#       Interface base clas.
class IRobotLoader(core.Interface):
    """
    Loads a robot.
    """
    def can_load(robot_data):
        pass
    
    def load(robot, scene, robot_data):
        pass

# TODO: Break some of this functionality out into a place it can be used by the 
# scene loader
class KMLRobotLoader(core.Component, KMLLoader):
    """
    Loads a robot from our structured KML Loader
    """
    
    core.implements(IRobotLoader)
    
    iface_map = {'Thruster' : IThruster}
    
    @staticmethod
    def can_load(robot_data):
        try:
            if robot_data.endswith('.rml') and len(robot_data) > 4:
                return True
            return False
        except AttributeError:
            return False
        
    def load(self, robot, scene, robot_data):
        config = yaml.load(file(robot_data))
        rbt_node = config['Robot']
        
        # Load main part
        robot.name = rbt_node['name']
        main_part = Part()
        main_part.load((scene, None, rbt_node))
        robot._main_part = main_part
        
        # Load child parts
        parts = rbt_node.get('Parts', None)
        if parts is not None:
            for part_node in parts.itervalues():
                iface_name, class_name = part_node['type']
                iface = self.iface_map[iface_name]
                part = core.Component.create(iface, class_name)
                part.load((scene, robot._main_part, part_node))
                
class Robot(core.Component):
    
    robot_loaders = core.ExtensionPoint(IRobotLoader)
    
    def __init__(self, scene, robot_data):
        """
        @type name: string
        @param name: The name of robot
        
        @type  robot_data: Anything
        @param robot_data: The object that will be passed to the robot loader
        """
        self.name = None
        self._main_part = None
        
        # Search for a scene loader compatible with file format then load it
        loaders = [loader for loader in self.robot_loaders \
                   if loader.can_load(robot_data) ]
        
        if len(loaders) == 0:
            raise SceneError('No Loader found for "%s".' % robot_data)
        
        loader = loaders[0]()
        loader.load(self, scene, robot_data)
    
    # Provides Robot.<part_name> access to parts
    class part_lookup(object):
        def __init__(self, children):
            self._children = children
        def __getattr__(self, attr):
            part = self._children.get(attr, None)
            if part is not None:
                return part
            else:
                raise AttributeError, "Robot does not have part '%s'" % attr
    
    class parts(core.cls_property):
        def fget(self):
            return Robot.part_lookup(self._main_part._children)
        
    def update(self, time_since_last_update):
        self._main_part.update(time_since_last_update)

    def save(self, location):
        raise SceneError("Save not yet implemented")
        # TODO: Finish me
    
    def reload(self):
        raise SceneError("Reload not yet implemented")
        # TODO: Finish me
    
class Part(Body, Visual):
    # Inherits IBody, IVisual, IKMLStorable
            
    @two_step_init
    def __init__(self):
        Body.__init__(self)
        Visual.__init__(self)
            
    def init(self, parent, name, scene, shape_type, shape_props, mass, mesh, 
             material, position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY,
             scale = Ogre.Vector3(1,1,1)):
        
        Body.init(self, None, name, scene, mass, shape_type, 
                      shape_props, position, orietnation)
        Visual.init(self, None, name, scene, mesh, material, position, 
                    orietnation, scale)
    
    def load(self, data_object):
        Body.load(self, data_object)
        Visual.load(self, data_object)
        # Link body and node toghether
        self._body.attachToNode(self._node)
        self.set_buoyancy((0,1,0))
        
        self._body.angularDamping = (10,10,10)
        print self._body.angularDamping
        #self.gravity = (0,0,0)
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class Thruster(Visual):
    # Inherits IBody, IVisual, IKMLStorable
    core.implements(IThruster)
         
    @two_step_init
    def __init__(self):
        self.direction = Ogre.Vector3.UNIT_Z
        self._force_pos = Ogre.Vector3.ZERO
        self._force = 0
        self.max_force = 0
        self.min_force = 0
        Visual.__init__(self)
            
    def init(self, parent, name, scene, direction, min_force, max_force,
             material, position = Ogre.Vector3.ZERO, 
             orientation = Ogre.Quaternion.IDENTITY,
             scale = Ogre.Vector3(1,1,1)):
        
        core.verifyClass(IPart, parent)
        #Visual.init(self, parent, name, scene, mesh, material, position, 
        #            orietnation, scale)
        # Switch me back after the scene node issue is fixed
        Object.init(parent, name)
        
        Thruster._create(self, scene, min_force, max_force, direction, mesh, 
                         material, position, orientation, scale)
        
        
    class force(core.cls_property):
        def fget(self):
            return self._force
        def fset(self, force):
            # Limit thrust to min and max values
            if force < self.min_force:
                self._force = self.min_force
            elif force > self.max_force:
                self._force = self.max_force 
            else:
                self._force = force
                 
    def update(self, time_since_last_frame):
        force = Ogre.Vector3(self.direction) * self._force
        self.parent.add_local_force(force, self._force_pos)
        
        # Redraw force lines
        self._thrust_line.beginUpdate(0)
        self._draw_thrust_line()
        self._thrust_line.end()
    
    def _draw_thrust_line(self):
        base_pt = Ogre.Vector3(0,0,0)
        thrust_pt = (base_pt + self._node.orientation * self.direction) * self._force
        self._thrust_line.position(base_pt)
        self._thrust_line.position(thrust_pt) 
    
    #def _create(self, direction):
    def _create(self, scene, direction, min_force, max_force, mesh, material, 
                position, orientation, scale):
        self.direction = direction
        self.min_force = min_force
        self.max_force = max_force
        # Uncomment when python ogre gets updated
        # Reparent self to node
#        parent_node = self._node.getParentSceneNode()
#        parent_node.removeChild(self._node)
#        self.parent._node.addChild(self._node)
#    
        entity = scene.scene_mgr.createEntity(self.name, mesh)
        entity.setMaterialName(material)
        
        self._node = self.parent._node.createChildSceneNode()
        self._node.attachObject(entity)

        pscale = self.parent._node.scale;
        iscale = Ogre.Vector3((1 / pscale.x),(1 / pscale.y),(1 / pscale.z))

        if scale != Ogre.Vector3(1,1,1):
            self._node.setScale(scale * iscale)
            entity.setNormaliseNormals(True)       
        
        self._node.position = Ogre.Vector3(position) * iscale
        self._node.orientation = orientation
        
        self._thrust_line = scene.scene_mgr.createManualObject(self.name + "manual");
        self._thrust_line.dynamic = True
        
        self._thrust_line.begin("BaseRedNoLighting", Ogre.RenderOperation.OT_LINE_STRIP);
        self._draw_thrust_line()
        self._thrust_line.end()
                
        self._node.attachObject(self._thrust_line)
        
    def load(self, data_object):
        #Visual.load(self, data_object)
        # Remove me when scene node parenting issue is fixed
        scene, parent, kml_node = data_object
        Object.load(self, (parent, kml_node))
        
        gfx_node = kml_node['Graphical'] 
        mesh = gfx_node['mesh']
        material = gfx_node['material']
        scale = Ogre.Vector3(gfx_node['scale'])
        
        position, orientation = parse_position_orientation(kml_node)
        self._force_pos = position
        
        direction = Ogre.Vector3(kml_node['direction'])
        min_force = kml_node['min_force']
        max_force = kml_node['max_force']
        Thruster._create(self, scene, direction, min_force, max_force, mesh, 
                         material, position, orientation, scale)

        