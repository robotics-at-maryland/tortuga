# This code is in the Public Domain
import  Ogre as ogre
import SampleFramework as sf

## As this demo does it's own key input we need to cope with the change to OIS
if ogre.OgreVersionString[:2]=="12":
    KC_H = ogre.KC_H
    KC_G = ogre.KC_G
    KC_U = ogre.KC_U
    KC_Y = ogre.KC_Y
else:
    import OIS as OIS
    KC_H = OIS.KC_H
    KC_G = OIS.KC_G
    KC_U = OIS.KC_U
    KC_Y = OIS.KC_Y

class SkyDomeApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyDome(True, 'Examples/CloudySky', 5, 8)
 
        light = sceneManager.createLight('MainLight')
        light.setPosition (ogre.Vector3(20, 80, 50))

        plane = ogre.Plane()
        plane.normal = ogre.Vector3.UNIT_Y
        plane.d = 200

        mm = ogre.MeshManager.getSingleton()
        mm.createPlane('FloorPlane', 'General', plane, 2000.0, 2000.0,
                       2, 2, True, 1, 1.0, 1.0, ogre.Vector3(0, 0, 1),
                       ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
                       True,True )

        entity = sceneManager.createEntity('floor', 'FloorPlane')
        entity.setMaterialName('Examples/RustySteel')
        sceneManager.getRootSceneNode().attachObject(entity)

        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)

    def _createFrameListener(self):
        self.frameListener = SkyDomeListener(self.renderWindow, self.camera, self.sceneManager)
        self.root.addFrameListener(self.frameListener)

class SkyDomeListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, sceneManager):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.sceneManager = sceneManager
        self.curvature = 1
        self.tiling = 15

    def frameStarted(self, frameEvent):
        updateSky = False
        if self._isToggleKeyDown(KC_H, 0.1):
            self.curvature += 1
            updateSky = True
        if self._isToggleKeyDown(KC_G, 0.1):
            self.curvature -= 1
            updateSky = True
        if self._isToggleKeyDown(KC_U, 0.1):
            self.tiling += 1
            updateSky = True
        if self._isToggleKeyDown(KC_Y, 0.1):
            self.tiling -= 1
            updateSky = True
        if updateSky:
            self.sceneManager.setSkyDome(True, 'Examples/CloudySky', self.curvature, self.tiling)
            
        return sf.FrameListener.frameStarted(self, frameEvent)



if __name__ == '__main__':
    try:
        application = SkyDomeApplication()
        application.go()
    except ogre.Exception, e:
        print e
