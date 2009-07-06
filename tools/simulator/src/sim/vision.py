# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/src/sim/vision.py

# STD Imports
import math
import ctypes
import random

# Library Imports
import ogre.renderer.OGRE as ogre

# Project Imports
import ext.core
import ext.vision
import ext.math
from ext.vision import Symbol
from sim.subsystems import Simulation
from ext.vehicle import IVehicle 

import ram.sim
import ram.core as core
import ram.timer
from ram.sim.object import IObject
from ram.sim.graphics import IVisual, Visual
from ram.sim.serialization import two_step_init
from ram.sim.serialization import parse_position_orientation
import ram.sim.object

class IBuoy(IObject):
    """ An object which you can see in the simulation"""

class IPipe(IObject):
    """ An object which you can see in the simulation"""
    pass

class IBarbedWire(IObject):
    """ An object which you can see in the simulation"""
    pass

class IBin(IObject):
    """ An object which you can see in the simulation"""
    pass

class ITarget(IObject):
    """ An object which you can see in the simulation"""
    pass

class IDuct(IObject):
    """ An object which you can see in the simulation"""
    pass

class ISafe(IObject):
    """ An object which you can see in the simulation"""
    pass

class ICourse(IObject):
    """
    Generates a course based on the given input, can apply randomness to said
    input.  This allows an entire new configurtion to be built very easily
    """
    pass

# TODO: Fill out the methods for the class

class Buoy(Visual):
    core.implements(IVisual, IBuoy)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'sphere.50cm.mesh', 
                       'scale' : [0.15, 0.15, 0.15],
                       'material' : 'Simple/Red' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class Pipe(Visual):
    core.implements(IVisual, IPipe)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)
        self._visible = False
        self.id = 0

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'cube.1m.mesh', 
                       'scale' : [1.2192, 0.1524, 0.0254],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class BarbedWire(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, IBarbedWire)
    
    SEPERATION = 1.2192 # 4 ft
    WIDTH = 1.8288 # 6 ft
    ROPE_LENGTH = 5 # 16.4 ft
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._front = None
        self._back = None
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation

    @property
    def front(self):
        return self._front

    @property
    def back(self):
        return self._back
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        frontBackOffset = \
            orientation * ogre.Vector3(BarbedWire.SEPERATION/2.0, 0, 0)
        ropeOffset = orientation * ogre.Vector3(0, BarbedWire.WIDTH/2.0, 0)
        ropeVertOffset = \
            orientation * ogre.Vector3(0, 0, -BarbedWire.ROPE_LENGTH/2.0)


        drawOrientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Z)
        ropeDrawOrientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Y)

        
        # Shared graphics node
        gfxNode = {'mesh': 'cylinder.mesh', 
                   'material' : 'Simple/Green',
                   'scale': [BarbedWire.WIDTH, 0.0508/2, 0.0508/2] }

        ropeGfxNode = {'mesh': 'cylinder.mesh', 
                       'material' : 'Simple/Black',
                       'scale': [BarbedWire.ROPE_LENGTH, 0.0508/3, 0.0508/3] }

        
        # Create Front Pipe
        position = basePos + (frontBackOffset * -1)
        cfg = {'name' : baseName + 'BarbedWireFront', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))

        # Create Front Left Rope
        position = basePos + (frontBackOffset * -1) + ropeOffset + \
                   ropeVertOffset
        cfg = {'name' : baseName + 'BarbedWireFrontLeftRope', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(ropeDrawOrientation) ,
               'Graphical' : ropeGfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))

        # Create Front Right Side
        position = basePos + (frontBackOffset * -1) - ropeOffset + \
                   ropeVertOffset
        cfg = {'name' : baseName + 'BarbedWireFrontRightRop', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(ropeDrawOrientation) ,
               'Graphical' : ropeGfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))

        
        # Create Back Pipe
        position = basePos + (frontBackOffset * 1)
        cfg = {'name' : baseName + 'BarbedWireBack', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        self._back = Visual()
        self._back.load((scene, parent, cfg))
        
        # Create Back Left Rope
        position = basePos + (frontBackOffset * 1) + ropeOffset + \
                   ropeVertOffset
        cfg = {'name' : baseName + 'BarbedWireBackLeftRope', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(ropeDrawOrientation) ,
               'Graphical' : ropeGfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))

        # Create Back Right Side
        position = basePos + (frontBackOffset * 1) - ropeOffset + \
                   ropeVertOffset
        cfg = {'name' : baseName + 'BarbedWireBackRightRop', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(ropeDrawOrientation) ,
               'Graphical' : ropeGfxNode}
        self._front = Visual()
        self._front.load((scene, parent, cfg))


class Target(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, ITarget)
    
    # The size of the target opening
    SEPERATION = 0.4572
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._position = ogre.Vector3.ZERO
        self._orientation = ogre.Quaternion.IDENTITY
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        sideOffset = orientation * ogre.Vector3(0, Target.SEPERATION/2, 0)
        upDownOffset = orientation * ogre.Vector3(0, 0, Target.SEPERATION/2)
        drawOrientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Z)
        
        # Shared graphics node
        gfxNode = {'mesh': 'cylinder.mesh', 
                   'material' : 'Simple/Green',
                   'scale': [0.508, 0.0508/2, 0.0508/2] }
        
        # Create Top Pipe
        position = basePos + (upDownOffset * 1)
        cfg = {'name' : baseName + 'TargetTop', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))
        
        # Create Bottom Pipe
        position = basePos + (upDownOffset * -1)
        cfg = {'name' : baseName + 'TargetBottom', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))

        # Flip them so they point up and down
        drawOrientation = drawOrientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_Y)
        
        # Create Left Pipe
        position = basePos + (sideOffset * 1)
        cfg = {'name' : baseName + 'TargetLeft', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))
        
        # Create Right Pipe
        position = basePos + (sideOffset * -1)
        cfg = {'name' : baseName + 'TargetRight', 
               'position' : position, 
               'orientation' : self._toAxisAngleArray(drawOrientation) ,
               'Graphical' : gfxNode}
        pipe = Visual()
        pipe.load((scene, parent, cfg))

