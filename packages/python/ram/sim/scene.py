# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   packages/python/ram/sim/scene.py


# Python imports
import imp
import sys
import os.path

# Library Ipmorts
import yaml
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt

# Project Imports
import ram.core as core
import ram.sim.defaults as defaults
from ram.sim.util import SimulationError
from ram.sim.serialization import ModuleLoader, parse_position_orientation, two_step_init, parse_orientation
from ram.sim.physics import World, Body, ITrigger, IBody
from ram.sim.graphics import Camera, CameraController, Visual, IVisual
from ram.sim.robot import Robot, IThruster
from ram.core import fixed_update, log_init

class SceneError(SimulationError):
    pass


class ISceneLoader(core.Interface):
    """
    Provides an abstract interface to loading a scene.
    """
    
    def can_load(name):
        """
        This is a static method of the class it looks at the name to determine
        if the scene is of a type it can load.
        
        @type name: string
        @param name: The absolute path to the scene file
        
        @rtype: bool
        @return: True if loader can load the scene, false if not
        """
        pass
    
    def load(scene_file, scene):
        """
        Loads the scene from the file into the given scene object.
        
        @type scene_file: string
        @param scene_file: The absolute path to the scene file
        
        @type scene: Scene
        @param scene: The scene into which the objects should be placed
        """
        pass



class Scene(object):
    """
    This provides access to all objects in the scene, be they cameras, robots,
    or just obstacles.
    """
    
    scene_loaders = core.ExtensionPoint(ISceneLoader)    
    
    #@log_init(defaults.sim_log_config)
    def __init__(self, name, scene_data):
        """
        @type name: string
        @param name: The name of scene
        
        @type  scene_data: Anything
        @param scene_data: The object that will be passed to the scene loader
        """
        self.name = name
        self._objects = []
        self._robots = {}
        self._cameras = {}
        self._camera_controllers = []
        self._physics_update_interval = 1.0 / 60
        
        # Create Ogre SceneManager and OgreNewt World
        self._scene_manager = \
            Ogre.Root.getSingleton().createSceneManager(Ogre.ST_GENERIC,
                                                        self.name)
        self._world = World()
        
        # Search for a scene loader compatible with file format then load it
        loaders = [loader for loader in self.scene_loaders \
                   if loader.can_load(scene_data) ]
        
        if len(loaders) == 0:
            raise SceneError('No Loader found for "%s".' % scene_file)
        
        loader = loaders[0]()
        loader.load(scene_data, self)
    
    def getObjectsByInterface(self, iface):
        devs = []
        for obj in self._objects:
            if iface in core.providedBy(obj):
                devs.append(obj)
        return devs
    
    def destroy(self):
        """
        Release all resources held by objects
        """
        
        # Release all references
        print 'Release references to scene objects'
        del self._objects
        del self._robots
        del self._cameras
        del self._camera_controllers
        
        # Shutdown the world
        self._world.shutdown()
        
        print 'Release reference to world object'
        # Not deleting this here seems to work out properly?
        #del self._world
        print 'Done'

    class scene_mgr(core.cls_property):
        """
        The Ogre scene manager for the scene
        """
        def fget(self):
            return self._scene_manager
        
    class world(core.cls_property):
        """
        The Physical world in the simulation
        """
        def fget(self):
            return self._world
           
    def get_camera(self, name):
         return self._cameras.get(name, None)

    def update(self, time_since_last_update):
        self._update_physics(time_since_last_update)

        # Update all of our objects
        for obj in self._objects:
           obj.update(time_since_last_update)
        
        for robot in self._robots.itervalues():
            robot.update(time_since_last_update)
        
        for controller in self._camera_controllers:
            controller.update(time_since_last_update)
    
    @fixed_update('_physics_update_interval')
    def _update_physics(self, time_since_last_update):
        """
        Updates the physics of the sim at a rate near the 
        _physics_update_interval attribute of the object.
        """
        self._world.update(self._physics_update_interval)
        
    def save(self, location):
        raise SceneError("Save not yet implemented")
        # TODO: Finish me
    
    def reload(self):
        raise SceneError("Reload not yet implemented")
        # TODO: Finish me
        
    def add_resource_locations(self, location_map):
        """
        Adds resouce location to the scene internal resource group.  This 
        resources will only be after the scene is loaded initialized.
        
        @type location_map: dict: string to string
        @param location_map: maps resource type, to its location.  Valid types
                             are 'Zip' and 'FileSystem'.
        """
        #self.logger.info('Adding resources locations')
        
