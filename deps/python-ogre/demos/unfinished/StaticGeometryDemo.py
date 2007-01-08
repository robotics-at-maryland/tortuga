# this code is in the public domain
import Ogre as ogre
import SampleFramework as sf

GRASS_WIDTH = 250
GRASS_HEIGHT = 300

class GrassApplication(sf.Application):
    def __init__(self):
        sf.Application.__init__(self)
        self.minLightColour = ogre.ColourValue(0.5, 0.1, 0.0)
        self.maxLightColour = ogre.ColourValue(1.0, 0.6, 0.0)
        self.minFlareSize = 40
        self.maxFlareSize = 80
    
    def _createScene(self):
        sceneManager = self.sceneManager

        sceneManager.setSkyBox(True, "Examples/SpaceSkyBox")

        self.setupLighting()
        self.createFloor()
        self.createGrassMesh()

        grassEntity = sceneManager.createEntity("1", "grassblades")
        staticGrass = sceneManager.createStaticGeometry("grassBladeGeometry")
        
        staticGrass.regionDimensions = (1000,1000,1000)
        # Set the region origin so the centre is at 0 world
        staticGrass.origin = (-500, -500, -500)
        for x in xrange(-1950, 1950, 150):
            for z in xrange(-1950, 1950, 150):
                pos = (x + ogre.Math.RangeRandom(-25,25),
                       0,
                       z + ogre.Math.RangeRandom(-25, 25))
                orientation = ogre.Quaternion()
                orientation.FromAngleAxis(ogre.Degree(ogre.Math.RangeRandom(0,359)),
                                           ogre.Vector3.UNIT_Y)
                scale = ogre.Vector3(1, ogre.Math.RangeRandom(0.85,1.15), 1)
                staticGrass.addEntity(grassEntity, pos, orientation, scale)
        staticGrass.build()
        self.staticGeom = staticGrass

        ogreMesh = ogre.MeshManager.getSingleton().load('ogrehead.mesh', "general")
                                                   #      ogre.ResourceManager.DEFAULT_RESOURCE_GROUP_NAME)
        tangentExist, src, dst = ogreMesh.suggestTangentVectorBuildParams()
        if not tangentExist:
            ogreMesh.buildTangentVectors(src, dst)
        ogreEntity = sceneManager.createEntity('head', 'ogrehead.mesh')
        ogreEntity.setMaterialName('Examples/OffsetMapping/Specular')
        headNode = sceneManager.getRootSceneNode().createChildSceneNode()
        headNode.attachObject(ogreEntity)
        headNode.scale = (7, 7, 7)
        headNode.position = (0, 200, 0)
        ogreEntity.normaliseNormals = (True)
        self.camera.move(ogre.Vector3(0, 350, 0))

    def _createFrameListener(self):
        self.frameListener = GrassListener(self.renderWindow, self.camera, self.sceneManager)
        self.root.addFrameListener(self.frameListener)

    def setupLighting(self):
        sceneManager = self.sceneManager
        # Set ambient light

        sceneManager.setAmbientLight (ogre.ColourValue(.2, .2, .2))
        redYellowLightsNode = sceneManager.getRootSceneNode().createChildSceneNode()
        redYellowLights = sceneManager.createBillboardSet('RedYellowLights', 20)
        redYellowLights.materialName = 'Examples/Flare'
        redYellowLightsNode.attachObject(redYellowLights)
        self.redYellowLightsNode = redYellowLightsNode

        greenBlueLightsNode = sceneManager.getRootSceneNode().createChildSceneNode()
        greenBlueLights = sceneManager.createBillboardSet('GreenBlueLights', 20)
        greenBlueLights.materialName = 'Examples/Flare'
        greenBlueLightsNode.attachObject(greenBlueLights)
        self.greenBlueLightsNode = greenBlueLightsNode

        # Red light billboard, in "off" state
        redLightPosition = ogre.Vector3(78, -8, -70)
        redLightBoard = redYellowLights.createBillboard(redLightPosition, ogre.ColourValue(1.0, 1.0, 1.0))
        redLightBoard.colour = ogre.ColourValue(0, 0, 0)
        
        # Blue light billboard, in "off" state        
        blueLightPosition = ogre.Vector3(-90, -8, -70)
        blueLightBoard = greenBlueLights.createBillboard(blueLightPosition, ogre.ColourValue(1, 1, 1))
        blueLightBoard.colour = ogre.ColourValue(0, 0, 0)
        
        # Yellow light billboard, in "off" state        
        yellowLightPosition = ogre.Vector3(-4.5, 30, -80)
        yellowLightBoard = redYellowLights.createBillboard(yellowLightPosition, ogre.ColourValue(1, 1, 1))
        yellowLightBoard.colour = ogre.ColourValue(0, 0, 0)
        
        # Green light billboard, in "off" state        
        greenLightPosition = ogre.Vector3(50, 70, 80)
        greenLightBoard = redYellowLights.createBillboard(greenLightPosition, ogre.ColourValue(1, 1, 1))
        greenLightBoard.colour = ogre.ColourValue(0, 0, 0)

        # Red light, in "off" state
        redLight = sceneManager.createLight('RedFlyingLight')
        redLight.type = ogre.Light.LT_POINT
        redLight.position = redLightPosition
        redLight.diffuseColour = ogre.ColourValue(0, 0, 0)
        redYellowLightsNode.attachObject(redLight)

        # Blue light, in "off" state
        blueLight = sceneManager.createLight('BlueFlyingLight')
        blueLight.type = ogre.Light.LT_POINT
        blueLight.position = blueLightPosition
        blueLight.diffuseColour = ogre.ColourValue(0, 0, 0)
        greenBlueLightsNode.attachObject(blueLight)

        # Yellow light, in "off" state
        yellowLight = sceneManager.createLight('YellowFlyingLight')
        yellowLight.type = ogre.Light.LT_POINT
        yellowLight.position = yellowLightPosition
        yellowLight.diffuseColour = ogre.ColourValue(0, 0, 0)
        redYellowLightsNode.attachObject(yellowLight)

        # Green light, in "off" state
        greenLight = sceneManager.createLight('GreenFlyingLight')
        greenLight.type = ogre.Light.LT_POINT
        greenLight.position = greenLightPosition
        greenLight.diffuseColour = ogre.ColourValue(0, 0, 0)
        greenBlueLightsNode.attachObject(greenLight)
        
        # Light flashers
        redLightFlasher = LightFlasher(redLight, redLightBoard, (1, 0, 0))
        greenLightFlasher = LightFlasher(greenLight, greenLightBoard, (0, 1, 0))
        blueLightFlasher = LightFlasher(blueLight, blueLightBoard, (0, 0, 1))
        yellowLightFlasher = LightFlasher(yellowLight, yellowLightBoard, (1.0,1.0,0.0))
        
        # Light controller functions
        redLightControllerFunc = LightFlasherControllerFunction(ogre.WFT_SINE, 0.5, 0.0)
        blueLightControllerFunc = LightFlasherControllerFunction(ogre.WFT_SINE, 0.75, 0.5)
        yellowLightControllerFunc = LightFlasherControllerFunction(ogre.WFT_SINE, 0.25, 0.0)
        greenLightControllerFunc = LightFlasherControllerFunction(ogre.WFT_SINE, 0.25, 0.5)