class Bin(Visual):
    """
    Represents a single competition bin
    """
    core.implements(IVisual, IBin)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)
        self._symbol = Symbol.NONEFOUND
        self._visible = False
        self.id = 0

    def load(self, data_object):
        scene, parent, node = data_object
        # Automatically fill in the graphical data
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'box.mesh', 'scale' : [0.9144, 0.6096, 0.0254]}
            
            # Determine symbol material
            material = 'CompElement/Bin'
            
            _type = node.get('symbol', '').lower()
            self._symbol = Symbol.NONEFOUND
            possibleSymbols = set(['heart','spade','club','diamond', 'ship',
                                   'factory', 'aircraft', 'tank'])
            if _type in possibleSymbols:
                # Added the symbol name to the base (first letter caps
                material = 'CompElement/' + _type[0].upper() + _type[1:] + 'Bin'
                
                # Set symbol type
                typeToFlag = {'club' : Symbol.CLUB, 
                              'heart' : Symbol.HEART,
                              'spade' : Symbol.SPADE, 
                              'diamond' : Symbol.DIAMOND,
                              'ship' : Symbol.SHIP,
                              'factory' : Symbol.FACTORY,
                              'aircraft' : Symbol.AIRCRAFT,
                              'tank' : Symbol.TANK}

                self._symbol = typeToFlag[_type]

            gfxNode['material'] = material
                
            # Load the node
            node['Graphical'] = gfxNode
            

        else:
            self._symbol = Symbol.NONEFOUND
        Visual.load(self, (scene, parent, node))
        
    def save(self, data_object):
        raise "Not yet implemented"
    
    @property
    def symbol(self):
        return self._symbol
    