#        print 'Adding resource locations'
#        print location_map
        rsrc_mrg = Ogre.ResourceGroupManager.getSingleton()
        for resource_type in location_map:
#            self.logger.info('/tAdding resources of type: %s' % resource_type)
#            print '\tAdding resources of type: %s' % resource_type
            for location in location_map[resource_type]:
#                print '\t',location_map[resource_type]
#                print '\t\t-',location
                  location = os.path.abspath(core.environmentSub(location))
#                  self.logger.info('/t/tAdding location: %s' % location)
#                  print '\t\tAdding location: %s' % location
                  rsrc_mrg.addResourceLocation(location, resource_type,
                                             self.name, False)
        rsrc_mrg.initialiseResourceGroup(self.name)
    
    def create_camera(self, name, position, offset, near_clip = 0.5):
        self._cameras[name] = Camera(name, self, position, offset, near_clip)
        self._camera_controllers.append(CameraController(self._cameras[name]))
    
    def create_object(self, obj_type, *args, **kwargs):
        """
        Creates an object

        @type  obj_type: Interface
        @param obj_type: The interface
        """
        kwargs['scene'] = self
        core.Component.create(obj_type, *args, **kwargs)
        
    def create_robot(self, config_path):
        robot = Robot(self, config_path)
        self._robots[robot.name] = robot
        return robot


class ISceneObject(core.Interface):
    pass

class SceneObject(Body, Visual):
    # Inherits IBody, IVisual, IKMLStorable
    core.implements(ISceneObject)
            
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
        Visual.load(self, data_object)
        Body.load(self, data_object)
        
        # Link body and node toghether
        self._body.attachToNode(self._node)
        buoyantPlaneUp = defaults.gravity.normalisedCopy() * -1;
        self.set_buoyancy(buoyantPlaneUp)
        
        self._body.angularDamping = (10,10,10)
        #print self._body.angularDamping
        #self.gravity = (0,0,0)
        
    def save(self, data_object):
        raise "Not yet implemented"
            
    
class ModuleSceneLoader(core.Component, ModuleLoader):
    """
    Loads a scene by loading a python module from file and executing the
    create_scene method.
    
    B{Implements:}
     - ISceneLoader
    """
    
    core.implements(ISceneLoader)
    
    # ISceneLoader Methods
    def load(self, scene_file, scene):
        """
        Uses the python imp module to load the module given the path to it.
        """
        scene_mod = ModuleLoader.load(self, scene_file)
        scene._bodies = scene_mod.create_scene(self, scene)
    
    # End ISceneLoader Methods
    
