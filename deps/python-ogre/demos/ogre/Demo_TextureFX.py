import  Ogre as ogre
import SampleFramework as sf

class TextureFXApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        camera = self.camera
        
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        light = sceneManager.createLight('MainLight')
        light.setPosition (20, 80, 150)

        self._createScalingPlane()
        self._createScrollingKnot()
        self._createWateryPlane()
        skyMaterial = ogre.MaterialManager.getSingleton()   #get the material manager pointer
        skyMaterial =  skyMaterial.create('SkyMat', ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME )
        skyMaterial.lightingEnabled = False
        
        textureUnitState = skyMaterial.getTechnique(0).getPass(0).createTextureUnitState('clouds.jpg', 0)
        textureUnitState.setScrollAnimation(0.15, 0)

        sceneManager.setSkyDome(True, 'SkyMat', -5.0, 2.0)

    def _createScalingPlane(self):
        entity = self.sceneManager.createEntity('Plane', ogre.SceneManager.PT_PLANE)
        entity.setMaterialName('Examples/TextureEffect1')
        node = self.sceneManager.getRootSceneNode().createChildSceneNode(ogre.Vector3(-250, -40, -100))
        node.attachObject(entity)

    def _createScrollingKnot(self):
        entity = self.sceneManager.createEntity('Knot', 'knot.mesh')
        entity.setMaterialName('Examples/TextureEffect2')
        node = self.sceneManager.getRootSceneNode().createChildSceneNode(ogre.Vector3(200, 50, 150))
        node.attachObject(entity)

    def _createWateryPlane(self):
        entity = self.sceneManager.createEntity('WaterPlane', ogre.SceneManager.PT_PLANE)
        entity.setMaterialName('Examples/TextureEffect3')
        node = self.sceneManager.getRootSceneNode().attachObject(entity)

if __name__ == '__main__':
    try:
        application = TextureFXApplication()
        application.go()
    except ogre.Exception, e:
        print e
