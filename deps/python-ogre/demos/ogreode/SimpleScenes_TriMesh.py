# # /*
# # SimpleScenes_TriMesh.h
# # ---------------------
# # A reimplementation of the ODE triangle mesh collision
# # demo using Ogre and the OgreOde wrapper.
# # */
import Ogre as ogre
import OgreOde

from SimpleScenes_BoxStack import *

# # /*
# # The box stacking test extends the box stacking demo, but adds a trimesh!
# # */
class SimpleScenes_TriMesh ( SimpleScenes_BoxStack ):
    def __init__ ( self, world ):
        SimpleScenes_BoxStack.__init__(self,world)
        ## Create the visual entity and scene node
        entity = self._mgr.createEntity("Landscape","landscape.mesh")

        node = self._mgr.getRootSceneNode().createChildSceneNode(entity.getName())
        node.attachObject(entity)
        node.setScale(ogre.Vector3(0.4,0.3,0.4))
        node.setPosition(ogre.Vector3(0,3,0))
        node.setOrientation(ogre.Quaternion(ogre.Degree(30),ogre.Vector3.UNIT_Y))

        entity.setNormaliseNormals(True)
        ##entity.setCastShadows(False)
        
        ## Turn it into a triangle mesh static geometry
        ei = OgreOde.EntityInformer (entity,node._getFullTransform())
        geom = ei.createStaticTriangleMesh(self._world, self._space)
        entity.setUserObject (geom)
        self._geoms.append(geom)

    ## Destructor, manually destroy the entity and node, since they're 
    ## not associated with a body they won't get deleted automatically
    def __del__ ( self ):
        self._mgr.destroySceneNode("Landscape")
        self._mgr.destroyEntity("Landscape")
        SimpleScenes.__del__(self)
       
    ## Return our name for the test application to display
    def getName(self):
        return "Test Triangle Mesh"
