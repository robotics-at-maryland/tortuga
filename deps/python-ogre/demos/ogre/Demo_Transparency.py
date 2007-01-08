# This code is in the Public Domain.
import Ogre as ogre
import SampleFramework as sf

class TransparencyApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        light = sceneManager.createLight('MainLight')
        light.setPosition ( 20, 80, 50 )
        
        theplane = self.sceneManager.createEntity("ThePlane", ogre.SceneManager.PT_PLANE )
        theplane.setMaterialName( "Simple/Translucent" )
        
        planeNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
        planeNode.attachObject( theplane )
        planeNode.setPosition( ogre.Vector3(0,0,0) )
        planeNode.setOrientation( ogre.Quaternion( ogre.Degree(d=-90), ogre.Vector3(1,0,0) ) )

#         planeEntity = self.sceneManager.createEntity('Plane', ogre.SceneManager.PT_PLANE)
#         planeEntity.setMaterialName('Examples/BumpyMetal')

        knotEntity = self.sceneManager.createEntity('Knot', 'knot.mesh')
        knotEntity.setMaterialName('Examples/TransparentTest')

        rootNode = self.sceneManager.getRootSceneNode()
        #rootNode.attachObject(planeEntity)
#         rootNode.attachObject(theplane)
        rootNode.attachObject(knotEntity)

        for index in xrange(0,10):
            node = self.sceneManager.createSceneNode()
            nodePos = ogre.Vector3()
            nodePos.x = ogre.Math.SymmetricRandom() * 500.0
            nodePos.y = ogre.Math.SymmetricRandom() * 500.0
            nodePos.z = ogre.Math.SymmetricRandom() * 500.0
            node.setPosition (nodePos)
            rootNode.addChild(node)
            knotEntityClone = knotEntity.clone('Knot%d' % index )
            node.attachObject(knotEntityClone)

if __name__ == '__main__':
    try:
        application = TransparencyApplication()
        application.go()
    except ogre.Exception, e:
        print e    
