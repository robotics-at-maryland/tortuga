import Ogre, OgreNewt

# Used for autonaming of entities
box_entity_count = 0
plane_entity_count = 0

def create_box(scene_mgr, world, callback, size = Ogre.Vector3(1.0, 1.0, 1.0), 
               name = 'GeneratedBox' + str(box_entity_count), 
               pos = Ogre.Vector3(0.0, 0.0, 0.0), 
               orient = Ogre.Quaternion.IDENTITY):
    """
    Convience function, creates a box of the give size where the user wants
    it
    """
    
    ## base mass on the size of the object.
    mass = size.x * size.y * size.z * 0.5
        
    ## calculate the inertia based on box formula and mass
    inertia = OgreNewt.CalcBoxSolid(mass, size)
                
    box = scene_mgr.createEntity(name, "box.mesh" )
    box_node = scene_mgr.getRootSceneNode().createChildSceneNode()
    box_node.attachObject(box)
    box_node.setScale(size)
    box.setNormaliseNormals(True)
    
    col = OgreNewt.Box(world, size)
    bod = OgreNewt.Body(world, col)
    #del col
                
    bod.attachToNode(box_node)
    
    bod.setMassMatrix(mass, inertia)
    bod.setCustomForceAndTorqueCallback( callback , "")
    
    box.setMaterialName("Simple/BumpyMetal")

    bod.setPositionOrientation(pos, orient)

    global box_entity_count 
    box_entity_count += 1
 
    return bod

def create_plane(scene_mgr, world, width, height,
                 name = 'GeneratedValue',
                 pos = Ogre.Vector3(0.0, 0.0, 0.0), 
                 norm = Ogre.Vector3.UNIT_Z):
    if name == 'GeneratedValue':
        name =  'GeneratedPlane' + str(plane_entity_count)            
                 
    # Visual Part
    meshname = "GeneratedPlaneMesh" + str(plane_entity_count)
    plane = Ogre.Plane(norm , 0 );
    Ogre.MeshManager.getSingletonPtr().createPlane(meshname,
           Ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
           width,height);
    
    theplane = scene_mgr.createEntity(name, meshname)
    theplane.setMaterialName("Simple/BumpyMetal")
    
    plane_node = scene_mgr.getRootSceneNode().createChildSceneNode()
    plane_node.attachObject(theplane)
    plane_node.setPosition(pos)
    #plane_node.setOrientation(orient)
    #plane_node.setScale(0.4)

    col = OgreNewt.TreeCollision(world, plane_node, True)
    plane = OgreNewt.Body(world, col)
    #del col
    
    plane.attachToNode(plane_node)
    plane.setPositionOrientation(pos, plane_node.orientation)

    global plane_entity_count 
    plane_entity_count += 1

    return plane
        