#         # Convert to shared pointers
#         redLightFlasher = ogre.SharedPtr(redLightFlasher)
#         greenLightFlasher = ogre.SharedPtr(greenLightFlasher)
#         blueLightFlasher = ogre.SharedPtr(blueLightFlasher)
#         yellowLightFlasher = ogre.SharedPtr(yellowLightFlasher)
#         
#         redLightControllerFunc = ogre.SharedPtr(redLightControllerFunc)
#         blueLightControllerFunc = ogre.SharedPtr(blueLightControllerFunc)
#         yellowLightControllerFunc = ogre.SharedPtr(yellowLightControllerFunc)
#         greenLightControllerFunc = ogre.SharedPtr(greenLightControllerFunc)

        # Light controllers
#         controllerManager = ogre.ControllerManager.getSingleton()
#         self.redLightController = controllerManager.createController(
#             controllerManager.getFrameTimeSource(),
#             redLightFlasher,
#             redLightControllerFunc)
#         self.blueLightController = controllerManager.createController(
#             controllerManager.getFrameTimeSource(),
#             blueLightFlasher,
#             blueLightControllerFunc)
#         self.yellowLightController = controllerManager.createController(
#             controllerManager.getFrameTimeSource(),
#             yellowLightFlasher,
#             yellowLightControllerFunc)
#         self.greenLightController = controllerManager.createController(
#             controllerManager.getFrameTimeSource(),
#             greenLightFlasher,
#             greenLightControllerFunc)
        
    def createFloor(self):
        pass

    def createGrassMesh(self):
        mesh = ogre.MeshManager.getSingleton().createManual(
            "grassblades",
            "general") #ogre.ResourceManager.DEFAULT_RESOURCE_GROUP_NAME)
        
        subMesh = mesh.createSubMesh()
        subMesh.useSharedVertices = False
        subMesh.vertexData =   (ogre.VertexData()) ##AJM PROBLEM>
        subMesh.vertexData.vertexStart = 0
        subMesh.vertexData.vertexCount = 12

        #work-around to prevent destruction
        self.vertexData = subMesh.vertexData

        vDec = subMesh.vertexData.vertexDeclaration
        vDec.addElement(0, vDec.getVertexSize(0), ogre.VET_FLOAT3, ogre.VES_POSITION, 0)
        vDec.addElement(0, vDec.getVertexSize(0), ogre.VET_FLOAT3, ogre.VES_NORMAL, 0)
        vDec.addElement(0, vDec.getVertexSize(0), ogre.VET_FLOAT2, ogre.VES_TEXTURE_COORDINATES, 0)
        
        vertexBuffer = ogre.HardwareBufferManager.getSingleton().createVertexBuffer(
            vDec.getVertexSize(0),
            12,
            ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY)

        vertexes = vertexBuffer.lock(subMesh.vertexData.vertexDeclaration,
                                      ogre.HardwareBuffer.HBL_DISCARD)
        baseVec = ogre.Vector3(GRASS_WIDTH/2.0, 0, 0)
        vec = ogre.Vector3(baseVec)
        rot = ogre.Quaternion()
        rot.FromAngleAxis(ogre.Degree(60), ogre.Vector3.UNIT_Y)
        vertexIndex = 0
        POSITION_ELEMENT = 0
        NORMAL_ELEMENT = 1
        UV_ELEMENT = 2
        for index in xrange(0,3):
            vertexes.setFloat(vertexIndex, POSITION_ELEMENT, -vec.x, GRASS_HEIGHT, -vec.z)
            vertexes.setFloat(vertexIndex, NORMAL_ELEMENT, 0, 1, 0)
            vertexes.setFloat(vertexIndex, UV_ELEMENT, 0, 0)
            vertexIndex += 1
            
            vertexes.setFloat(vertexIndex, POSITION_ELEMENT, vec.x, GRASS_HEIGHT, vec.z)
            vertexes.setFloat(vertexIndex, NORMAL_ELEMENT, 0, 1, 0)
            vertexes.setFloat(vertexIndex, UV_ELEMENT, 1, 0)
            vertexIndex += 1
            
            vertexes.setFloat(vertexIndex, POSITION_ELEMENT, -vec.x, 0, -vec.z)
            vertexes.setFloat(vertexIndex, NORMAL_ELEMENT, 0, 1, 0)
            vertexes.setFloat(vertexIndex, UV_ELEMENT, 0, 1)
            vertexIndex += 1
            
            vertexes.setFloat(vertexIndex, POSITION_ELEMENT, vec.x, 0, vec.z)
            vertexes.setFloat(vertexIndex, NORMAL_ELEMENT, 0, 1, 0)
            vertexes.setFloat(vertexIndex, UV_ELEMENT, 1, 1)
            vertexIndex += 1

            vec = rot * vec
        vertexBuffer.unlock()
        
        subMesh.vertexData.vertexBufferBinding.setBinding(0, vertexBuffer)
        subMesh.indexData.indexCount = 6*3
        subMesh.indexData.indexBuffer = ogre.HardwareBufferManager.getSingleton().createIndexBuffer(
            ogre.HardwareIndexBuffer.IT_16BIT, 6*3, ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY)
        indices = []
        for i in xrange(0,3):
            indices += [ x + i * 4 for x in [0,3,1, 0,2,3] ]
        subMesh.indexData.indexBuffer.writeIndexes(0, indices)
        subMesh.materialName = ("Examples/GrassBlades")
        
        #work-around to prevent destruction
        self.indexData = subMesh.indexData