class KMLSceneLoader(core.Component):
    core.implements(ISceneLoader)

    def __init__(self):
        self._light_count= 0

    @staticmethod
    def can_load(scene_data):
        try:
            if scene_data.endswith('.sml') and len(scene_data) > 4:
                return True
            return False
        except AttributeError:
            return False
        
    def load(self, scene_file, scene):
        self._scene_mgr = scene.scene_mgr
        self._scene = scene
        
        try:
            config = yaml.load(file(scene_file))
        except yaml.YAMLError, e:
            raise SceneError, str(e)
        kml_node = config['Scene']
        
        # Load resources
        scene.add_resource_locations(kml_node['Resources'])
        
        # Setup skybox
        if kml_node.has_key('SkyBox'):
            self._create_sky_box(kml_node['SkyBox'])
        
        # Load Lights
        if kml_node.has_key('Lights'):
            lights_node = kml_node['Lights']
            for name, node in lights_node.iteritems():
                node['name'] = name
                self._create_light(node)
        else:
            #raise SceneError, 'Scene must have lights'
            pass
        
        ambient_light_colour = kml_node.get('ambient_light_colour', 
                                             None)
        if ambient_light_colour is not None:
            ambient_light_colour = Ogre.ColourValue(*ambient_light_colour)
            self._scene_mgr.setAmbientLight(ambient_light_colour)
        #self._scene_mgr.setAmbientLight(Ogre.ColourValue(1,1,1))
            
        # Load cameras
        if kml_node.has_key('Cameras'):
            for name, node in kml_node['Cameras'].iteritems():
                node['name'] = name
                self._create_camera(node)
        else:
            raise SceneError, 'Scene must have cameras'
        
        # Load Objects
        objects = kml_node.get('Objects', None)
        if objects is not None:
            for name, node in objects.iteritems():
                iface_name, class_name = node['type']
                obj = core.Component.create(iface_name, class_name)
                node['name'] = name
                obj.load((scene, None, node))
                
                # TODO: Put this up in the scene
                scene._objects.append(obj)
        
        if kml_node.has_key('Robots'):
            for name, node in kml_node['Robots'].iteritems():
                if not hasattr(node, 'has_key'):
                    scene.create_robot(core.environmentSub(node))
                else:
                    path = node['path']
                    robot = scene.create_robot(core.environmentSub(path))
                
                    # Change position and orientaiton if needed
                    position, orientation = parse_position_orientation(node)
                    robot._main_part._body.setPositionOrientation(position, 
                                                                  orientation)


                
    def _create_sky_box(self, node):
        """
        Parse the needed information to create a sky box out of a kml node 
        defaults values shown below::
        
          SkyBox:
              enable: True
              material_name: <Must Be Given>
              distance: 5000
              draw_first: True
              orientation: Quaternion.Identity
              group_name: General
        
        """
        enable = node.get('enable', True)
        material_name = node['material_name']
        distance = node.get('distance', 5000)
        draw_first = node.get('draw_first', True)
        orientation = parse_orientation(node)
        group_name = node.get('group', Ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,)
            
        self._scene_mgr.setSkyBox(enable, material_name, distance, 
                                  draw_first, orientation, group_name)
    
    # TODO: Move me into object heirarchy
    def _create_light(self, node):
        """
        Creates a light from a kml node
        """
        # Name (generate one if needed)
        default_name = 'Light' + str(self._light_count)
        name = node.get('name', default_name)
        if name == default_name:
            self._light_count += 1
        
        # Type (look up type based on member of Ogre.Light class
        type = node.get('type', None)
        if type is not None:
            try:
                type = Ogre.Light.__dict__[type]
            except:
                raise SceneError, '"%s" is not a valid light type' % type
        else:
            type = Ogre.Light.LT_POINT
        
        position = node.get('position', Ogre.Vector3.ZERO)
        
        # TODO: Add parsing for the rest of the parameters 
        #       (based on the type of light given)
        light = self._scene_mgr.createLight(name)
        light.setType(type)
        light.setPosition(position)
        
        light_colour = node.get('colour', None)
        if light_colour is not None:
            light_colour = Ogre.ColourValue(*light_colour)
            light.setDiffuseColour(light_colour)
        
    # TODO: move me into object heirarchy    
    def _create_camera(self, node):
        name = node['name']
        position = node.get('position', Ogre.Vector3.ZERO)
        offset = node.get('offset', (0,1,0))
        near_clip = node.get('near_clip', 0.5)
        
        self._scene.create_camera(name, position, offset, near_clip)
        
        