class BlackJackTable(ram.sim.object.Object):
    """
    Represents the array of bins in the competition
    """
    core.implements(ram.sim.object.IObject)
    
    SEPERATION = 0.64008
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
        self._farLeftBin = None
        self._leftBin = None
        self._rightBin = None
        self._farRightBin = None

    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]

    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basePos, orientation = parse_position_orientation(node)
        basePos = ram.sim.OgreVector3(basePos)
        baseOffset = orientation * ogre.Vector3(0, BlackJackTable.SEPERATION, 0)
        baseName = node['name']
        
        symbols = node.get('symbols', ['heart','spade','club','diamond'])
        while len(symbols) < 4:
            symbols.append('')
        
        # Create far left bin
        self._farLeftBin = Bin()
        position = basePos + (baseOffset * -1.5)
        cfg = {'name' : baseName + 'FarLeftBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'symbol' : symbols[0]}
        self._farLeftBin.load((scene, parent, cfg))
        scene._objects.append(self._farLeftBin)
        
        # Create left bin
        self._leftBin = Bin()
        position = basePos + (baseOffset * -0.5)
        cfg = {'name' : baseName + 'LeftBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'symbol' : symbols[1]}
        self._leftBin.load((scene, parent, cfg))
        scene._objects.append(self._leftBin)

        # Create right bin
        self._rightBin = Bin()
        position = basePos + (baseOffset * 0.5)
        cfg = {'name' : baseName + 'RightBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'symbol' : symbols[2]}
        self._rightBin.load((scene, parent, cfg))
        scene._objects.append(self._rightBin)

        # Create right bin
        self._farRightBin = Bin()
        position = basePos + (baseOffset * 1.5)
        cfg = {'name' : baseName + 'FarRightBin', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation),
               'symbol' : symbols[3]}
        self._farRightBin.load((scene, parent, cfg))
        scene._objects.append(self._farRightBin)
        
        
    def save(self, data_object):
        raise "Not yet implemented"
    
class AirDuct(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, IDuct)
    
    SEPERATION = 0.762
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
    
    def _toAxisAngleArray(self, orientation):
        angle = ogre.Degree(0)
        vector = ogre.Vector3()
        orientation.ToAngleAxis(angle, vector)
        return [vector.x, vector.y, vector.z, angle.valueDegrees()]
    
    @property
    def position(self):
        return self._position
    
    @property
    def orientation(self):
        return self._orientation
    
    def load(self, data_object):
        scene, parent, node = data_object
        ram.sim.object.Object.load(self, (parent, node))
        
        # Parse config information
        basepos, orientation = parse_position_orientation(node)
        self._position = ram.sim.OgreVector3(basepos)
        self._orientation = orientation
        basePos = ram.sim.OgreVector3(basepos)
        baseName = node['name']
        
        sideOffset = orientation * ogre.Vector3(0, AirDuct.SEPERATION, 0)
        upDownOffset = orientation * ogre.Vector3(0, 0, AirDuct.SEPERATION)
        
        # Shared graphics node
        gfxNode = {'mesh': 'box.mesh', 'material' : 'CompElement/AirDuctSide',
                   'scale': [1.524, 1.524, 0.003175] }
        
        # Create Bottom
        position = basePos + (upDownOffset * -1)
        botGfx = gfxNode.copy()
        botGfx['material'] = 'CompElement/AirDuctBottom'
        cfg = {'name' : baseName + 'DuctBottom', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : botGfx}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Top
        position = basePos + (upDownOffset * 1)
        cfg = {'name' : baseName + 'DuctTop', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        orientation = orientation * ogre.Quaternion(
            ogre.Degree(90), ogre.Vector3.UNIT_X)
        
        # Create Right
        position = basePos + (sideOffset * -1)
        cfg = {'name' : baseName + 'DuctRight', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Left
        position = basePos + (sideOffset * 1)
        cfg = {'name' : baseName + 'DuctLeft', 'position' : position, 
               'orientation' : self._toAxisAngleArray(orientation) ,
               'Graphical' : gfxNode}
        side = Visual()
        side.load((scene, parent, cfg))
        
        # Create Left
#        position = basePos + (sideOffset * -1)
#        cfg = {'name' : baseName + 'DuctTop', 'position' : position, 
#               'orientation' : self._toAxisAngleArray(orientation) ,
#               'Graphical' : gfxNode}
#        side = Visual()
#        side.load((scene, parent, cfg))

class Safe(Visual):
    core.implements(IVisual, ISafe)
    
    @two_step_init
    def __init__(self):
        Visual.__init__(self)

    def load(self, data_object):
        scene, parent, node = data_object
        if not node.has_key('Graphical'):
            # Default mesh and scale info
            gfxNode = {'mesh' : 'cube.1m.mesh', 
                       'scale' : [0.3048, 0.3048, 0.3048],
                       'material' : 'Simple/Orange' }
            node['Graphical'] = gfxNode
        Visual.load(self, (scene, parent, node))

class Course(ram.sim.object.Object):
    core.implements(ram.sim.object.IObject, ICourse)
    
    @two_step_init
    def __init__(self):
        ram.sim.object.Object.__init__(self)
    
    def load(self, data_object):
        scene, parent, cfg = data_object
        ram.sim.object.Object.load(self, (parent, cfg))
        
        startType = cfg['startType']
        startName = cfg['start']
        startPos = cfg['startPos']
        startDepth = self._readRandom(cfg.get('startDepth', 0))
        curHeading = self._readRandom(cfg.get('startHeading', 0))
        curPos = ext.math.Vector2(float(startPos[0]), float(startPos[1]))
        seed = cfg.get('seed', -1)

        if -1 != seed:
            random.seed(seed)
        
        # Create start object
        startObjCfg = {
            'name' : startName,
            'type' : startType,
            'position' : [curPos.x, curPos.y, startDepth],
            'orientation' : [0, 0, 1, -curHeading],
        }
        self._createObject(scene, startObjCfg)
        
        # Create all objects recursively
        if cfg.has_key(startName):
            self._createObjectives(scene, cfg, cfg[startName], curPos,
                                   curHeading)
    
    def _createObjectives(self, scene, mainCfg, sectionCfg, startPos, 
                          startHeading):
        
        for name in sectionCfg.iterkeys():
            # Read in the configuration information
            pCfg = sectionCfg[name]
            type_ = pCfg['type']
            
            # Define usable variables
            rotMatrix = ext.math.Matrix2()
                
            curPos = startPos
            curHeading = self._readRandom(pCfg.get('direction',0)) + startHeading
                
            # New current position
            if pCfg.has_key('distance'):
                distToNext = self._readRandom(pCfg['distance'])
                rotMatrix.fromAngle(ext.math.Degree(curHeading))
                offset = rotMatrix * ext.math.Vector2.UNIT_X * distToNext
                curPos = curPos + offset
                
            # Add in the offset
            if pCfg.has_key('offset'):
                offsetDist = pCfg['offset']
                rotMatrix.fromAngle(ext.math.Degree(-90 + curHeading))
                offset = rotMatrix * ext.math.Vector2.UNIT_X * offsetDist
                curPos = curPos + offset

            # Get the final position
            pos = [curPos.x, curPos.y, self._readRandom(pCfg['depth'])]

            # New heading
            newHeading = self._readRandom(pCfg.get('heading',0)) + curHeading

            # Build the configuration and load the object
            objCfg = {
                'name' : name,
                'type' : type_,
                'position' : pos,
                'orientation' : [0, 0, 1, -newHeading],
            }

            # Add the other options into the config
            notToAdd = set(['heading', 'depth', 'distance', 'offset', 'type'])
            for key,value in pCfg.iteritems():
                if key not in notToAdd:
                    objCfg[key] = value
            
            self._createObject(scene, objCfg)

            # Determine the new current position and heading
            if mainCfg.has_key(name):
                # Create all objects recursively
                self._createObjectives(scene, mainCfg, mainCfg[name], curPos, 
                                       newHeading)
    
    def _readRandom(self, val):
        if type(val) is list:
            val = random.uniform(val[0], val[1])
        return val
        
    def _createObject(self, scene, cfg):
        iface_name, class_name = cfg['type']
        obj = core.Component.create(iface_name, class_name)
        obj.load((scene, None, cfg))
        scene._objects.append(obj)
        
class IdealSimVision(ext.vision.VisionSystem):
    def __init__(self, config, deps):
        # Transform arguments to create base VisionSystem class
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)

        # The camera do nothing
        ext.vision.VisionSystem.__init__(self, ext.vision.Camera(640, 480),
                                         ext.vision.Camera(640, 480), cfg,
                                         depList)
        
        
        # Grab the scene we are operating in
        sim = ext.core.Subsystem.getSubsystemOfType(Simulation, deps, 
                                                    nonNone = True)
        vehicle = ext.core.Subsystem.getSubsystemOfType(IVehicle, deps, 
                                                        nonNone = True)
        self.robot = vehicle.getDevice('SimulationDevice').robot
        self._horizontalFOV = config.get('horizontalFOV', 107)
        self._verticalFOV = config.get('verticalFOV', 78)
        
        # Red light detector variables
        self._runRedLight = False
        self._foundLight = False
        
        # Orange pipe detector variables
        self._runOrangePipe = False
        self._foundPipe = False
        self._pipeCentered = False
        self._pipeID = 1
        
        # Bin detector variables
        self._runBin = False
        self._foundBin = False
        self._binCentered = False
        self._binID = 1
        
        # Duct Detector variables
        self._runDuct = False
        self._foundDuct = False
        
        # Safe Detector variables
        self._runDownSafeDetector = False
        self._foundDownSafe = False
        
        # Target Detector variables
        self._runTarget = False
        self._foundTarget = False

        # BarbedWire Detector Variables
        self._runBarbedWire = False
        self._foundBarbedWire = False
                
        # Find all the Buoys, Pipes and Bins
        self._bouys = sim.scene.getObjectsByInterface(IBuoy)
        self._pipes = sim.scene.getObjectsByInterface(IPipe)
        self._bins = sim.scene.getObjectsByInterface(IBin)
        self._ducts = sim.scene.getObjectsByInterface(IDuct)
        self._safes = sim.scene.getObjectsByInterface(ISafe)
        self._targets = sim.scene.getObjectsByInterface(ITarget)
        self._barbedWires = sim.scene.getObjectsByInterface(IBarbedWire)

    def redLightDetectorOn(self):
        self._runRedLight = True
        self.publish(ext.vision.EventType.RED_LIGHT_DETECTOR_ON,
                     ext.core.Event())
    
    def redLightDetectorOff(self):
        self._runRedLight = False
        self.publish(ext.vision.EventType.RED_LIGHT_DETECTOR_OFF,
                     ext.core.Event())

    def pipeLineDetectorOn(self):
        self._runOrangePipe = True
        self.publish(ext.vision.EventType.PIPELINE_DETECTOR_ON,
                     ext.core.Event())
        
    def pipeLineDetectorOff(self):
        self._runOrangePipe = False
        self.publish(ext.vision.EventType.PIPELINE_DETECTOR_OFF,
                     ext.core.Event())
        
    def binDetectorOn(self):
        self._runBin = True
        self.publish(ext.vision.EventType.BIN_DETECTOR_ON,
                     ext.core.Event())
        
    def binDetectorOff(self):
        self._runBin = False
        self.publish(ext.vision.EventType.BIN_DETECTOR_OFF,
                     ext.core.Event())
        
    def ductDetectorOn(self):
        self._runDuct = True
        self.publish(ext.vision.EventType.DUCT_DETECTOR_ON,
                     ext.core.Event())
        
    def ductDetectorOff(self):
        self._runDuct = False
        self.publish(ext.vision.EventType.DUCT_DETECTOR_OFF,
                     ext.core.Event())

    def downwardSafeDetectorOn(self):
        self._runDownSafeDetector = True
        self.publish(ext.vision.EventType.SAFE_DETECTOR_ON,
                     ext.core.Event())
        
    def downwardSafeDetectorOff(self):
        self._runDownSafeDetector = False
        self.publish(ext.vision.EventType.SAFE_DETECTOR_OFF,
                     ext.core.Event())

    def targetDetectorOn(self):
        self._runTarget = True
        self.publish(ext.vision.EventType.TARGET_DETECTOR_ON,
                     ext.core.Event())
        
    def targetDetectorOff(self):
        self._runTarget = False
        self.publish(ext.vision.EventType.TARGET_DETECTOR_OFF,
                     ext.core.Event())

    def barbedWireDetectorOn(self):
        self._runBarbedWire = True
        self.publish(ext.vision.EventType.BARBED_WIRE_DETECTOR_ON,
                     ext.core.Event())

    def barbedWireDetectorOff(self):
        self._runBarbedWire = False
        self.publish(ext.vision.EventType.BARBED_WIRE_DETECTOR_OFF,
                     ext.core.Event())

    def backgrounded(self):
        return False

    def update(self, timeSinceLastUpdate):
        """
        Checks against the obstacles, and generates the proper events
        """
        if self._runRedLight:
            self._checkRedLight()
        if self._runOrangePipe:
            self._checkOrangePipe()
        if self._runBin:
            self._checkBin()
        if self._runDuct:
            self._checkDuct()
        if self._runDownSafeDetector:
            self._checkDownwardSafe()
        if self._runTarget:
            self._checkTarget()
        if self._runBarbedWire:
            self._checkBarbedWire()
    
    def _findClosest(self, objects):
        """
        Find the closest object to the given position
        
        @note: This is inefficient but the object count is to remain low, so
               we are fine.
        """
        closest = None
        obj = None
        relativePos = None
        robotPos = self.robot.position + \
            (self.robot.orientation * ogre.Vector3(0.5, 0, 0))

        for o in objects:
            toObj = o.position - robotPos
            
            # None yet found, default to this one
            if closest is None:
               closest = (o, toObj)
            else:
                obj, realtivePos = closest
                if toObj.squaredLength() < realtivePos.squaredLength():
                    # Found a better one switch to it
                    closest = (o, toObj)

        return closest
        
    def _downwardCheck(self, relativePos, obj):
        """
        Determines the whether or the object at the given relative position
        is is visible, and if so, it returns its x, y (camera frame cordinates)
        and angle.
        
        @type  relativePos: ogre.renderer.OGRE.Vector3
        @param relativePos: The relative position to the downward object
        
        @rtype:  (bool, double, double, ext.math.Degree)
        @return: (visible, x, y, angle)
        """
        
        camVector = self.robot.orientation * -ogre.Vector3.UNIT_Z
        camVector.normalise()

        # Find Roll (X cordinate)
        forwardVector = self.robot.orientation * ogre.Vector3.UNIT_X
        rollPlane = ogre.Plane(forwardVector, 0) 
        rollVec = rollPlane.projectVector(relativePos).normalisedCopy()
        roll = ogre.Math.ACos(rollVec.dotProduct(camVector)).valueDegrees()
        
        # Find Pitch (Y cordinate)
        rightVector = self.robot.orientation * ogre.Vector3.UNIT_Y
        pitchPlane = ogre.Plane(rightVector, 0) 
        pitchVec = pitchPlane.projectVector(relativePos).normalisedCopy()
        pitch = ogre.Math.ACos(pitchVec.dotProduct(camVector)).valueDegrees()

        # Add in sign
        relativePos = self.robot.orientation.UnitInverse() * relativePos
        if relativePos.y < 0:
            roll *= -1
        if relativePos.x < 0:
            pitch *= -1
        
        # Check to see if its the field of view
        visible = False
        x = 0.0
        y = 0.0
        angle = ext.math.Degree(0.0)
        
        if (math.fabs(roll) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            visible = True
            
            x = roll / (self._horizontalFOV/2) * -1
            y = pitch / (self._verticalFOV/2)
            
            # Find pipe relative angle
            forwardPipe = obj.orientation * ogre.Vector3.UNIT_X
            orientation = forwardVector.getRotationTo(forwardPipe)
            angle = ext.math.Degree(orientation.getRoll(True).valueDegrees())
            
        return (visible, x, y, angle)
        
    def _forwardCheck(self, relativePos, obj):
        """
        Determines the whether or the object at the given relative position
        is is visible, and if so, it returns its x, y (camera frame cordinates)
        and angle.
        
        @type  relativePos: ogre.renderer.OGRE.Vector3
        @param relativePos: The relative position to the downward object
        
        @rtype:  (bool, double, double, ext.math.Degree)
        @return: (visible, x, y, angle)
        """
        
        forwardVector = self.robot.orientation * ogre.Vector3.UNIT_X
        
        quat = forwardVector.getRotationTo(relativePos)
        yaw = -quat.getRoll(True).valueDegrees()
        pitch = quat.getYaw(True).valueDegrees()
        
        # Check to see if its the field of view
        visible = False
        x = 0.0
        y = 0.0
        azimuth = ext.math.Degree(0.0)
        elevation = ext.math.Degree(0.0) 
        angle = ext.math.Degree(0.0)
        
        # Check to see if its the field of view
        if (math.fabs(yaw) <= (self._horizontalFOV/2)) and \
           (math.fabs(pitch) <= (self._verticalFOV/2)):
            visible = True
        
            x = yaw / (self._horizontalFOV/2)
            # Negative because of the corindate system
            y = math.fabs(pitch / (self._verticalFOV/2))
            if relativePos.z < 0:
                y = -y

        
            # These have to be swaped as well
            azimuth = ext.math.Degree(-yaw)
            elevation = ext.math.Degree(-pitch)
        
            # Find relative angle
            forwardObj = obj.orientation * ogre.Vector3.UNIT_X
            orientation = forwardVector.getRotationTo(forwardObj)
            angle = ext.math.Degree(orientation.getRoll(True).valueDegrees())
            
        return (visible, x, y, azimuth, elevation, angle)
        
    def _checkRedLight(self):
        """
        Check for the red light
        """
        # Drop out if we have no buoys
        if self._bouys is None:
            return
        if len(self._bouys) == 0:
            return
        
        # Determine orientation to the buoy
        bouy, relativePos = self._findClosest(self._bouys)
        lightVisible, x, y, azimuth, elevation, angle = \
            self._forwardCheck(relativePos, bouy)

        if lightVisible and (relativePos.length() < 3):
            # Pack data into the event
            event = ext.core.Event()
            event.x = x
            event.y = y
            event.azimuth = azimuth
            event.elevation = elevation
            
            # Convert to feet
            event.range = relativePos.length() * 3.2808399
            
            self.publish(ext.vision.EventType.LIGHT_FOUND, event)

            if relativePos.length() < 0.5:
                self.publish(ext.vision.EventType.LIGHT_ALMOST_HIT,
                             ext.core.Event())
            
        else:
            if self._foundLight:
                self.publish(ext.vision.EventType.LIGHT_LOST, ext.core.Event())

        self._foundLight = lightVisible
        
    def _checkTarget(self):
        """
        Check for the Target (ie. "Machine Gun Nest"), the squareness is just
        an approximation. 
        """
        # Drop out if we have no targets
        if self._targets is None:
            return
        if len(self._targets) == 0:
            return
        
        # Determine orientation to the target
        target, relativePos = self._findClosest(self._targets)
        targetVisible, x, y, azimuth, elevation, angle = \
            self._forwardCheck(relativePos, target)

        if targetVisible and (relativePos.length() < 3):
            # Pack data into the event
            event = ext.core.Event()
            event.x = x
            event.y = y
            event.squareNess =  math.fabs(math.cos(angle.valueRadians()))
            
            # Convert to feet
            event.range = relativePos.length() * 3.2808399
            
            self.publish(ext.vision.EventType.TARGET_FOUND, event)
            
        else:
            if self._foundTarget:
                self.publish(ext.vision.EventType.TARGET_LOST, ext.core.Event())

        self._foundTarget = targetVisible

    def _checkBarbedWire(self):
        """
        Check for the Barbed Wire. 
        """
        # Drop out if we have no Barbed Wire
        if self._barbedWires is None:
            return
        if len(self._barbedWires) == 0:
            return
        
        # Grab both pipes determine the relative angle of the whole barbed
        # wire set
        barbedWire, relativePos = self._findClosest(self._barbedWires)
        visible, x, y, azimuth, elevation, angle = \
            self._forwardCheck(relativePos, barbedWire)
        frontPipe = barbedWire.front
        backPipe = barbedWire.back

        # Get stats on both pipes
        relPosFront = frontPipe.position - self.robot.position
        relPosBack = backPipe.position - self.robot.position

        frontVisible, frontX, frontY, azimuth, elevation, frontAngle = \
            self._forwardCheck(relPosFront, frontPipe)
        backVisible, backX, backY, azimuth, elevation, backAngle = \
            self._forwardCheck(relPosBack, backPipe)

        # Fill out default event state variables
        event = ext.core.Event()
        event.bottomX = 0
        event.bottomY = 0
        event.bottomWidth = -1

        # Determine width
        objWidth = math.fabs(math.cos(angle.valueRadians())) * 2
        availFrontWidth = math.fabs(math.tan(math.radians(self._horizontalFOV)) \
             * relPosFront.length())
        availBackWidth = math.fabs(math.tan(math.radians(self._horizontalFOV)) \
             * relPosBack.length())
        frontWidth = objWidth / availFrontWidth
        backWidth = objWidth / availBackWidth
        if frontWidth > 1:
            frontWidth = 1
        if backWidth > 1:
            backWidth = 1

        if (frontVisible and not backVisible) and (relPosFront.length() < 6):
            # Only see the front
            event.topX = frontX
            event.topY = frontY
            event.topWidth =  frontWidth
            
            self.publish(ext.vision.EventType.BARBED_WIRE_FOUND, event)
        if (backVisible and not frontVisible) and (relPosBack.length() < 6):
            # Only see the back
            event.topX = backX
            event.topY = backY
            event.topWidth =  backWidth
            
            self.publish(ext.vision.EventType.BARBED_WIRE_FOUND, event)
        if (frontVisible and backVisible) and (relPosFront.length() < 6) \
               and (relPosBack.length() < 6):
            # See both
            # Assign values based on which pipe is ontop
            if backY < frontY:
                event.topX = frontX
                event.topY = frontY
                event.topWidth =  frontWidth
                event.bottomX = backX
                event.bottomY = backY
                event.bottomWidth =  backWidth
            else:
                event.topX = backX
                event.topY = backY
                event.topWidth =  backWidth
                event.bottomX = frontX
                event.bottomY = frontY
                event.bottomWidth =  frontWidth
            
            self.publish(ext.vision.EventType.BARBED_WIRE_FOUND, event)
        else:
            if self._foundBarbedWire:
                self.publish(ext.vision.EventType.BARBED_WIRE_LOST,
                             ext.core.Event())

        self._foundBarbedWire = frontVisible | backVisible
        
    def _checkOrangePipe(self):
        # Drop out if we have no Pipes
        if self._pipes is None:
            return
        if len(self._pipes) == 0:
            return
        
        found = False
        visibleBins = 0
        robotPos = self.robot.position + \
            (self.robot.orientation * ogre.Vector3(0.5, 0, 0))

        for pipe in self._pipes:
            relativePos = pipe.position - robotPos
            pipeVisible, x, y, angle = self._downwardCheck(relativePos, pipe)

            if pipeVisible and (relativePos.length() < 4.5):
                visibleBins += 1
                found = True

                id = 0
                if not pipe._visible:
                    pipe.id = self._pipeID
                    pipe._visible = True
                    self._pipeID += 1
                else:
                    id = pipe.id
                
                # Publish found event
                event = ext.core.Event()
                event.x = x
                event.y = y
                event.angle = angle
                event.id = id
                self.publish(ext.vision.EventType.PIPE_FOUND, event)
                
            elif pipe._visible:
                event = ext.core.Event()
                event.id = pipe.id
                self.publish(ext.vision.EventType.PIPE_DROPPED, event)
                pipe._visible = False
                
        # Lost Event
        if self._foundPipe and (not found):
            self.publish(ext.vision.EventType.PIPE_LOST, ext.core.Event())

        # Check for centering
        closestPipe, relativePos = self._findClosest(self._pipes)
        pipeVisible, x, y, angle = self._downwardCheck(relativePos, closestPipe)

        toCenter = ogre.Vector2(x, y)
        if pipeVisible and (toCenter.normalise() < 0.08):
            if not self._pipeCentered:
                self._pipeCentered = True
                event = ext.core.Event()
                event.x = x
                event.y = y
                event.angle = angle
                event.id = closestPipe.id

                self.publish(ext.vision.EventType.PIPE_CENTERED, event)
        else:
            self._pipeCentered = False

        self._foundPipe = found  
      
        
    def _checkBin(self):
        # Drop out if we have no Bins
        if self._bins is None:
            return
        if len(self._bins) == 0:
            return
        
        found = False
        visibleBins = 0
        lastAngle = ext.math.Degree(0)
        robotPos = self.robot.position + \
            (self.robot.orientation * ogre.Vector3(0.5, 0, 0))

        for bin in self._bins:
            relativePos = bin.position - robotPos
            binVisible, x, y, angle = self._downwardCheck(relativePos, bin)

            if binVisible and (relativePos.length() < 4.5):
                visibleBins += 1
                found = True

                id = self._binID
                self._binID += 1
                if not bin._visible:
                    bin.id = id
                    bin._visible = True
                else:
                    id = bin.id
                
                # Publish found event
                event = ext.core.Event()
                event.x = x
                event.y = y
                event.angle = angle
                event.symbol = bin.symbol
                event.id = id
                self.publish(ext.vision.EventType.BIN_FOUND, event)
                
                # Record angle for use in the multi bin event
                lastAngle = angle
            elif bin._visible:
                event = ext.core.Event()
                event.id = bin.id
                self.publish(ext.vision.EventType.BIN_DROPPED, event)
                bin._visible = False
                
        # Multi Bin Angle Event
        if visibleBins > 1:
            event = ext.core.Event()
            event.angle = lastAngle
            self.publish(ext.vision.EventType.MULTI_BIN_ANGLE, event)
        
        # Lost Event
        if self._foundBin and (not found):
            self.publish(ext.vision.EventType.BIN_LOST, ext.core.Event())

        # Check for centering

        # Transform to image space
        closestBin, relativePos = self._findClosest(self._bins)
        binVisible, x, y, angle = self._downwardCheck(relativePos, closestBin)

        toCenter = ogre.Vector2(x,y)
        if binVisible and (toCenter.normalise() < 0.2):
            if not self._binCentered:
                self._binCentered = True
                event = ext.core.Event()
                event.id = closestBin.id
                self.publish(ext.vision.EventType.BIN_CENTERED, event)
            else:
                self._binCentered = False

        self._foundBin = found
        
    def _checkDuct(self):
        # Drop out if we have no Ducts
        if self._ducts is None:
            return
        if len(self._ducts) == 0:
            return
        
        duct, relativePos = self._findClosest(self._ducts)
        ductVisible, x, y, azimuth, elevation, angle = \
            self._forwardCheck(relativePos, duct)

        if ductVisible and (relativePos.length() < 8):
            event = ext.vision.DuctEvent(0.0, 0.0, 0.0, 0.0, False, False)
            event.x = x
            event.y = y
            
            # Runs 1 -> 0, getting bigger the closer you go
            event.range = 1 - ((8 - relativePos.length()) / 8.0)
            
            # TODO: Scale me
            angleDeg = angle.valueDegrees() * -1
            if angleDeg > 90:
                angleDeg -= 180
            elif angleDeg < -90:
                angleDeg += 180
            
            event.alignment = angleDeg
            
            # Determine if we are aligned (with perpendicular offset)
            forwardVector = duct.orientation * ogre.Vector3.UNIT_X
            offset = relativePos.crossProduct(forwardVector).length()
            
            if (offset < 2) and (math.fabs(angleDeg) < 3):
                event.aligned = True
            else:
                event.aligned = False
            event.visible = True
            
            self.publish(ext.vision.EventType.DUCT_FOUND, event)
            
#            # Check for centering
#            toCenter = ogre.Vector2(x, y)
#            if toCenter.normalise() < 0.08:
#                if not self._ductCentered:
#                    self._ductCentered = True
#                    self.publish(ext.vision.EventType.duct_CENTERED, event)
#            else:
#                self._ductCentered = False
            
        else:
            if self._foundDuct:
                self.publish(ext.vision.EventType.DUCT_LOST, ext.core.Event())

        self._foundDuct = ductVisible
        
    def _checkDownwardSafe(self):
        # Drop out if we have no Safes
        if self._safes is None:
            return
        if len(self._safes) == 0:
            return
        
        safe, relativePos = self._findClosest(self._safes)
        safeVisible, x, y, angle = self._downwardCheck(relativePos, safe)

        if safeVisible and (relativePos.length() < 7):
            event = ext.core.Event()
            event.x = x
            event.y = y
            #event.angle = angle
            self.publish(ext.vision.EventType.SAFE_FOUND, event)
            
            # Check for centering
#            toCenter = ogre.Vector2(x, y)
#            if toCenter.normalise() < 0.08:
#                if not self._safeCentered:
#                    self._safeCentered = True
#                    self.publish(ext.vision.EventType.PIPE_CENTERED, event)
#            else:
#                self._safeCentered = False
            
        else:
            if self._foundDownSafe:
                self.publish(ext.vision.EventType.SAFE_LOST, ext.core.Event())

        self._foundDownSafe = safeVisible
        

ext.core.SubsystemMaker.registerSubsystem('IdealSimVision', IdealSimVision)

class RenderCameraListener(ogre.RenderTargetListener):
    """
    Listens for the rendering of our camera textures, copies them to system
    memory, then ejects them into the vision system.
    """
    
    def __init__(self, vehicle, camera, buffer_, texture, updateInterval):
        """
        @type  camera: ext.vision.Camera
        @param camera: The camera to send the render image to
        
        @type  buffer: ctypes array
        @param buffer: The buffer to copy the renderered image to
        
        @type  texture: ogre.renderer.OGRE.Texture
        @param texture: The texture that it rendered to by Ogre
        """
        
        ogre.RenderTargetListener.__init__(self)
        self._trailMarker = vehicle.getDevice('TrailMarker')
        self._camera = camera
        self._bufferAddress = ctypes.addressof(buffer_)
        self._texture = texture
        self._updateInterval = updateInterval
        
        self._image = None
        self._lastTime = 0

    def preRenderTargetUpdate(self, renderTargetEvent):
        self._trailMarker.setMarkerVisibility(False)

    def postRenderTargetUpdate(self, renderTargetEvent):
        self._trailMarker.setMarkerVisibility(True)
        
        now = ram.timer.time()
        if 0 == self._lastTime:
            timePassed = self._updateInterval
        else:
            timePassed = now - self._lastTime

        # Only update at the desired interval
        if timePassed >= self._updateInterval:
            self._updateCamera(timePassed)

            self._lastTime = now

    def _updateCamera(self, timeSinceLastUpdate):
        # Lock all of the texture buffer
        textureBuffer = self._texture.getBuffer()
        lockBox = ogre.Box(0, 0, 640, 480)
        textureBuffer.lock(lockBox, ogre.HardwareBuffer.HBL_NORMAL)

        # Copy the lock portion our image
        texPb = textureBuffer.getCurrentLock()

        # Convert and copy our pixels
        ogre.PixelUtil.bulkPixelConversion(ogre.CastInt(texPb.getData()),
                                           textureBuffer.getFormat(),
                                           self._bufferAddress,
                                           ogre.PixelFormat.PF_R8G8B8,
                                           640 * 480)

        # Create our temporary image
        self._image = ext.vision.Image.loadFromBuffer(self._bufferAddress,
                                                      640, 480, False)
        # Send image to camera
        self._camera.capturedImage(self._image)
        
        textureBuffer.unlock()

        

class SimVision(ext.vision.VisionSystem):
    def __init__(self, config, deps):
        vehicle = ext.core.Subsystem.getSubsystemOfType(IVehicle, deps, 
                                                        nonNone = True)
        self.robot = vehicle.getDevice('SimulationDevice').robot

        # Creates ogre.renderer.OGRE.Camera and attaches it to the vehicle
        forwardOgreCamera = Simulation._createCamera(
            self.robot, '_forward', (0.5, 0, 0), (1, 0, 0))
        downwardOgreCamera = Simulation._createCamera( 
            self.robot, '_downward', (0.5, 0, -0.1), (0, 0, -1))
        
        # Create _forwardCamera, _forwardBuffer, and _forwardTexture
        self._setupCameraRendering(forwardOgreCamera, 640, 480)
        self._setupCameraRendering(downwardOgreCamera, 640, 480) 

        # Transform arguments to create base VisionSystem class
        cfg = ext.core.ConfigNode.fromString(str(config))
        depList = ext.core.SubsystemList()
        for subsys in deps:
            depList.append(subsys)
        
        ext.vision.VisionSystem.__init__(self, self._forwardCamera,
                                         self._downwardCamera,
                                         cfg, depList)


        cameraRate = config.get('cameraRate', 10)
        self._cameraUpdateInterval = 1.0 / cameraRate

        # Setup render target listeners to do the copying of images
        self._forwardCameraListener = RenderCameraListener(vehicle,
            self._forwardCamera, self._forwardBuffer, self._forwardTexture,
            self._cameraUpdateInterval)
        self._forwardTexture.getBuffer().getRenderTarget().addListener(
            self._forwardCameraListener)
        
        self._downwardCameraListener = RenderCameraListener(vehicle,
            self._downwardCamera, self._downwardBuffer, self._downwardTexture,
            self._cameraUpdateInterval)
        self._downwardTexture.getBuffer().getRenderTarget().addListener(
            self._downwardCameraListener)
        
    def _setupCameraRendering(self, camera, width, height):
        """
        @type  camera: ogre.renderer.OGRE.Camera
        @param camera: The Ogre camera to grab images from
        """ 

        ImageBufferType = ctypes.c_uint8 * (640 * 480 * 3)

        # Create our camera to allow communication with the vision system
        setattr(self, camera.name + 'Camera', ext.vision.Camera(width, height))
        # Buffer we copy the rendered image into
        setattr(self, camera.name + 'Buffer', ImageBufferType())

        # Create our texture which the camera renderers to
        renderSys = ogre.Root.getSingleton().getRenderSystem()
        texture = ogre.TextureManager.getSingleton().createManual(
            camera.name + 'RenderTexture', 
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
            ogre.TEX_TYPE_2D, width, height, 0, ogre.PixelFormat.PF_B8G8R8,
            ogre.TU_RENDERTARGET)
        rttTex = texture.getBuffer().getRenderTarget();
        viewport = rttTex.addViewport(camera)
        viewport.setBackgroundColour(ogre.ColourValue(200,200,200))

        setattr(self, camera.name + 'Texture', texture)

    def saveForwardCameraSnapshot(self, filename):
        """
        Saves the current forward camera image to file
        """
        address = ctypes.addressof(self._forwardBuffer)
        image = ext.vision.Image.loadFromBuffer(address, 640, 480, False)
        ext.vision.Image.saveToFile(image, filename)


ext.core.SubsystemMaker.registerSubsystem('SimVision', SimVision)