# Light related classes
class LightFlasher(ogre.ControllerValue_less_float_grate_):
    def __init__(self, light, billboard, maxColour):
        ogre.ControllerValue_less_float_grate_.__init__(self)
        self.light = light
        self.billboard = billboard
        self.maxColour = ogre.ColourValue(maxColour[0],maxColour[1],maxColour[2])
        self.intensity = 0.0

    def getValue(self):
        return self.intensity

    def setValue(self, value):
        self.intensity = value

        newColour = self.maxColour * value
        newColour.a = self.maxColour.a

        self.light.diffuseColour = newColour
        self.billboard.colour = newColour
        
class LightFlasherControllerFunction(ogre.WaveformControllerFunction):
    def __init__(self, waveType, frequency, phase):
        ogre.WaveformControllerFunction.__init__(
            self, waveType, 0, frequency, phase, 1, True)

class LightWibbler(ogre.ControllerValue_less_float_grate_):
    def __init__(self, light, billboard, minColour, maxColour, minSize, maxSize):
        ogre.ControllerValue_less_float_grate_.__init__(self)
        self.light = light
        self.billboard = billboard
        self.colourRange = (maxColour - minColour) * 0.5
        self.halfColour = minColour + self.colourRange
        self.minSize = minSize
        self.sizeRange = maxSize - minSize

    def getValue(self):
        return self.intensity

    def setValue(self, value):
        self.intensity = value
        newColour = self.halfColour + self.colourRange * intensity
        self.light.diffuseColour = (newColour)
        self.billboard.colour = (newColour)
        newSize = self.minSize + intensity * self.sizeRange

class GrassListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, animationState):
        sf.FrameListener.__init__(self, renderWindow, camera)

    def frameStarted(self, frameEvent):
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    application = GrassApplication()
    application.go()
    
