
# /*
# -----------------------------------------------------------------------------
# Filename:    PlayPen.cpp
# Description: Somewhere to play in the sand...
# -----------------------------------------------------------------------------
# */


#define NUM_TEST_NODES 5
SceneNode* mTestNode[NUM_TEST_NODES] = {0,0,0,0,0} 
SceneNode* mLightNode = 0 
SceneNode* self.rootNode 
SceneNode* camNode 
Entity* mEntity 
animTime = 0 
Animation* mAnim = 0 
std.vector<AnimationState*> mAnimStateList 
AnimationState* mAnimState = 0 
Overlay* mpOverlay 
Entity* pPlaneEnt 
Camera* testCam = 0 
SceneNode* camPlaneNode[6] 
Light* mLight 
IntersectionSceneQuery* intersectionQuery = 0 
RaySceneQuery* rayQuery = 0 
Entity* ball = 0 
Vector3 ballVector 
bool testreload = False 
String testBackgroundLoadGroup 

## Hacky globals
GpuProgramParametersSharedPtr fragParams 
GpuProgramParametersSharedPtr vertParams 
MaterialPtr skin 
Frustum* frustum = 0 
Camera* theCam 
Camera* reflectCam = 0 
Camera* camera2 = 0 
Bone* manuallyControlledBone = 0 
##AnimationBlender* animBlender = 0 
String animBlendTarget[2] 
animBlendTargetIndex 
MovablePlane movablePlane("APlane") 

using namespace OIS 

class RefractionTextureListener : public RenderTargetListener

public:
    (self): preRenderTargetUpdate( RenderTargetEvent& evt)

        pPlaneEnt.setVisible(False) 


    (self): postRenderTargetUpdate( RenderTargetEvent& evt)

        pPlaneEnt.setVisible(True) 



class ReflectionTextureListener : public RenderTargetListener

public:
    (self): preRenderTargetUpdate( RenderTargetEvent& evt)

        static Plane reflectPlane(Vector3.UNIT_Y, -100) 
        pPlaneEnt.setVisible(False) 
        theCam.enableReflection(reflectPlane) 


    (self): postRenderTargetUpdate( RenderTargetEvent& evt)

        pPlaneEnt.setVisible(True) 
        theCam.disableReflection() 





class PlayPenListener : public ExampleFrameListener, public ResourceBackgroundQueue.Listener

protected:
	SceneManager* self.sceneManager 
public:
    PlayPenListener(SceneManager* mgr, RenderWindow* win, Camera* cam)
        : ExampleFrameListener(win, cam),self.sceneManager(mgr)




	##/ Background load completed
def operationCompleted(BackgroundProcessTicket ticket)

		mDebugText = "Background load complete" 



    bool frameStarted( FrameEvent& evt)

        if ( not vertParams.isNull()):

            Matrix4 scaleMat = Matrix4.IDENTITY 
            scaleMat[0][0] = 0.5 
            scaleMat[1][1] = -0.5 
            scaleMat[2][2] = 0.5 
            scaleMat[0][3] = 0.5 
            scaleMat[1][3] = 0.5 
            scaleMat[2][3] = 0.5 
            Matrix4 mat = frustum.getProjectionMatrixWithRSDepth() * 
                frustum.getViewMatrix() 
            
            
            mat = scaleMat * mat 

            vertParams.setNamedConstant("texViewProjMatrix", mat) 





		if (manuallyControlledBone):

			manuallyControlledBone.yaw(Degree(evt.timeSinceLastFrame*100))  



		static float reloadtime = 10.0 
		if (testreload):

			reloadtime -= evt.timeSinceLastFrame 
			if (reloadtime <= 0):

				Entity* e = self.sceneManager.getEntity("1") 
				e.getParentSceneNode().detachObject("1") 
				e = self.sceneManager.getEntity("2") 
				e.getParentSceneNode().detachObject("2") 
				self.sceneManager.destroyAllEntities() 
				ResourceGroupManager.getSingleton().unloadResourceGroup("Sinbad") 
				ResourceGroupManager.getSingleton().loadResourceGroup("Sinbad") 

				testreload = False 




		static float backgroundLoadTime = 5.0 
		if (!testBackgroundLoadGroup.empty()):

			backgroundLoadTime -= evt.timeSinceLastFrame 
			if (backgroundLoadTime < 0):

				ResourceBackgroundQueue.getSingleton().loadResourceGroup(testBackgroundLoadGroup, self) 
				testBackgroundLoadGroup.clear() 
				mDebugText = "Background load queued" 







        bool ret = ExampleFrameListener.frameStarted(evt) 

		if (reflectCam):

			reflectCam.setOrientation(mCamera.getOrientation()) 
			reflectCam.setPosition(mCamera.getPosition()) 

		if (camera2):

			camera2.setOrientation(mCamera.getOrientation()) 
			camera2.setPosition(mCamera.getPosition()) 

		return ret 



    
    bool frameEnded( FrameEvent& evt)




        ## local just to stop toggles flipping too fast
        static timeUntilNextToggle = 0 
        static bool animate = True 
        static bool rotate = False 

        static bool firstTime = True 

        if (!firstTime):

            ##mCamera.yaw(20) 

        firstTime = False 

        if (timeUntilNextToggle >= 0) :
            timeUntilNextToggle -= evt.timeSinceLastFrame 

		static bool mWireframe = False 
		if (self.keyboard.isKeyDown(OIS.KC_G) && timeUntilNextToggle <= 0):
OIS.KC_
			mWireframe = !mWireframe 
			if (mWireframe):

				mCamera.setPolygonMode(PM_WIREFRAME) 

			else:

				mCamera.setPolygonMode(PM_SOLID) 

			timeUntilNextToggle = 0.5 



        MaterialPtr mat = MaterialManager.getSingleton().getByName("Core/StatsBlockBorder/Up") 
        mat.setDepthCheckEnabled(True) 
        mat.setDepthWriteEnabled(True) 

        for (i = 0  i < NUM_TEST_NODES; ++i)

            if (mTestNode[i] && rotate):
            mTestNode[i].yaw(Degree(evt.timeSinceLastFrame * 15)) 

        
        if (mAnimState && animate):
            mAnimState.addTime(evt.timeSinceLastFrame) 
/*
		if (animBlender && animate):

			if (animBlender.getProgress() <= 0.0):

				animBlender.init(animBlendTarget[animBlendTargetIndex]) 
				animBlendTargetIndex = (animBlendTargetIndex + 1) % 2 
				animBlender.blend(animBlendTarget[animBlendTargetIndex], AnimationBlender.BlendWhileAnimating, 10, False) 

			else:

				animBlender.addTime(evt.timeSinceLastFrame) 


*/
		std.vector<AnimationState*>::iterator animi 
		for (animi = mAnimStateList.begin()  animi != mAnimStateList.end(); ++animi)

			(*animi).addTime(evt.timeSinceLastFrame) 


        if (self.keyboard.isKeyDown(OIS.KC_R) && timeUntilNextToggle <= 0):
OIS.OIS.KC_
            rotate = !rotate 
            timeUntilNextToggle = 0.5 

        if (self.keyboard.isKeyDown(OIS.KC_1) && timeUntilNextToggle <= 0):
OIS.OIS.KC_
            animate = !animate 
            timeUntilNextToggle = 0.5 



        if (rayQuery):

		    static std.set<Entity*> lastEnts 
		    rayQuery.setRay(mCamera.getCameraToViewportRay(0.5, 0.5)) 

		    ## Reset last set
		    for (std.set<Entity*>::iterator lasti = lastEnts.begin() 
				    lasti != lastEnts.end()  ++lasti)

			    (*lasti).setMaterialName("Examples/OgreLogo") 

		    lastEnts.clear() 
    		
    			
		    RaySceneQueryResult& results = rayQuery.execute() 
		    for (RaySceneQueryResult.iterator mov = results.begin() 
				    mov != results.end()  ++mov)

                if (mov.movable):

			        if (mov.movable.getMovableType() == "Entity"):

				        Entity* ent = static_cast<Entity*>(mov.movable) 
				        lastEnts.insert(ent) 
				        ent.setMaterialName("Examples/TextureEffect2") 
        						





        if (intersectionQuery):

            static std.set<Entity*> lastEnts 

            ## Reset last set
            for (std.set<Entity*>::iterator lasti = lastEnts.begin() 
                lasti != lastEnts.end()  ++lasti)

                (*lasti).setMaterialName("Examples/OgreLogo") 

            lastEnts.clear() 


            IntersectionSceneQueryResult& results = intersectionQuery.execute() 
            for (SceneQueryMovableIntersectionList.iterator mov = results.movables2movables.begin() 
                mov != results.movables2movables.end()  ++mov)

                SceneQueryMovableObjectPair& thepair = *mov 
                if (thepair.first.getMovableType() == "Entity"):

                    Entity* ent = static_cast<Entity*>(thepair.first) 
                    lastEnts.insert(ent) 
                    ent.setMaterialName("Examples/TextureEffect2") 


                if (thepair.second.getMovableType() == "Entity"):

                    Entity* ent = static_cast<Entity*>(thepair.second) 
                    lastEnts.insert(ent) 
                    ent.setMaterialName("Examples/TextureEffect2") 





        /*
		if (self.keyboard.isKeyDown(OIS.KC_V) && timeUntilNextToggle <= 0):
OIS.OIS.KC_
            static bool isVP = False 
            if (!isVP):

                skin.getTechnique(0).getPass(0).setVertexProgram("SimpleVP") 
                skin.getTechnique(0).getPass(0).setVertexProgramParameters(vertParams) 
                isVP = True 

            else:

                skin.getTechnique(0).getPass(0).setVertexProgram("") 
                isVP = False 

			timeUntilNextToggle = 0.5 

        */

		if (self.keyboard.isKeyDown(OIS.KC_P)):
OIS.OIS.KC_
            mTestNode[0].yaw(Degree(-evt.timeSinceLastFrame * 30)) 

		if (self.keyboard.isKeyDown(OIS.KC_O)):
OIS.OIS.KC_
            mTestNode[0].yaw(Degree(evt.timeSinceLastFrame * 30)) 

		if (self.keyboard.isKeyDown(OIS.KC_K)):
OIS.OIS.KC_
            mTestNode[0].roll(Degree(-evt.timeSinceLastFrame * 30)) 

		if (self.keyboard.isKeyDown(OIS.KC_L)):
OIS.OIS.KC_
            mTestNode[0].roll(Degree(evt.timeSinceLastFrame * 30)) 

		if (self.keyboard.isKeyDown(OIS.KC_U)):
OIS.OIS.KC_
            mTestNode[0].translate(0,0,-evt.timeSinceLastFrame * 30) 

		if (self.keyboard.isKeyDown(OIS.KC_J)):
OIS.OIS.KC_
            mTestNode[0].translate(0,0,evt.timeSinceLastFrame * 30) 

		if (self.keyboard.isKeyDown(OIS.KC_M)):
OIS.OIS.KC_
            mTestNode[0].translate(0,evt.timeSinceLastFrame * 30, 0) 

		if (self.keyboard.isKeyDown(OIS.KC_N)):
OIS.OIS.KC_
            mTestNode[0].translate(0,-evt.timeSinceLastFrame * 30, 0) 


        if (self.keyboard.isKeyDown(OIS.KC_0) && timeUntilNextToggle <= 0):
OIS.OIS.KC_
            mAnimState.setEnabled(!mAnimState.getEnabled()) 
            timeUntilNextToggle = 0.5 


		if (mEntity && self.keyboard.isKeyDown(OIS.KC_SPACE) && timeUntilNextToggle <= 0):
OIS.OIS.KC_
			self.sceneManager.destroyEntity(mEntity) 
			##mLight.setCastShadows(True) 
			mEntity = self.sceneManager.createEntity("newEnt", "robot.mesh") 
			self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(mEntity) 
			mAnimStateList.clear() 
			AnimationState* anim = mEntity.getAnimationState("Walk") 
			anim.setEnabled(True) 
			mAnimStateList.push_back(anim) 


        
        /** Hack to test frustum vols
        if (testCam):

            ## reposition the camera planes
            PlaneBoundedVolumeList volList = mLight._getFrustumClipVolumes(testCam) 

            PlaneBoundedVolume& vol = volList[1] 
            for (p = 0  p < 6; ++p)

                Plane& pl = vol.planes[p] 
                camPlaneNode[p].setOrientation(Vector3.UNIT_Z.getRotationTo(pl.normal)) 
                camPlaneNode[p].setPosition(0,0,0) 
                camPlaneNode[p].translate(0,0, pl.d, Node.TS_LOCAL) 


            vol.intersects(mEntity.getWorldBoundingBox()) 

        */

        ## Prcamera details
        ##mWindow.setDebugText("P: " + StringConverter.toString(mCamera.getDerivedPosition()) + " " + 
        ##    "O: " + StringConverter.toString(mCamera.getDerivedOrientation())) 
        return ExampleFrameListener.frameStarted(evt) && ExampleFrameListener::frameEnded(evt)         






class PlayPenApplication : public ExampleApplication

protected:
    RefractionTextureListener mRefractionListener 
    ReflectionTextureListener mReflectionListener 
public:
    PlayPenApplication() {
    
    


    ~PlayPenApplication() 

        if (frustum):
            delete frustum 

protected:
    
    (self): chooseSceneManager((self):)

		## DefaultSceneManager
		##self.sceneManager = self.root.createSceneManager("DefaultSceneManager", "PlayPenSMInstance") 

		## BspSceneManager
		##self.sceneManager = self.root.createSceneManager("BspSceneManager", "PlayPenSMInstance") 

		## OctreeSceneManager
		##self.sceneManager = self.root.createSceneManager("OctreeSceneManager", "PlayPenSMInstance") 

		## TerrainSceneManager
		self.sceneManager = self.root.createSceneManager("TerrainSceneManager", "PlayPenSMInstance") 



    (self): createTestBugPlaneMesh3Streams( String& testMeshName)

        Plane plane(Vector3.UNIT_Z, 0) 
        width = 258 
        height = 171 
        xsegments = 50 
        ysegments = 50 
        xTile = 1.0 
        yTile = 1.0 
         Vector3& upVector = Vector3.UNIT_Y 
        MeshPtr pMesh = MeshManager.getSingleton().createManual(testMeshName, 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        SubMesh *pSub = pMesh.createSubMesh() 

        ## Set up vertex data
        ## Use a single shared buffer
        pMesh.sharedVertexData = new VertexData() 
        VertexData* vertexData = pMesh.sharedVertexData 
        ## Set up Vertex Declaration
        VertexDeclaration* vertexDecl = vertexData.vertexDeclaration 
        ## We always need positions
        vertexDecl.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        vertexDecl.addElement(1, 0, VET_FLOAT3, VES_NORMAL) 
        vertexDecl.addElement(2, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0) 

        vertexData.vertexCount = (xsegments + 1) * (ysegments + 1) 

        ## Allocate vertex buffers
        HardwareVertexBufferSharedPtr vbufpos = 
            HardwareBufferManager.getSingleton().
            createVertexBuffer(vertexDecl.getVertexSize(0), vertexData.vertexCount,
            HardwareBuffer.HBU_STATIC_WRITE_ONLY, False) 
        HardwareVertexBufferSharedPtr vbufnorm = 
            HardwareBufferManager.getSingleton().
            createVertexBuffer(vertexDecl.getVertexSize(1), vertexData.vertexCount,
            HardwareBuffer.HBU_STATIC_WRITE_ONLY, False) 
        HardwareVertexBufferSharedPtr vbuftex = 
            HardwareBufferManager.getSingleton().
            createVertexBuffer(vertexDecl.getVertexSize(2), vertexData.vertexCount,
            HardwareBuffer.HBU_STATIC_WRITE_ONLY, False) 

        ## Set up the binding (one source only)
        VertexBufferBinding* binding = vertexData.vertexBufferBinding 
        binding.setBinding(0, vbufpos) 
        binding.setBinding(1, vbufnorm) 
        binding.setBinding(2, vbuftex) 

        ## Work out the transform required
        ## Default orientation of plane is normal along +z, distance 0
        Matrix4 xlate, xform, rot 
        Matrix3 rot3 
        xlate = rot = Matrix4.IDENTITY 
        ## Determine axes
        Vector3 zAxis, yAxis, xAxis 
        zAxis = plane.normal 
        zAxis.normalise() 
        yAxis = upVector 
        yAxis.normalise() 
        xAxis = yAxis.crossProduct(zAxis) 

        rot3.FromAxes(xAxis, yAxis, zAxis) 
        rot = rot3 

        ## Set up standard xform from origin
        xlate.setTrans(plane.normal * -plane.d) 

        ## concatenate
        xform = xlate * rot 

        ## Generate vertex data
        ## Lock the whole buffer
        float* pPos = static_cast<float*>(
            vbufpos.lock(HardwareBuffer.HBL_DISCARD) ) 
        float* pNorm = static_cast<float*>(
            vbufnorm.lock(HardwareBuffer.HBL_DISCARD) ) 
        float* pTex = static_cast<float*>(
            vbuftex.lock(HardwareBuffer.HBL_DISCARD) ) 
        xSpace = width / xsegments 
        ySpace = height / ysegments 
        halfWidth = width / 2 
        halfHeight = height / 2 
        xTex = (1.0 * xTile) / xsegments 
        yTex = (1.0 * yTile) / ysegments 
        Vector3 vec 
        Vector3 min, max 
        maxSquaredLength 
        bool firstTime = True 

        for (y = 0  y < ysegments + 1; ++y)

            for (x = 0  x < xsegments + 1; ++x)

                ## Work out centered on origin
                vec.x = (x * xSpace) - halfWidth 
                vec.y = (y * ySpace) - halfHeight 
                vec.z = 0.0 
                ## Transform by orientation and distance
                vec = xform * vec 
                ## Assign to geometry
                *pPos++ = vec.x 
                *pPos++ = vec.y 
                *pPos++ = vec.z 

                ## Build bounds as we go
                if (firstTime):

                    min = vec 
                    max = vec 
                    maxSquaredLength = vec.squaredLength() 
                    firstTime = False 

                else:

                    min.makeFloor(vec) 
                    max.makeCeil(vec) 
                    maxSquaredLength = std.max(maxSquaredLength, vec.squaredLength()) 


                ## Default normal is along unit Z
                vec = Vector3.UNIT_Z 
                ## Rotate
                vec = rot * vec 

                *pNorm++ = vec.x 
                *pNorm++ = vec.y 
                *pNorm++ = vec.z 

                *pTex++ = x * xTex 
                *pTex++ = 1 - (y * yTex) 


            } ## x
        } ## y

        ## Unlock
        vbufpos.unlock() 
        vbufnorm.unlock() 
        vbuftex.unlock() 
        ## Generate face list
        pSub.useSharedVertices = True 
        ##tesselate2DMesh(pSub, xsegments + 1, ysegments + 1, False, indexBufferUsage, indexShadowBuffer) 
        SubMesh* sm = pSub 
        meshWidth = xsegments + 1 
        meshHeight = ysegments + 1  
        bool doubleSided = False 
        HardwareBuffer.Usage indexBufferUsage = HardwareBuffer::HBU_STATIC_WRITE_ONLY 
        bool indexShadowBuffer = True 
        ## The mesh is built, just make a list of indexes to spit out the triangles
        vInc, uInc, v, u, iterations 
        vCount, uCount 

        if (doubleSided):

            iterations = 2 
            vInc = 1 
            v = 0  ## Start with front

        else:

            iterations = 1 
            vInc = 1 
            v = 0 


        ## Allocate memory for faces
        ## Num faces, width*height*2 (2 tris per square), index count is * 3 on top
        sm.indexData.indexCount = (meshWidth-1) * (meshHeight-1) * 2 * iterations * 3 
        sm.indexData.indexBuffer = HardwareBufferManager.getSingleton().
            createIndexBuffer(HardwareIndexBuffer.IT_16BIT,
            sm.indexData.indexCount, indexBufferUsage, indexShadowBuffer) 

        v1, v2, v3 
        ##bool firstTri = True 
        HardwareIndexBufferSharedPtr ibuf = sm.indexData.indexBuffer 
        ## Lock the whole buffer
        unsigned short* pIndexes = static_cast<unsigned short*>(
            ibuf.lock(HardwareBuffer.HBL_DISCARD) ) 

        while (iterations--)

            ## Make tris in a zigzag pattern (compatible with strips)
            u = 0 
            uInc = 1  ## Start with moving +u

            vCount = meshHeight - 1 
            while (vCount--)

                uCount = meshWidth - 1 
                while (uCount--)

                    ## First Tri in cell
                    ## -----------------
                    v1 = ((v + vInc) * meshWidth) + u 
                    v2 = (v * meshWidth) + u 
                    v3 = ((v + vInc) * meshWidth) + (u + uInc) 
                    ## Output indexes
                    *pIndexes++ = v1 
                    *pIndexes++ = v2 
                    *pIndexes++ = v3 
                    ## Second Tri in cell
                    ## ------------------
                    v1 = ((v + vInc) * meshWidth) + (u + uInc) 
                    v2 = (v * meshWidth) + u 
                    v3 = (v * meshWidth) + (u + uInc) 
                    ## Output indexes
                    *pIndexes++ = v1 
                    *pIndexes++ = v2 
                    *pIndexes++ = v3 

                    ## Next column
                    u += uInc 

                ## Next row
                v += vInc 
                u = 0 




            ## Reverse vInc for double sided
            v = meshHeight - 1 
            vInc = -vInc 


        ## Unlock
        ibuf.unlock() 

        ##pMesh._updateBounds() 
        pMesh._setBounds(AxisAlignedBox(min, max)) 
        pMesh._setBoundingSphereRadius(Math.Sqrt(maxSquaredLength)) 
        ## load
        pMesh.load() 
        pMesh.touch() 


    (self): stressTestStaticGeometry((self):)


		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

		## Create a polight
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(1, -1, -1.5) 
		dir.normalise() 
		l.setDirection(dir) 
		l.setDiffuseColour(1.0, 0.7, 0.0) 


		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 0 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			4500,4500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("Examples/GrassFloor") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		Vector3 min(-2000,0,-2000) 
		Vector3 max(2000,0,2000) 


		Entity* e = self.sceneManager.createEntity("1", "ogrehead.mesh") 
		StaticGeometry* s = 0 

		unsigned count = 10 
		while(count--)

			if(s) self.sceneManager.destroyStaticGeometry(s) :
			s = self.sceneManager.createStaticGeometry("bing") 

			s.addEntity(e, Vector3(100, 100, 100)) 

			s.build() 



		##s.setRenderingDistance(1000) 
		##s.dump("static.txt") 
		##self.sceneManager.showBoundingBoxes(True) 
		mCamera.setLodBias(0.5) 
        



def testManualBlend()

		## create material
		MaterialPtr mat = MaterialManager.getSingleton().create("TestMat", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass * p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.createTextureUnitState("Dirt.jpg") 
		TextureUnitState* t = p.createTextureUnitState("PoolFloorLightingMap.png") 
		t.setColourOperationEx(LBX_BLEND_MANUAL, LBS_TEXTURE, LBS_CURRENT, 
			ColourValue.White, ColourValue::White, 0.75) 

		Entity *planeEnt = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(planeEnt) 
		planeEnt.setMaterialName("TestManual") 





def testBug()

		MaterialPtr m = MaterialManager.getSingleton ().getByName ("BaseWhiteNoLighting")  
		m.getBestTechnique() 




def testTransparencyMipMaps()

		MaterialPtr mat = MaterialManager.getSingleton().create("test", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		## known png with alpha
		Pass* pass = mat.getTechnique(0).getPass(0) 
		pass.createTextureUnitState("ogretext.png") 
		pass.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		## alpha blend
		pass.setDepthWriteEnabled(False) 

		## alpha reject
		##pass.setDepthWriteEnabled(True) 
		##pass.setAlphaRejectSettings(CMPF_LESS, 128) 

		## Define a floor plane mesh
		Plane p 
		p.normal = Vector3.UNIT_Y 
		p.d = 200 
		MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,2000,2000,1,1,True,1,5,5,Vector3.UNIT_Z) 

		## Create an entity (the floor)
		Entity* ent = self.sceneManager.createEntity("floor", "FloorPlane") 
		ent.setMaterialName("test") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 

		self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 
		self.sceneManager.setAmbientLight(ColourValue.White) 



		
		alphaLevel = 0.5 
		MaterialPtr alphamat = MaterialManager.getSingleton().create("testy", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* pass = alphamat.getTechnique(0).getPass(0) 
		pass.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		pass.setDepthWriteEnabled(False) 
		TextureUnitState* t = pass.createTextureUnitState() 
		t.setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, alphaLevel) 

		ent = self.sceneManager.createEntity("asd", "ogrehead.mesh") 
		ent.setMaterialName("testy") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 


		


    (self): testCthNewBlending((self):)

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        ## Accept default settings: polight, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        ##l.setPosition(20,80,50) 


        Entity *ent = self.sceneManager.createEntity("head", "ogrehead_2.mesh") 

        ## Add entity to the root scene node
        self.rootNode = self.sceneManager.getRootSceneNode() 
        static_cast<SceneNode*>(self.rootNode.createChild()).attachObject(ent) 

        mTestNode[0] = static_cast<SceneNode*>(
            self.rootNode.createChild("TestNode", Vector3(100,0,0))) 
        ent = self.sceneManager.createEntity("head2", "ogrehead.mesh") 
        mTestNode[0].attachObject(ent) 

        mTestNode[0].attachObject(l) 
        

        ## Create a skydome
        self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 200 
        MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,2000,2000,1,1,True,1,5,5,Vector3.UNIT_Z) 

        ## Create an entity (the floor)
        ent = self.sceneManager.createEntity("floor", "FloorPlane") 
        ent.setMaterialName("Block/Material1") 

        self.rootNode.attachObject(ent) 

        mCamera.setPosition(0,0,200) 
        mCamera.setDirection(0,0,-1) 

        ##self.sceneManager.setDisplaySceneNodes(True) 



    (self): testMatrices((self):)

        mCamera.setPosition(0,0,500) 
        mCamera.lookAt(0,0,0) 
         Matrix4& view = mCamera.getViewMatrix() 
         Matrix4& proj = mCamera.getProjectionMatrix() 

        Matrix4 viewproj = proj * view 

        Vector3 point3d(100,100,0) 

        Vector3 projPo= viewproj * point3d 

        point3d = Vector3(100,100,400) 
        projPo= viewproj * point3d 

    (self): testBasicPlane()

        /*
        ## Create a light
        Light* l = self.sceneManager.createLight("MainLight") 
        ## Accept default settings: polight, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50) 
        */

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 
        Entity *ent 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 200 
        MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,2000,2000,1,1,True,1,5,5,Vector3.UNIT_Z) 

        ## Create an entity (the floor)
        ent = self.sceneManager.createEntity("floor", "FloorPlane") 
        ent.setMaterialName("Examples/RustySteel") 

        self.sceneManager.getRootSceneNode().attachObject(ent) 

        Entity* sphereEnt = self.sceneManager.createEntity("ogre", "ogrehead.mesh") 
		
		self.rootNode = self.sceneManager.getRootSceneNode() 
        SceneNode* node = self.sceneManager.createSceneNode() 
        node.attachObject(sphereEnt) 
        self.rootNode.addChild(node) 



    (self): testAlpha()

        /*
        ## Create a light
        Light* l = self.sceneManager.createLight("MainLight") 
        ## Accept default settings: polight, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50) 
        */

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 
        Entity *ent 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 200 
        MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,2000,2000,1,1,True,1,5,5,Vector3.UNIT_Z) 

        p.normal = Vector3.UNIT_Z 
        p.d = 200 
        MeshManager.getSingleton().createPlane("WallPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,2000,2000,1,1,True,1,5,5,Vector3.UNIT_Y) 

        ## Create an entity (the floor)
        ent = self.sceneManager.createEntity("floor", "FloorPlane") 
        ent.setMaterialName("Core/StatsBlockBorder/Up") 

        self.sceneManager.getRootSceneNode().attachObject(ent) 

        ent = self.sceneManager.createEntity("wall", "WallPlane") 
        ent.setMaterialName("Core/StatsBlockBorder/Up") 

        self.sceneManager.getRootSceneNode().attachObject(ent) 


        Entity* sphereEnt = self.sceneManager.createEntity("ogre", "ogrehead.mesh") 
		
		self.rootNode = self.sceneManager.getRootSceneNode() 
        SceneNode* node = self.sceneManager.createSceneNode() 
        node.attachObject(sphereEnt) 
        self.rootNode.addChild(node) 

        self.sceneManager.showBoundingBoxes(True) 


    (self): testBsp()

        ## Load Quake3 locations from a file
        ConfigFile cf 

        cf.load("quake3settings.cfg") 

        String quakePk3 = cf.getSetting("Pak0Location") 
        String quakeLevel = cf.getSetting("Map") 

		ResourceGroupManager.getSingleton().addResourceLocation(quakePk3, "Zip") 


        ## Load world geometry
        self.sceneManager.setWorldGeometry(quakeLevel) 

        ## modify camera for close work
        mCamera.setNearClipDistance(4) 
        mCamera.setFarClipDistance(4000) 

        ## Also change position, and set Quake-type orientation
        ## Get random player start point
        ViewPovp = self.sceneManager.getSuggestedViewpoint(True) 
        mCamera.setPosition(vp.position) 
        mCamera.pitch(Degree(90))  ## Quake uses X/Y horizon, Z up
        mCamera.rotate(vp.orientation) 
        ## Don't yaw along variable axis, causes leaning
        mCamera.setFixedYawAxis(True, Vector3.UNIT_Z) 



    (self): testAnimation()

        Light* l = self.sceneManager.createLight("MainLight") 
        l.setPosition(200,110,0) 
        l.setType(Light.LT_POINT) 
        Entity *ent 

        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.4)) 

        ## Create an entity (the plant)
        ent = self.sceneManager.createEntity("1", "skeletonandpose.mesh") 

        SceneNode* node = static_cast<SceneNode*>(self.sceneManager.getRootSceneNode().createChild(Vector3(-50,0,0))) 
        node.attachObject(ent) 
        node.scale(2,2,2) 

        mAnimState = ent.getAnimationState("pose2_Clip") 
        mAnimState.setEnabled(True) 

        mWindow.getViewport(0).setBackgroundColour(ColourValue(1,0,0)) 




def testAnimationBlend()

		Light* l = self.sceneManager.createLight("MainLight") 
		l.setPosition(200,110,0) 
		l.setType(Light.LT_POINT) 
		Entity *ent 

		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.4)) 

		## Create an entity (the plant)
		ent = self.sceneManager.createEntity("1", "sticky.mesh") 

		SceneNode* node = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		node.attachObject(ent) 
		node.scale(2,2,2) 

		##mAnimState = ent.getAnimationState("triangle") 
		##mAnimState.setEnabled(True) 
/*
		animBlender = new AnimationBlender(ent) 
		animBlendTarget[0] = "still" 
		animBlendTarget[1] = "triangle" 
		animBlendTargetIndex = 1 
		animBlender.init(animBlendTarget[0]) 
		animBlender.blend(animBlendTarget[1], AnimationBlender.BlendWhileAnimating, 10, False) 
*/
		mCamera.setPosition(-50,0,0) 
		mCamera.lookAt(0,0,0) 




    (self): testGpuPrograms((self):)

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        Entity* pEnt = self.sceneManager.createEntity( "1", "robot.mesh" ) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pEnt) 

        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
            
        pEnt = self.sceneManager.createEntity( "2", "ogrehead.mesh" ) 
        mTestNode[0].attachObject( pEnt ) 
        mTestNode[0].translate(100,0,0) 

        ## Rejig the ogre skin material
        ## Load the programs first

        pEnt.getSubEntity(1).setMaterialName("SimpleTest") 


    (self): testProjection((self):)

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        Entity* pEnt 
        ##pEnt = self.sceneManager.createEntity( "1", "knot.mesh" ) 
        ##self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-30,0,-50)).attachObject(pEnt) 
        ##pEnt.setMaterialName("Examples/OgreLogo") 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Z 
        p.d = 200 
        MeshManager.getSingleton().createPlane("WallPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,1500,1500,1,1,True,1,5,5,Vector3.UNIT_Y) 
        pEnt = self.sceneManager.createEntity( "5", "WallPlane" ) 
        pEnt.setMaterialName("Examples/OgreLogo") 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pEnt) 


        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
            
        ##pEnt = self.sceneManager.createEntity( "2", "ogrehead.mesh" ) 
        ##mTestNode[0].attachObject( pEnt ) 
        mTestNode[0].translate(0, 0, 750) 

        frustum = new Frustum() 
        frustum.setVisible(True) 
        frustum.setFarClipDistance(5000) 
        frustum.setNearClipDistance(200) 
        frustum.setAspectRatio(1) 
        ##frustum.setProjectionType(PT_ORTHOGRAPHIC) 
        mTestNode[0].attachObject(frustum) 

        ## Hook the frustum up to the material
        MaterialPtr mat = MaterialManager.getSingleton().getByName("Examples/OgreLogo") 
        TextureUnitState *t = mat.getTechnique(0).getPass(0).getTextureUnitState(0) 
        t.setProjectiveTexturing(True, frustum) 
        ##t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 



    (self): testMultiViewports((self):)

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        Entity* pEnt = self.sceneManager.createEntity( "1", "knot.mesh" ) 
        self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-30,0,-50)).attachObject(pEnt) 

        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
            
        pEnt = self.sceneManager.createEntity( "2", "ogrehead.mesh" ) 
        mTestNode[0].attachObject( pEnt ) 
        mTestNode[0].translate(0, 0, 200) 

        frustum = new Frustum() 
        ##frustum.setVisible(True) 
        frustum.setFarClipDistance(5000) 
        frustum.setNearClipDistance(100) 
        mTestNode[0].attachObject(frustum) 

        Viewport* vp = self.root.getAutoCreatedWindow().addViewport(mCamera, 1, 0.5, 0.5, 0.5, 0.5) 
        vp.setOverlaysEnabled(False) 
        vp.setBackgroundColour(ColourValue(1,0,0)) 




    ## Just override the mandatory create scene method
    (self): testSceneNodeTracking((self):)


        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 

        ## Create a skydome
        self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

        ## Create a light
        Light* l = self.sceneManager.createLight("MainLight") 
        ## Accept default settings: polight, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50) 

        Entity *ent 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 200 
        MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,200000,200000,20,20,True,1,50,50,Vector3.UNIT_Z) 

        ## Create an entity (the floor)
        ent = self.sceneManager.createEntity("floor", "FloorPlane") 
        ent.setMaterialName("Examples/RustySteel") 
        ## Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 

        ## Add a head, give it it's own node
        SceneNode* headNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ent = self.sceneManager.createEntity("head", "ogrehead.mesh") 
        headNode.attachObject(ent) 

        ## Add another head, give it it's own node
        SceneNode* headNode2 = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ent = self.sceneManager.createEntity("head2", "ogrehead.mesh") 
        headNode2.attachObject(ent) 

        ## Make sure the head node tracks the root
        headNode.setAutoTracking(True, headNode2, Vector3.UNIT_Z) 
        ##headNode.setFixedYawAxis(True) 

        ## Create the camera node & attach camera
        ##SceneNode* camNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ##camNode.attachObject(mCamera) 

        ## set up spline animation of node
        Animation* anim = self.sceneManager.createAnimation("CameraTrack", 10) 
        ## Spline it for nice curves
        anim.setInterpolationMode(Animation.IM_SPLINE) 
        ## Create a track to animate the head's node
        NodeAnimationTrack* track = anim.createNodeTrack(0, headNode) 
        ## Setup keyframes
        TransformKeyFrame* key = track.createNodeKeyFrame(0)  ## startposition
        key = track.createNodeKeyFrame(2.5) 
        key.setTranslate(Vector3(500,500,-1000)) 
        key = track.createNodeKeyFrame(5) 
        key.setTranslate(Vector3(-1500,1000,-600)) 
        key = track.createNodeKeyFrame(7.5) 
        key.setTranslate(Vector3(0,-100,0)) 
        key = track.createNodeKeyFrame(10) 
        key.setTranslate(Vector3(0,0,0)) 
        ## Create a track to animate the second head's node
        track = anim.createNodeTrack(1, headNode2) 
        ## Setup keyframes
        key = track.createNodeKeyFrame(0)  ## startposition
        key = track.createNodeKeyFrame(2.5) 
        key.setTranslate(Vector3(-500,600,-100)) 
        key = track.createNodeKeyFrame(5) 
        key.setTranslate(Vector3(800,200,-600)) 
        key = track.createNodeKeyFrame(7.5) 
        key.setTranslate(Vector3(200,-1000,0)) 
        key = track.createNodeKeyFrame(10) 
        key.setTranslate(Vector3(30,70,110)) 
        ## Create a new animation state to track self
        mAnimState = self.sceneManager.createAnimationState("CameraTrack") 
        mAnimState.setEnabled(True) 

        ## Put in a bit of fog for the hell of it
        self.sceneManager.setFog(FOG_EXP, ColourValue.White, 0.0002) 





    (self): testDistortion((self):)

        theCam = mCamera 
        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        Entity* pEnt 

        RenderTexture* rttTex = self.root.getRenderSystem().createRenderTexture( "Refraction", 512, 512 ) 

            Viewport *v = rttTex.addViewport( mCamera ) 
            MaterialPtr mat = MaterialManager.getSingleton().getByName("Examples/FresnelReflectionRefraction") 
            mat.getTechnique(0).getPass(0).getTextureUnitState(2).setTextureName("Refraction") 
            v.setOverlaysEnabled(False) 
            rttTex.addListener(&mRefractionListener) 


        rttTex = self.root.getRenderSystem().createRenderTexture( "Reflection", 512, 512 ) 

            Viewport *v = rttTex.addViewport( mCamera ) 
            MaterialPtr mat = MaterialManager.getSingleton().getByName("Examples/FresnelReflectionRefraction") 
            mat.getTechnique(0).getPass(0).getTextureUnitState(1).setTextureName("Reflection") 
            v.setOverlaysEnabled(False) 
            rttTex.addListener(&mReflectionListener) 

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 100 
        MeshManager.getSingleton().createPlane("WallPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        pPlaneEnt = self.sceneManager.createEntity( "5", "WallPlane" ) 
        pPlaneEnt.setMaterialName("Examples/FresnelReflectionRefraction") 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

        
        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox") 

        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        i 
        for (i = 0  i < 10; ++i)

            pEnt = self.sceneManager.createEntity( "ogre" + StringConverter.toString(i), "ogrehead.mesh" ) 
            self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(i*100 - 500, -75, 0)).attachObject(pEnt) 
            pEnt = self.sceneManager.createEntity( "knot" + StringConverter.toString(i), "knot.mesh" ) 
            self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(i*100 - 500, 140, 0)).attachObject(pEnt) 







    (self): testEdgeBuilderSingleIndexBufSingleVertexBuf()

        /* This tests the edge builders ability to find shared edges in the simple case
           of a single index buffer referencing a single vertex buffer
        */
        VertexData vd 
        IndexData id 
        ## Test pyramid
        vd.vertexCount = 4 
        vd.vertexStart = 0 
        vd.vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd.vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        HardwareVertexBufferSharedPtr vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 4, HardwareBuffer::HBU_STATIC,True) 
        vd.vertexBufferBinding.setBinding(0, vbuf) 
        float* pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 50   *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 100; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = -50;
        vbuf.unlock() 
            
        id.indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 12, HardwareBuffer::HBU_STATIC, True) 
        id.indexCount = 12 
        id.indexStart = 0 
        unsigned short* pIdx = static_cast<unsigned short*>(id.indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 1; *pIdx++ = 2;
        *pIdx++ = 0  *pIdx++ = 2; *pIdx++ = 3;
        *pIdx++ = 1  *pIdx++ = 3; *pIdx++ = 2;
        *pIdx++ = 0  *pIdx++ = 3; *pIdx++ = 1;
        id.indexBuffer.unlock() 

        EdgeListBuilder edgeBuilder 
        edgeBuilder.addVertexData(&vd) 
        edgeBuilder.addIndexData(&id) 
        EdgeData* edgeData = edgeBuilder.build() 

        edgeData.log(LogManager.getSingleton().getDefaultLog()) 

        delete edgeData 




    (self): testEdgeBuilderMultiIndexBufSingleVertexBuf()

        /* This tests the edge builders ability to find shared edges when there are
           multiple index sets (submeshes) using a single vertex buffer.
        */
        VertexData vd 
        IndexData id[4] 
        ## Test pyramid
        vd.vertexCount = 4 
        vd.vertexStart = 0 
        vd.vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd.vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        HardwareVertexBufferSharedPtr vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 4, HardwareBuffer::HBU_STATIC,True) 
        vd.vertexBufferBinding.setBinding(0, vbuf) 
        float* pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 50   *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 100; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = -50;
        vbuf.unlock() 
            
        id[0].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[0].indexCount = 3 
        id[0].indexStart = 0 
        unsigned short* pIdx = static_cast<unsigned short*>(id[0].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 1; *pIdx++ = 2;
        id[0].indexBuffer.unlock() 

        id[1].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[1].indexCount = 3 
        id[1].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[1].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 2; *pIdx++ = 3;
        id[1].indexBuffer.unlock() 

        id[2].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[2].indexCount = 3 
        id[2].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[2].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 1  *pIdx++ = 3; *pIdx++ = 2;
        id[2].indexBuffer.unlock() 

        id[3].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[3].indexCount = 3 
        id[3].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[3].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 3; *pIdx++ = 1;
        id[3].indexBuffer.unlock() 

        EdgeListBuilder edgeBuilder 
        edgeBuilder.addVertexData(&vd) 
        edgeBuilder.addIndexData(&id[0]) 
        edgeBuilder.addIndexData(&id[1]) 
        edgeBuilder.addIndexData(&id[2]) 
        edgeBuilder.addIndexData(&id[3]) 
        EdgeData* edgeData = edgeBuilder.build() 

        edgeData.log(LogManager.getSingleton().getDefaultLog()) 

        delete edgeData 





    (self): testEdgeBuilderMultiIndexBufMultiVertexBuf()

        /* This tests the edge builders ability to find shared edges when there are
           both multiple index sets (submeshes) each using a different vertex buffer
           (not using shared geoemtry). 
        */

        VertexData vd[4] 
        IndexData id[4] 
        ## Test pyramid
        vd[0].vertexCount = 3 
        vd[0].vertexStart = 0 
        vd[0].vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd[0].vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        HardwareVertexBufferSharedPtr vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 3, HardwareBuffer::HBU_STATIC,True) 
        vd[0].vertexBufferBinding.setBinding(0, vbuf) 
        float* pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 50   *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 100; *pFloat++ = 0  ;
        vbuf.unlock() 

        vd[1].vertexCount = 3 
        vd[1].vertexStart = 0 
        vd[1].vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd[1].vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 3, HardwareBuffer::HBU_STATIC,True) 
        vd[1].vertexBufferBinding.setBinding(0, vbuf) 
        pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 100; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = -50;
        vbuf.unlock() 

        vd[2].vertexCount = 3 
        vd[2].vertexStart = 0 
        vd[2].vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd[2].vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 3, HardwareBuffer::HBU_STATIC,True) 
        vd[2].vertexBufferBinding.setBinding(0, vbuf) 
        pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 50   *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 100; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = -50;
        vbuf.unlock() 
            
        vd[3].vertexCount = 3 
        vd[3].vertexStart = 0 
        vd[3].vertexDeclaration = HardwareBufferManager.getSingleton().createVertexDeclaration() 
        vd[3].vertexDeclaration.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
        vbuf = HardwareBufferManager.getSingleton().createVertexBuffer(sizeof(float)*3, 3, HardwareBuffer::HBU_STATIC,True) 
        vd[3].vertexBufferBinding.setBinding(0, vbuf) 
        pFloat = static_cast<float*>(vbuf.lock(HardwareBuffer.HBL_DISCARD)) 
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 50   *pFloat++ = 0  ; *pFloat++ = 0  ;
        *pFloat++ = 0    *pFloat++ = 0  ; *pFloat++ = -50;
        vbuf.unlock() 

        id[0].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[0].indexCount = 3 
        id[0].indexStart = 0 
        unsigned short* pIdx = static_cast<unsigned short*>(id[0].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 1; *pIdx++ = 2;
        id[0].indexBuffer.unlock() 

        id[1].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[1].indexCount = 3 
        id[1].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[1].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 1; *pIdx++ = 2;
        id[1].indexBuffer.unlock() 

        id[2].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[2].indexCount = 3 
        id[2].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[2].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 2; *pIdx++ = 1;
        id[2].indexBuffer.unlock() 

        id[3].indexBuffer = HardwareBufferManager.getSingleton().createIndexBuffer(
            HardwareIndexBuffer.IT_16BIT, 3, HardwareBuffer::HBU_STATIC, True) 
        id[3].indexCount = 3 
        id[3].indexStart = 0 
        pIdx = static_cast<unsigned short*>(id[3].indexBuffer.lock(HardwareBuffer.HBL_DISCARD)) 
        *pIdx++ = 0  *pIdx++ = 2; *pIdx++ = 1;
        id[3].indexBuffer.unlock() 

        EdgeListBuilder edgeBuilder 
        edgeBuilder.addVertexData(&vd[0]) 
        edgeBuilder.addVertexData(&vd[1]) 
        edgeBuilder.addVertexData(&vd[2]) 
        edgeBuilder.addVertexData(&vd[3]) 
        edgeBuilder.addIndexData(&id[0], 0) 
        edgeBuilder.addIndexData(&id[1], 1) 
        edgeBuilder.addIndexData(&id[2], 2) 
        edgeBuilder.addIndexData(&id[3], 3) 
        EdgeData* edgeData = edgeBuilder.build() 

        edgeData.log(LogManager.getSingleton().getDefaultLog()) 

        delete edgeData 




    (self): testSkeletalAnimation()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
        ##mWindow.getViewport(0).setBackgroundColour(ColourValue.White) 



        Entity *ent = self.sceneManager.createEntity("robot", "test.mesh") 
        ## Uncomment the below to test software skinning
        ##ent.setMaterialName("Examples/Rocky") 
        ## Add entity to the scene node
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
        mAnimState = ent.getAnimationState("Default") 
        mAnimState.setEnabled(True) 

        ## Give it a little ambience with lights
        Light* l 
        l = self.sceneManager.createLight("BlueLight") 
        l.setPosition(-200,-80,-100) 
        l.setDiffuseColour(0.5, 0.5, 1.0) 

        l = self.sceneManager.createLight("GreenLight") 
        l.setPosition(0,0,-100) 
        l.setDiffuseColour(0.5, 1.0, 0.5) 

        ## Position the camera
        mCamera.setPosition(100,50,100) 
        mCamera.lookAt(-50,50,0) 

        ## Report whether hardware skinning is enabled or not
        Technique* t = ent.getSubEntity(0).getMaterial().getBestTechnique() 
        Pass* p = t.getPass(0) 
		OverlayElement* guiDbg = OverlayManager.getSingleton().getOverlayElement("Core/DebugText") 
        if (p.hasVertexProgram() && :
            p.getVertexProgram().isSkeletalAnimationIncluded())

			guiDbg.setCaption("Hardware skinning is enabled") 

        else:

            guiDbg.setCaption("Software skinning is enabled") 






    (self): testPrepareShadowVolume((self):)


        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        mTestNode[0] = (SceneNode*)self.sceneManager.getRootSceneNode().createChild() 
            
        Entity* pEnt = self.sceneManager.createEntity( "1", "ogrehead.mesh" ) 
        mTestNode[0].attachObject( pEnt ) 

        pEnt.getMesh().prepareForShadowVolume() 



    (self): testWindowedViewportMode()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        mTestNode[0] = (SceneNode*)self.sceneManager.getRootSceneNode().createChild() 

        Entity* pEnt = self.sceneManager.createEntity( "1", "ogrehead.mesh" ) 
        mTestNode[0].attachObject( pEnt ) 

        mCamera.setWindow(0,0,0.5,0.5) 


    (self): testSubEntityVisibility()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        mTestNode[0] = (SceneNode*)self.sceneManager.getRootSceneNode().createChild() 

        Entity* pEnt = self.sceneManager.createEntity( "1", "ogrehead.mesh" ) 
        mTestNode[0].attachObject( pEnt ) 

        pEnt.getSubEntity(1).setVisible(False) 




    (self): testAttachObjectsToBones()

        Entity *ent 
        for (i = 0  i < 12; ++i)

            ent = self.sceneManager.createEntity("robot" + StringConverter.toString(i), "robot.mesh") 
			if (False)##i % 2):

				Entity* ent2 = self.sceneManager.createEntity("plane" + StringConverter.toString(i), "razor.mesh") 
				ent.attachObjectToBone("Joint8", ent2) 

			else:

				ParticleSystem* psys = self.sceneManager.createParticleSystem("psys" + StringConverter.toString(i), "Examples/PurpleFountain") 
				psys.getEmitter(0).setTimeToLive(0.2) 
				ent.attachObjectToBone("Joint15", psys) 

            ## Add entity to the scene node
            self.sceneManager.getRootSceneNode().createChildSceneNode(
                Vector3(0,0,(i*200)-(12*200/2))).attachObject(ent) 

			ent.getParentNode().yaw(Degree(i * 45)) 

        mAnimState = ent.getAnimationState("Walk") 
        mAnimState.setEnabled(True) 



        ## Give it a little ambience with lights
        Light* l 
        l = self.sceneManager.createLight("BlueLight") 
        l.setPosition(-200,-80,-100) 
        l.setDiffuseColour(0.5, 0.5, 1.0) 

        l = self.sceneManager.createLight("GreenLight") 
        l.setPosition(0,0,-100) 
        l.setDiffuseColour(0.5, 1.0, 0.5) 

        ## Position the camera
        mCamera.setPosition(100,50,100) 
        mCamera.lookAt(-50,50,0) 

        self.sceneManager.setAmbientLight(ColourValue(1,1,1,1)) 
        self.sceneManager.showBoundingBoxes(True) 


    (self): testOrtho()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.0, 0.0, 0.0)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setPosition(800,600,0) 

        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mLightNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 

        Entity* pEnt = self.sceneManager.createEntity( "3", "knot.mesh" ) 
        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-200, 0, -200)) 
        mTestNode[1].attachObject( pEnt ) 

        pEnt = self.sceneManager.createEntity( "4", "knot.mesh" ) 
        mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 0, 200)) 
        mTestNode[2].attachObject( pEnt ) 


        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox") 


        Plane plane 
        plane.normal = Vector3.UNIT_Y 
        plane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
        pPlaneEnt.setMaterialName("2 - Default") 
        pPlaneEnt.setCastShadows(False) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

        mCamera.setFixedYawAxis(False) 
        mCamera.setProjectionType(PT_ORTHOGRAPHIC) 
        mCamera.setPosition(0,10000,0) 
        mCamera.lookAt(0,0,0) 
        mCamera.setNearClipDistance(1000) 



def testManualLOD()

		MeshPtr msh1 = (MeshPtr)MeshManager.getSingleton().load("robot.mesh", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 

		msh1.createManualLodLevel(200, "razor.mesh") 
		msh1.createManualLodLevel(500, "sphere.mesh") 

		Entity *ent 
		for (i = 0  i < 5; ++i)

			ent = self.sceneManager.createEntity("robot" + StringConverter.toString(i), "robot.mesh") 
			## Add entity to the scene node
			self.sceneManager.getRootSceneNode().createChildSceneNode(
				Vector3(0,0,(i*50)-(5*50/2))).attachObject(ent) 

		mAnimState = ent.getAnimationState("Walk") 
		mAnimState.setEnabled(True) 



		## Give it a little ambience with lights
		Light* l 
		l = self.sceneManager.createLight("BlueLight") 
		l.setPosition(-200,-80,-100) 
		l.setDiffuseColour(0.5, 0.5, 1.0) 

		l = self.sceneManager.createLight("GreenLight") 
		l.setPosition(0,0,-100) 
		l.setDiffuseColour(0.5, 1.0, 0.5) 

		## Position the camera
		mCamera.setPosition(100,50,100) 
		mCamera.lookAt(-50,50,0) 

		self.sceneManager.setAmbientLight(ColourValue.White) 



def testFallbackResourceGroup()

		## Load all textures from new resource group "Test"
		ResourceGroupManager.getSingleton().removeResourceLocation("../../../Media/materials/textures") 
		ResourceGroupManager.getSingleton().removeResourceLocation("../../../Media/models") 
		ResourceGroupManager.getSingleton().createResourceGroup("Test") 
		ResourceGroupManager.getSingleton().addResourceLocation("../../../Media/materials/textures", "FileSystem", "Test") 
		ResourceGroupManager.getSingleton().addResourceLocation("../../../Media/models", "FileSystem", "Test") 

		## Load a texture from default group (won't be found there, but should fall back)
		TextureManager.getSingleton().load("dirt01.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) 

		ResourceGroupManager.getSingleton().unloadUnreferencedResourcesInGroup("Test") 

		## Add a few robots for fun
		Entity *ent 
		for (i = 0  i < 5; ++i)

			ent = self.sceneManager.createEntity("robot" + StringConverter.toString(i), "robot.mesh") 
			## Add entity to the scene node
			self.sceneManager.getRootSceneNode().createChildSceneNode(
				Vector3(0,0,(i*50)-(5*50/2))).attachObject(ent) 

		## Give it a little ambience with lights
		Light* l 
		l = self.sceneManager.createLight("BlueLight") 
		l.setPosition(-200,-80,-100) 
		l.setDiffuseColour(0.5, 0.5, 1.0) 

		l = self.sceneManager.createLight("GreenLight") 
		l.setPosition(0,0,-100) 
		l.setDiffuseColour(0.5, 1.0, 0.5) 

		## Position the camera
		mCamera.setPosition(100,50,100) 
		mCamera.lookAt(-50,50,0) 



def testGeneratedLOD()

		MeshPtr msh1 = (MeshPtr)MeshManager.getSingleton().load("barrel.mesh", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 

		msh1.removeLodLevels() 

		Mesh.LodDistanceList lodList 
		lodList.push_back(50) 
		lodList.push_back(100) 
		lodList.push_back(150) 
		lodList.push_back(200) 
		lodList.push_back(250) 
		lodList.push_back(300) 

		msh1.generateLodLevels(lodList, ProgressiveMesh.VRQ_PROPORTIONAL, 0.3) 

		Entity *ent 
		for (i = 0  i < 1; ++i)

			ent = self.sceneManager.createEntity("tst" + StringConverter.toString(i), "barrel.mesh") 
			## Add entity to the scene node
			self.sceneManager.getRootSceneNode().createChildSceneNode(
				Vector3(0,0,(i*50)-(5*50/2))).attachObject(ent) 


		## Give it a little ambience with lights
		Light* l 
		l = self.sceneManager.createLight("BlueLight") 
		l.setPosition(-200,-80,-100) 
		l.setDiffuseColour(0.5, 0.5, 1.0) 

		l = self.sceneManager.createLight("GreenLight") 
		l.setPosition(0,0,-100) 
		l.setDiffuseColour(0.5, 1.0, 0.5) 

		## Position the camera
		mCamera.setPosition(100,50,100) 
		mCamera.lookAt(-50,50,0) 

		self.sceneManager.setAmbientLight(ColourValue.White) 



    (self): clearSceneSetup()

        bool showOctree = True 
        self.sceneManager.setOption("ShowOctree", &showOctree) 

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 

        ## Create a skydome
        self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

        ## Create a light
        Light* l = self.sceneManager.createLight("MainLight") 
        ## Accept default settings: polight, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50) 

        Entity *ent 

        ## Create an entity (the floor)
        ent = self.sceneManager.createEntity("floor", "FloorPlane") 
        ent.setMaterialName("Examples/RustySteel") 
        ## Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 

        ## Add a head, give it it's own node
        SceneNode* headNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ent = self.sceneManager.createEntity("head", "ogrehead.mesh") 
        headNode.attachObject(ent) 

        ## Add another head, give it it's own node
        SceneNode* headNode2 = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ent = self.sceneManager.createEntity("head2", "ogrehead.mesh") 
        headNode2.attachObject(ent) 

        ## Make sure the head node tracks the root
        headNode.setAutoTracking(True, headNode2, Vector3.UNIT_Z) 
        ##headNode.setFixedYawAxis(True) 

        ## Create the camera node & attach camera
        ##SceneNode* camNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        ##camNode.attachObject(mCamera) 

        ## set up spline animation of node
        Animation* anim = self.sceneManager.createAnimation("CameraTrack", 10) 
        ## Spline it for nice curves
        anim.setInterpolationMode(Animation.IM_SPLINE) 
        ## Create a track to animate the head's node
        NodeAnimationTrack* track = anim.createNodeTrack(0, headNode) 
        ## Setup keyframes
        TransformKeyFrame* key = track.createNodeKeyFrame(0)  ## startposition
        key = track.createNodeKeyFrame(2.5) 
        key.setTranslate(Vector3(500,500,-1000)) 
        key = track.createNodeKeyFrame(5) 
        key.setTranslate(Vector3(-1500,1000,-600)) 
        key = track.createNodeKeyFrame(7.5) 
        key.setTranslate(Vector3(0,-100,0)) 
        key = track.createNodeKeyFrame(10) 
        key.setTranslate(Vector3(0,0,0)) 
        ## Create a track to animate the second head's node
        track = anim.createNodeTrack(1, headNode2) 
        ## Setup keyframes
        key = track.createNodeKeyFrame(0)  ## startposition
        key = track.createNodeKeyFrame(2.5) 
        key.setTranslate(Vector3(-500,600,-100)) 
        key = track.createNodeKeyFrame(5) 
        key.setTranslate(Vector3(800,200,-600)) 
        key = track.createNodeKeyFrame(7.5) 
        key.setTranslate(Vector3(200,-1000,0)) 
        key = track.createNodeKeyFrame(10) 
        key.setTranslate(Vector3(30,70,110)) 
        ## Create a new animation state to track self
        mAnimState = self.sceneManager.createAnimationState("CameraTrack") 
        mAnimState.setEnabled(True) 

    class ClearSceneListener : public FrameListener

    protected:
        SceneManager* self.sceneManager 
        PlayPenApplication* ppApp 

    public:
        ClearSceneListener(SceneManager* sm, PlayPenApplication* target)

            self.sceneManager = sm 
            ppApp = target 

        bool frameStarted( FrameEvent& evt)

            static timeElapsed = 0 

            timeElapsed += evt.timeSinceLastFrame 
            if (timeElapsed > 15):

                self.sceneManager.clearScene() 
                ppApp.clearSceneSetup() 
                timeElapsed = 0 

            return True 


    ClearSceneListener* csListener 
    (self): testClearScene()

        ## Define a floor plane mesh
        Plane p 
        p.normal = Vector3.UNIT_Y 
        p.d = 200 
        MeshManager.getSingleton().createPlane("FloorPlane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			p,200000,200000,20,20,True,1,50,50,Vector3.UNIT_Z) 


        ## leak here I know
        csListener = new ClearSceneListener(self.sceneManager, self) 
        Root.getSingleton().addFrameListener(csListener) 
        clearSceneSetup() 


    (self): testStencilShadows(ShadowTechnique tech, bool pointLight, bool directionalLight)

        self.sceneManager.setShadowTechnique(tech) 
        ##self.sceneManager.setShowDebugShadows(True) 
        self.sceneManager.setShadowDirectionalLightExtrusionDistance(1000) 
        ##self.sceneManager.setShadowColour(ColourValue(0.4, 0.25, 0.25)) 

        ##self.sceneManager.setShadowFarDistance(800) 
        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.0, 0.0, 0.0)) 
        
        ## Polight
        if(pointLight):

            mLight = self.sceneManager.createLight("MainLight") 
            mLight.setPosition(-400,400,-300) 
            mLight.setDiffuseColour(0.9, 0.9, 1) 
            mLight.setSpecularColour(0.9, 0.9, 1) 
            mLight.setAttenuation(6000,1,0.001,0) 

        ## Directional light
        if (directionalLight):

            mLight = self.sceneManager.createLight("Light2") 
            Vector3 dir(-1,-1,0) 
            dir.normalise() 
            mLight.setType(Light.LT_DIRECTIONAL) 
            mLight.setDirection(dir) 
            mLight.setDiffuseColour(1, 1, 0.8) 
            mLight.setSpecularColour(1, 1, 1) 


        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 

		## Hardware skin
        Entity* pEnt 
        pEnt = self.sceneManager.createEntity( "1", "robot.mesh" ) 
        AnimationState* anim = pEnt.getAnimationState("Walk") 
        anim.setEnabled(True) 
		mAnimStateList.push_back(anim) 
		mTestNode[0].attachObject( pEnt ) 

		## Software skin
		pEnt = self.sceneManager.createEntity( "12", "robot.mesh" ) 
		anim = pEnt.getAnimationState("Walk") 
		anim.setEnabled(True) 
		mAnimStateList.push_back(anim) 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 0, 0)).attachObject(pEnt) 
		pEnt.setMaterialName("Examples/Rocky") 


        ## Does not receive shadows
        pEnt = self.sceneManager.createEntity( "3", "knot.mesh" ) 
        pEnt.setMaterialName("Examples/EnvMappedRustySteel") 
        MaterialPtr mat2 = MaterialManager.getSingleton().getByName("Examples/EnvMappedRustySteel") 
        mat2.setReceiveShadows(False) 
        mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-200, 0, -200)) 
        mTestNode[2].attachObject( pEnt ) 

        ## Transparent object 
        pEnt = self.sceneManager.createEntity( "3.5", "knot.mesh" ) 
        pEnt.setMaterialName("Examples/TransparentTest") 
        MaterialPtr mat3 = MaterialManager.getSingleton().getByName("Examples/TransparentTest") 
		pEnt.setCastShadows(False) 
        mTestNode[3] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(350, 0, -200)) 
        mTestNode[3].attachObject( pEnt ) 

		## User test
		/*
		pEnt = self.sceneManager.createEntity( "3.6", "ogre_male_endCaps.mesh" ) 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(0, 0, 100)).attachObject( pEnt ) 
		*/

        MeshPtr msh = MeshManager.getSingleton().load("knot.mesh", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        unsigned short src, dest 
        if (!msh.suggestTangentVectorBuildParams(VES_TANGENT, src, dest)):

            msh.buildTangentVectors(VES_TANGENT, src, dest) 

        pEnt = self.sceneManager.createEntity( "4", "knot.mesh" ) 
        pEnt.setMaterialName("Examples/BumpMapping/MultiLightSpecular") 
        mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 0, 200)) 
        mTestNode[2].attachObject( pEnt ) 

		## controller based material
		pEnt = self.sceneManager.createEntity( "432", "knot.mesh" ) 
		pEnt.setMaterialName("Examples/TextureEffect2") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 200, 200)).attachObject( pEnt ) 

        ParticleSystem* pSys2 = self.sceneManager.createParticleSystem("smoke", 
            "Examples/Smoke") 
        mTestNode[4] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-300, -100, 200)) 
        mTestNode[4].attachObject(pSys2) 


        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox") 


        Plane plane 
        plane.normal = Vector3.UNIT_Y 
        plane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
        pPlaneEnt.setMaterialName("2 - Default") 
        pPlaneEnt.setCastShadows(False) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

        mCamera.setPosition(180, 34, 223) 
        mCamera.setOrientation(Quaternion(0.7265, -0.2064, 0.6304, 0.1791)) 

		## Create a render texture
		TexturePtr rtt = TextureManager.getSingleton().createManual("rtt0", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET) 
		rtt.getBuffer().getRenderTarget().addViewport(mCamera) 
		## Create an overlay showing the rtt
		MaterialPtr debugMat = MaterialManager.getSingleton().create(
			"DebugRTT", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		debugMat.getTechnique(0).getPass(0).setLightingEnabled(False) 
		TextureUnitState *t = debugMat.getTechnique(0).getPass(0).createTextureUnitState("rtt0") 
		t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
		OverlayContainer* debugPanel = (OverlayContainer*)
			(OverlayManager.getSingleton().createOverlayElement("Panel", "Ogre/DebugShadowPanel")) 
		debugPanel._setPosition(0.6, 0) 
		debugPanel._setDimensions(0.4, 0.6) 
		debugPanel.setMaterialName("DebugRTT") 
		Overlay* debugOverlay = OverlayManager.getSingleton().getByName("Core/DebugOverlay") 
		debugOverlay.add2D(debugPanel) 




    (self): test2Spotlights()

        self.sceneManager.setAmbientLight(ColourValue(0.3, 0.3, 0.3)) 

        mLight = self.sceneManager.createLight("MainLight") 
        ## Spotlight test
        mLight.setType(Light.LT_SPOTLIGHT) 
        mLight.setDiffuseColour(1.0, 0.0, 0.8) 
        mLight.setSpotlightRange(Degree(30), Degree(40)) 
        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[0].setPosition(800,600,0) 
        mTestNode[0].lookAt(Vector3(800,0,0), Node.TS_WORLD, Vector3::UNIT_Z) 
        mTestNode[0].attachObject(mLight) 

        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mLight = self.sceneManager.createLight("AnotherLight") 
        ## Spotlight test
        mLight.setType(Light.LT_SPOTLIGHT) 
        mLight.setDiffuseColour(0, 1.0, 0.8) 
        mLight.setSpotlightRange(Degree(30), Degree(40)) 
        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[1].setPosition(0,600,800) 
        mTestNode[1].lookAt(Vector3(0,0,800), Node.TS_WORLD, Vector3::UNIT_Z) 
        mTestNode[1].attachObject(mLight) 


        Plane plane 
        plane.normal = Vector3.UNIT_Y 
        plane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            3500,3500,100,100,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt 
        pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
        pPlaneEnt.setMaterialName("2 - Default") 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 




    (self): testTextureShadows(ShadowTechnique tech)

        self.sceneManager.setShadowTextureSize(1024) 
        self.sceneManager.setShadowTechnique(tech) 
		/*
		LiSPSMShadowCameraSetup* lispsmSetup = new LiSPSMShadowCameraSetup() 
		lispsmSetup.setOptimalAdjustFactor(0.5) 
		self.sceneManager.setShadowCameraSetup(ShadowCameraSetupPtr(lispsmSetup)) 
		*/
		self.sceneManager.setShadowCameraSetup(ShadowCameraSetupPtr(new PlaneOptimalShadowCameraSetup(&movablePlane))) 

        self.sceneManager.setShadowFarDistance(1500) 
        self.sceneManager.setShadowColour(ColourValue(0.35, 0.35, 0.35)) 
        ##self.sceneManager.setShadowFarDistance(800) 
        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.3, 0.3, 0.3)) 

        mLight = self.sceneManager.createLight("MainLight") 

        ## Directional test
        mLight.setType(Light.LT_DIRECTIONAL) 
        Vector3 vec(-1,-1,0) 
        vec.normalise() 
        mLight.setDirection(vec) 

		
        ## Spotlight test
		/*
        mLight.setType(Light.LT_SPOTLIGHT) 
        mLight.setDiffuseColour(1.0, 1.0, 0.8) 
        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[0].setPosition(800,600,0) 
        mTestNode[0].lookAt(Vector3(0,0,0), Node.TS_WORLD, Vector3::UNIT_Z) 
        mTestNode[0].attachObject(mLight) 
		*/

        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 


        Entity* pEnt 
        pEnt = self.sceneManager.createEntity( "1", "robot.mesh" ) 
		##pEnt.setRenderingDistance(100) 
        mAnimState = pEnt.getAnimationState("Walk") 
        mAnimState.setEnabled(True) 
        ##pEnt.setMaterialName("2 - Default") 
        mTestNode[1].attachObject( pEnt ) 
        mTestNode[1].translate(0,-100,0) 

        pEnt = self.sceneManager.createEntity( "3", "knot.mesh" ) 
        mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-200, 0, -200)) 
        mTestNode[2].attachObject( pEnt ) 

        ## Transparent object (can force cast shadows)
        pEnt = self.sceneManager.createEntity( "3.5", "knot.mesh" ) 
		MaterialPtr tmat = MaterialManager.getSingleton().create("TestAlphaTransparency", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		tmat.setTransparencyCastsShadows(True) 
		Pass* tpass = tmat.getTechnique(0).getPass(0) 
		tpass.setAlphaRejectSettings(CMPF_GREATER, 150) 
		tpass.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		tpass.createTextureUnitState("gras_02.png") 
		tpass.setCullingMode(CULL_NONE) 

        pEnt.setMaterialName("TestAlphaTransparency") 
        mTestNode[3] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(350, 0, -200)) 
        mTestNode[3].attachObject( pEnt ) 

        MeshPtr msh = MeshManager.getSingleton().load("knot.mesh",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        msh.buildTangentVectors(VES_TANGENT, 0, 0) 
        pEnt = self.sceneManager.createEntity( "4", "knot.mesh" ) 
        ##pEnt.setMaterialName("Examples/BumpMapping/MultiLightSpecular") 
        mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 0, 200)) 
        mTestNode[2].attachObject( pEnt ) 

        self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox") 


        movablePlane.normal = Vector3.UNIT_Y 
        movablePlane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, movablePlane,
            1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt 
        pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		if (tech & SHADOWDETAILTYPE_CUSTOM_SEQUENCE):

			pPlaneEnt.setMaterialName("Examples/Plane/ShadowsCustomSequence") 

		else:

			pPlaneEnt.setMaterialName("2 - Default") 

        pPlaneEnt.setCastShadows(False) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

        ## Set up a debug panel to display the shadow
        MaterialPtr debugMat = MaterialManager.getSingleton().create(
            "Ogre/DebugShadowMap", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        debugMat.getTechnique(0).getPass(0).setLightingEnabled(False) 
		TexturePtr shadowTex = self.sceneManager.getShadowTexture(0) 
        TextureUnitState *t = debugMat.getTechnique(0).getPass(0).createTextureUnitState(shadowTex.getName()) 
        t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
        ##t = debugMat.getTechnique(0).getPass(0).createTextureUnitState("spot_shadow_fade.png") 
        ##t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
        ##t.setColourOperation(LBO_ADD) 

        OverlayContainer* debugPanel = (OverlayContainer*)
            (OverlayManager.getSingleton().createOverlayElement("Panel", "Ogre/DebugShadowPanel")) 
        debugPanel._setPosition(0.8, 0) 
        debugPanel._setDimensions(0.2, 0.3) 
        debugPanel.setMaterialName("Ogre/DebugShadowMap") 
        Overlay* debugOverlay = OverlayManager.getSingleton().getByName("Core/DebugOverlay") 
        debugOverlay.add2D(debugPanel) 



        ParticleSystem* pSys2 = self.sceneManager.createParticleSystem("smoke", 
            "Examples/Smoke") 
        mTestNode[4] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-300, -100, 200)) 
        mTestNode[4].attachObject(pSys2) 




def testTextureShadowsCustomCasterMat(ShadowTechnique tech)


		testTextureShadows(tech) 

		String customCasterMatVp = 
			"(self): customCasterVp(float4 position : POSITION,\n"
			"out float4 oPosition : POSITION,\n"
			"uniform float4x4 worldViewProj)\n"
			"{\n"
			"	oPosition = mul(worldViewProj, position) \n"
			"}\n" 
		String customCasterMatFp = 
			"(self): customCasterFp(\n"
			"out float4 oColor : COLOR)\n"
			"{\n"
			"	oColor = float4(1,1,0,1)  ## just a test\n"
			"}\n" 

		HighLevelGpuProgramPtr vp = HighLevelGpuProgramManager.getSingleton()
			.createProgram("CustomShadowCasterVp", 
				ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
				"cg", GPT_VERTEX_PROGRAM) 
		vp.setSource(customCasterMatVp) 
		vp.setParameter("profiles", "vs_1_1 arbvp1") 
		vp.setParameter("entry_point", "customCasterVp") 
		vp.load() 

		HighLevelGpuProgramPtr fp = HighLevelGpuProgramManager.getSingleton()
			.createProgram("CustomShadowCasterFp", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			"cg", GPT_FRAGMENT_PROGRAM) 
		fp.setSource(customCasterMatFp) 
		fp.setParameter("profiles", "ps_1_1 arbfp1") 
		fp.setParameter("entry_point", "customCasterFp") 
		fp.load() 
		
		MaterialPtr mat = MaterialManager.getSingleton().create("CustomShadowCaster", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setVertexProgram("CustomShadowCasterVp") 
		p.getVertexProgramParameters().setNamedAutoConstant(
			"worldViewProj", GpuProgramParameters.ACT_WORLDVIEWPROJ_MATRIX) 
		p.setFragmentProgram("CustomShadowCasterFp") 

		self.sceneManager.setShadowTextureCasterMaterial("CustomShadowCaster") 


		




def testTextureShadowsCustomReceiverMat(ShadowTechnique tech)

		testTextureShadows(tech) 

		String customReceiverMatVp = 
			"(self): customReceiverVp(float4 position : POSITION,\n"
			"out float4 oPosition : POSITION,\n"
			"out float2 oUV : TEXCOORD0,\n"
			"uniform float4x4 texViewProj,\n"
			"uniform float4x4 worldViewProj)\n"
			"{\n"
			"	oPosition = mul(worldViewProj, position) \n"
			"	float4 suv = mul(texViewProj, position) \n"
			"	oUV = suv.xy / suv.w \n"
			"}\n" 
		String customReceiverMatFp = 
			"(self): customReceiverFp(\n"
			"float2 uv : TEXCOORD0,\n"
			"uniform sampler2D shadowTex : register(s0),\n"
			"out float4 oColor : COLOR)\n"
			"{\n"
			"	float4 shadow = tex2D(shadowTex, uv) \n"
			"	oColor = shadow * float4(1,0,1,1)  ## just a test\n"
			"}\n" 

		HighLevelGpuProgramPtr vp = HighLevelGpuProgramManager.getSingleton()
			.createProgram("CustomShadowReceiverVp", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			"cg", GPT_VERTEX_PROGRAM) 
		vp.setSource(customReceiverMatVp) 
		vp.setParameter("profiles", "vs_1_1 arbvp1") 
		vp.setParameter("entry_point", "customReceiverVp") 
		vp.load() 

		HighLevelGpuProgramPtr fp = HighLevelGpuProgramManager.getSingleton()
			.createProgram("CustomShadowReceiverFp", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			"cg", GPT_FRAGMENT_PROGRAM) 
		fp.setSource(customReceiverMatFp) 
		fp.setParameter("profiles", "ps_1_1 arbfp1") 
		fp.setParameter("entry_point", "customReceiverFp") 
		fp.load() 

		MaterialPtr mat = MaterialManager.getSingleton().create("CustomShadowReceiver", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setVertexProgram("CustomShadowReceiverVp") 
		p.getVertexProgramParameters().setNamedAutoConstant(
			"worldViewProj", GpuProgramParameters.ACT_WORLDVIEWPROJ_MATRIX) 
		p.getVertexProgramParameters().setNamedAutoConstant(
			"texViewProj", GpuProgramParameters.ACT_TEXTURE_VIEWPROJ_MATRIX) 
		p.setFragmentProgram("CustomShadowReceiverFp") 
		p.createTextureUnitState()  ## shadow texture will populate

		self.sceneManager.setShadowTextureReceiverMaterial("CustomShadowReceiver") 






	##---------------------------------------------------------------------------
	class GaussianListener: public Ogre.CompositorInstance::Listener

	protected:
		mVpWidth, mVpHeight 
		## Array params - have to pack in groups of 4 since self is how Cg generates them
		## also prevents dependent texture read problems if ops don't require swizzle
		float mBloomTexWeights[15][4] 
		float mBloomTexOffsetsHorz[15][4] 
		float mBloomTexOffsetsVert[15][4] 
	public:
		GaussianListener() {}
		virtual ~GaussianListener() {}
		(self): notifyViewportSize(width, height)

			mVpWidth = width 
			mVpHeight = height 
			## Calculate gaussian texture offsets & weights
			float deviation = 3.0 
			float texelSize = 1.0 / (float)std.min(mVpWidth, mVpHeight) 

			## central sample, no offset
			mBloomTexOffsetsHorz[0][0] = 0.0 
			mBloomTexOffsetsHorz[0][1] = 0.0 
			mBloomTexWeights[0][0] = mBloomTexWeights[0][1] = 
				mBloomTexWeights[0][2] = Ogre.Math::gaussianDistribution(0, 0, deviation) 
			mBloomTexWeights[0][3] = 1.0 

			## 'pre' samples
			for(i = 1  i < 8; ++i)

				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = 
					mBloomTexWeights[i][2] = Ogre.Math::gaussianDistribution(i, 0, deviation) 
				mBloomTexWeights[i][3] = 1.0 
				mBloomTexOffsetsHorz[i][0] = i * texelSize 
				mBloomTexOffsetsHorz[i][1] = 0.0 
				mBloomTexOffsetsVert[i][0] = 0.0 
				mBloomTexOffsetsVert[i][1] = i * texelSize 

			## 'post' samples
			for(i = 8  i < 15; ++i)

				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = 
					mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0] 
				mBloomTexWeights[i][3] = 1.0 

				mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0] 
				mBloomTexOffsetsHorz[i][1] = 0.0 
				mBloomTexOffsetsVert[i][0] = 0.0 
				mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1] 



		virtual (self): notifyMaterialSetup(Ogre.uint32 pass_id, Ogre::MaterialPtr &mat)

			## Prepare the fragment params offsets
			switch(pass_id)

			case 701: ## blur horz

					## horizontal bloom
					mat.load() 
					Ogre.GpuProgramParametersSharedPtr fparams = 
						mat.getBestTechnique().getPass(0).getFragmentProgramParameters() 
					 Ogre.String& progName = mat.getBestTechnique().getPass(0).getFragmentProgramName() 
					## A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
					if (progName.find("GLSL") != Ogre.String::npos):

						fparams.setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15) 
						fparams.setNamedConstant("sampleWeights", mBloomTexWeights[0], 15) 

					else:

						fparams.setNamedConstant("sampleOffsets[0]", mBloomTexOffsetsHorz[0], 15) 
						fparams.setNamedConstant("sampleWeights[0]", mBloomTexWeights[0], 15) 


					break 

			case 700: ## blur vert

					## vertical bloom 
					mat.load() 
					Ogre.GpuProgramParametersSharedPtr fparams = 
						mat.getTechnique(0).getPass(0).getFragmentProgramParameters() 
					 Ogre.String& progName = mat.getBestTechnique().getPass(0).getFragmentProgramName() 
					## A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
					if (progName.find("GLSL") != Ogre.String::npos):

						fparams.setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15) 
						fparams.setNamedConstant("sampleWeights", mBloomTexWeights[0], 15) 

					else:

						fparams.setNamedConstant("sampleOffsets[0]", mBloomTexOffsetsVert[0], 15) 
						fparams.setNamedConstant("sampleWeights[0]", mBloomTexWeights[0], 15) 


					break 




		virtual (self): notifyMaterialRender(Ogre.uint32 pass_id, Ogre::MaterialPtr &mat)




	GaussianListener gaussianListener 

def testCompositorTextureShadows(ShadowTechnique tech)

		self.sceneManager.setShadowTextureSize(512) 
		self.sceneManager.setShadowTechnique(tech) 
		self.sceneManager.setShadowFarDistance(1500) 
		self.sceneManager.setShadowColour(ColourValue(0.35, 0.35, 0.35)) 
		##self.sceneManager.setShadowFarDistance(800) 
		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.3, 0.3, 0.3)) 

		mLight = self.sceneManager.createLight("MainLight") 

		/*
		## Directional test
		mLight.setType(Light.LT_DIRECTIONAL) 
		Vector3 vec(-1,-1,0) 
		vec.normalise() 
		mLight.setDirection(vec) 

		*/
		## Spotlight test
		mLight.setType(Light.LT_SPOTLIGHT) 
		mLight.setDiffuseColour(1.0, 1.0, 0.8) 
		mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		mTestNode[0].setPosition(800,600,0) 
		mTestNode[0].lookAt(Vector3(0,0,0), Node.TS_WORLD, Vector3::UNIT_Z) 
		mTestNode[0].attachObject(mLight) 
		

		mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 


		Entity* pEnt 
		pEnt = self.sceneManager.createEntity( "1", "robot.mesh" ) 
		##pEnt.setRenderingDistance(100) 
		mAnimState = pEnt.getAnimationState("Walk") 
		mAnimState.setEnabled(True) 
		##pEnt.setMaterialName("2 - Default") 
		mTestNode[1].attachObject( pEnt ) 
		mTestNode[1].translate(0,-100,0) 

		pEnt = self.sceneManager.createEntity( "3", "knot.mesh" ) 
		mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-200, 0, -200)) 
		mTestNode[2].attachObject( pEnt ) 

		## Transparent object (can force cast shadows)
		pEnt = self.sceneManager.createEntity( "3.5", "knot.mesh" ) 
		MaterialPtr tmat = MaterialManager.getSingleton().create("TestAlphaTransparency", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		tmat.setTransparencyCastsShadows(True) 
		Pass* tpass = tmat.getTechnique(0).getPass(0) 
		tpass.setAlphaRejectSettings(CMPF_GREATER, 150) 
		tpass.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		tpass.createTextureUnitState("gras_02.png") 
		tpass.setCullingMode(CULL_NONE) 

		pEnt.setMaterialName("TestAlphaTransparency") 
		mTestNode[3] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(350, 0, -200)) 
		mTestNode[3].attachObject( pEnt ) 

		MeshPtr msh = MeshManager.getSingleton().load("knot.mesh",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		msh.buildTangentVectors(VES_TANGENT, 0, 0) 
		pEnt = self.sceneManager.createEntity( "4", "knot.mesh" ) 
		##pEnt.setMaterialName("Examples/BumpMapping/MultiLightSpecular") 
		mTestNode[2] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100, 0, 200)) 
		mTestNode[2].attachObject( pEnt ) 

		self.sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox") 


		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt 
		pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		## Set up a debug panel to display the shadow
		MaterialPtr debugMat = MaterialManager.getSingleton().create(
			"Ogre/DebugShadowMap", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		debugMat.getTechnique(0).getPass(0).setLightingEnabled(False) 
		TexturePtr shadowTex = self.sceneManager.getShadowTexture(0) 
		TextureUnitState *t = debugMat.getTechnique(0).getPass(0).createTextureUnitState(shadowTex.getName()) 
		t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
		##t = debugMat.getTechnique(0).getPass(0).createTextureUnitState("spot_shadow_fade.png") 
		##t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
		##t.setColourOperation(LBO_ADD) 

		OverlayContainer* debugPanel = (OverlayContainer*)
			(OverlayManager.getSingleton().createOverlayElement("Panel", "Ogre/DebugShadowPanel")) 
		debugPanel._setPosition(0.8, 0) 
		debugPanel._setDimensions(0.2, 0.3) 
		debugPanel.setMaterialName("Ogre/DebugShadowMap") 
		Overlay* debugOverlay = OverlayManager.getSingleton().getByName("Core/DebugOverlay") 
		debugOverlay.add2D(debugPanel) 



		ParticleSystem* pSys2 = self.sceneManager.createParticleSystem("smoke", 
			"Examples/Smoke") 
		mTestNode[4] = self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(-300, -100, 200)) 
		mTestNode[4].attachObject(pSys2) 

		RenderTarget* shadowRtt = shadowTex.getBuffer().getRenderTarget() 
		Viewport* vp = shadowRtt.getViewport(0) 
		Ogre.CompositorInstance *instance = 
			CompositorManager.getSingleton().addCompositor(vp, "Gaussian Blur") 
		CompositorManager.getSingleton().setCompositorEnabled(
			vp, "Gaussian Blur", True) 
		instance.addListener(&gaussianListener) 
		gaussianListener.notifyViewportSize(vp.getActualWidth(), vp.getActualHeight()) 







    (self): testOverlayZOrder((self):)

        Overlay* o = OverlayManager.getSingleton().getByName("Test/Overlay3") 
        o.show() 
        o = OverlayManager.getSingleton().getByName("Test/Overlay2") 
        o.show() 
        o = OverlayManager.getSingleton().getByName("Test/Overlay1") 
        o.show() 


    (self): createRandomEntityClones(Entity* ent, size_t cloneCount, 
         Vector3& min, const Vector3& max)

        Entity *cloneEnt 
        for (size_t n = 0  n < cloneCount; ++n)

            ## Create a new node under the root
            SceneNode* node = self.sceneManager.createSceneNode() 
            ## Random translate
            Vector3 nodePos 
            nodePos.x = Math.RangeRandom(min.x, max.x) 
            nodePos.y = Math.RangeRandom(min.y, max.y) 
            nodePos.z = Math.RangeRandom(min.z, max.z) 
            node.setPosition(nodePos) 
            self.sceneManager.getRootSceneNode().addChild(node) 
            cloneEnt = ent.clone(ent.getName() + "_clone" + StringConverter.toString(n)) 
            ## Attach to new node
            node.attachObject(cloneEnt) 




    (self): testIntersectionSceneQuery()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        ## Create a set of random balls
        Entity* ent = self.sceneManager.createEntity("Ball", "sphere.mesh") 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
        createRandomEntityClones(ent, 500, Vector3(-5000,-5000,-5000), Vector3(5000,5000,5000)) 

        intersectionQuery = self.sceneManager.createIntersectionQuery() 


    (self): testRaySceneQuery()

        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

        ## Create a polight
        Light* l = self.sceneManager.createLight("MainLight") 
        l.setType(Light.LT_DIRECTIONAL) 
        l.setDirection(-Vector3.UNIT_Y) 

        ## Create a set of random balls
        Entity* ent = self.sceneManager.createEntity("Ball", "sphere.mesh") 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
        createRandomEntityClones(ent, 100, Vector3(-1000,-1000,-1000), Vector3(1000,1000,1000)) 

        rayQuery = self.sceneManager.createRayQuery(
			mCamera.getCameraToViewportRay(0.5, 0.5)) 
        rayQuery.setSortByDistance(True, 1) 

        bool val = True 
        self.sceneManager.setOption("ShowOctree", &val) 

		##mCamera.setFarClipDistance(0) 



def testLotsAndLotsOfEntities()

		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

		## Create a polight
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(-Vector3.UNIT_Y) 

		## Create a set of random balls
		Entity* ent = self.sceneManager.createEntity("Ball", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		createRandomEntityClones(ent, 400, Vector3(-2000,-2000,-2000), Vector3(2000,2000,2000)) 

		##bool val = True 
		##self.sceneManager.setOption("ShowOctree", &val) 



def testSimpleMesh()

		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

		## Create a polight
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(1, -1, 0) 
		dir.normalise() 
		l.setDirection(dir) 

		## Create a set of random balls
		Entity* ent = self.sceneManager.createEntity("test", "xsicylinder.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 



def testGLSLTangent()


		Entity* ent = self.sceneManager.createEntity("test", "athene.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		ent.setMaterialName("testglsl") 


def testInfiniteAAB()

		## When using the BspSceneManager
		##self.sceneManager.setWorldGeometry("ogretestmap.bsp") 

		## When using the TerrainSceneManager
		self.sceneManager.setWorldGeometry("terrain.cfg") 

		AxisAlignedBox b1  ## null
		assert( b1.isNull() ) 
		
		AxisAlignedBox b2(Vector3.ZERO, 5.0 * Vector3::UNIT_SCALE)  ## finite
		assert( b2.isFinite() ) 

		AxisAlignedBox b3 
		b3.setInfinite() 
		assert( b3.isInfinite() ) 


			## Create background material
			MaterialPtr material = MaterialManager.getSingleton().create("Background", "General") 
			material.getTechnique(0).getPass(0).createTextureUnitState("rockwall.tga") 
			material.getTechnique(0).getPass(0).setDepthCheckEnabled(False) 
			material.getTechnique(0).getPass(0).setDepthWriteEnabled(False) 
			material.getTechnique(0).getPass(0).setLightingEnabled(False) 

			## Create left background rectangle
			## NOTE: Uses finite aab
			Rectangle2D* rect1 = new Rectangle2D(True) 
			rect1.setCorners(-0.5, 0.1, -0.1, -0.1) 
			## Hacky, set small bounding box, to show problem
			rect1.setBoundingBox(AxisAlignedBox(-10.0*Vector3.UNIT_SCALE, 10.0*Vector3::UNIT_SCALE)) 
			rect1.setMaterial("Background") 
			rect1.setRenderQueueGroup(RENDER_QUEUE_OVERLAY - 1) 
			SceneNode* node = self.sceneManager.getRootSceneNode().createChildSceneNode("Background1") 
			node.attachObject(rect1) 

			## Create right background rectangle
			## NOTE: Uses infinite aab
			Rectangle2D* rect2 = new Rectangle2D(True) 
			rect2.setCorners(0.1, 0.1, 0.5, -0.1) 
			AxisAlignedBox aabInf  aabInf.setInfinite();
			rect2.setBoundingBox(aabInf) 
			rect2.setMaterial("Background") 
			rect2.setRenderQueueGroup(RENDER_QUEUE_OVERLAY - 1) 
			node = self.sceneManager.getRootSceneNode().createChildSceneNode("Background2") 
			node.attachObject(rect2) 

			## Create a manual object for 2D
			ManualObject* manual = self.sceneManager.createManualObject("manual") 
			manual.setUseIdentityProjection(True) 
			manual.setUseIdentityView(True) 
			manual.begin("BaseWhiteNoLighting", RenderOperation.OT_LINE_STRIP) 
			manual.position(-0.2, -0.2, 0.0) 
			manual.position( 0.2, -0.2, 0.0) 
			manual.position( 0.2,  0.2, 0.0) 
			manual.position(-0.2,  0.2, 0.0) 
			manual.index(0) 
			manual.index(1) 
			manual.index(2) 
			manual.index(3) 
			manual.index(0) 
			manual.end() 
			manual.setBoundingBox(aabInf)  ## Use infinite aab to always stay visible
			rect2.setRenderQueueGroup(RENDER_QUEUE_OVERLAY - 1) 
			self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(manual) 


		self.sceneManager.showBoundingBoxes(True) 

		Entity* ent = self.sceneManager.createEntity("test", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(
			"test", 50.0 * Vector3.UNIT_X).attachObject(ent) 


def test2Windows((self):)

		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

		## Create a polight
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(1, -1, 0) 
		dir.normalise() 
		l.setDirection(dir) 

		## Create a set of random balls
		Entity* ent = self.sceneManager.createEntity("test", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 

		NameValuePairList valuePair 
		valuePair["top"] = StringConverter.toString(0) 
		valuePair["left"] = StringConverter.toString(0) 

		RenderWindow* win2 = self.root.createRenderWindow("window2", 200,200, False, &valuePair) 
		win2.addViewport(mCamera) 



def testStaticGeometry((self):)

		self.sceneManager.setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE) 
		##self.sceneManager.setShowDebugShadows(True) 

		self.sceneManager.setSkyBox(True, "Examples/EveningSkyBox") 
		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 

		## Create a polight
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(0, -1, -1.5) 
		dir.normalise() 
		l.setDirection(dir) 
		l.setDiffuseColour(1.0, 0.7, 0.0) 


		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 0 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			4500,4500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("Examples/GrassFloor") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		Vector3 min(-2000,0,-2000) 
		Vector3 max(2000,0,2000) 


		Entity* e = self.sceneManager.createEntity("1", "column.mesh") 
		##createRandomEntityClones(e, 1000, min, max) 
		
		StaticGeometry* s = self.sceneManager.createStaticGeometry("bing") 
		s.setCastShadows(True) 
		s.setRegionDimensions(Vector3(500,500,500)) 
		for (i = 0  i < 100; ++i)

			Vector3 pos 
			pos.x = Math.RangeRandom(min.x, max.x) 
			pos.y = Math.RangeRandom(min.y, max.y) 
			pos.z = Math.RangeRandom(min.z, max.z) 

			s.addEntity(e, pos, Quaternion.IDENTITY) 



		s.build() 
		##s.setRenderingDistance(1000) 
		##s.dump("static.txt") 
		##self.sceneManager.showBoundingBoxes(True) 
		mCamera.setLodBias(0.5) 
		





def testReloadResources()

		self.sceneManager.setAmbientLight(ColourValue.White) 
		ResourceGroupManager.getSingleton().createResourceGroup("Sinbad") 
		Root.getSingleton().addResourceLocation("../../../Media/models", "FileSystem", "Sinbad") 
		MeshManager& mmgr = MeshManager.getSingleton() 
		mmgr.load("robot.mesh", "Sinbad") 
		mmgr.load("knot.mesh", "Sinbad") 

		Entity* e = self.sceneManager.createEntity("1", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		e = self.sceneManager.createEntity("2", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100,0,0)).attachObject(e) 
		e = self.sceneManager.createEntity("3", "knot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100,300,0)).attachObject(e) 

		testreload = True 



def testBillboardTextureCoords()

		self.sceneManager.setAmbientLight(ColourValue.White) 

		BillboardSet* bbs = self.sceneManager.createBillboardSet("test") 
        BillboardSet* bbs2 = self.sceneManager.createBillboardSet("test2") 
		float xsegs = 3 
		float ysegs = 3 
		float width = 300 
		float height = 300 
		float gap = 20 

		## set up texture coords
		bbs.setTextureStacksAndSlices(ysegs, xsegs) 
		bbs.setDefaultDimensions(width/xsegs, height/xsegs) 
		bbs2.setDefaultDimensions(width/xsegs, height/xsegs) 

		for (float y = 0  y < ysegs; ++y)

			for (float x = 0  x < xsegs; ++x)

				Vector3 midPo
				midPoint.x = (x * width / xsegs) + ((x-1) * gap) 
				midPoint.y = (y * height / ysegs) + ((y-1) * gap) 
				midPoint.z = 0 
				Billboard* bb = bbs.createBillboard(midPoint) 
				bb.setTexcoordIndex((ysegs - y - 1)*xsegs + x) 
                Billboard* bb2 = bbs2.createBillboard(midPoint) 
                bb2.setTexcoordRect(
                    FloatRect((x + 0) / xsegs, (ysegs - y - 1) / ysegs,
                              (x + 1) / xsegs, (ysegs - y - 0) / ysegs)) 



		bbs.setMaterialName("Examples/OgreLogo") 
        bbs2.setMaterialName("Examples/OgreLogo") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(bbs) 
        self.sceneManager.getRootSceneNode()
            .createChildSceneNode(Vector3(- (width + xsegs * gap), 0, 0))
            .attachObject(bbs2) 



	class SortFunctor

	public:
		operator()( int& p) const

			return p 



def testRadixSort()

		RadixSort<std.list<int>, int, int> rs 
		SortFunctor f 

		std.list<int> particles 
		for (r = 0  r < 20; ++r)

			particles.push_back((int)Math.RangeRandom(-1e3, 1e3f)) 


		std.list<int>::iterator i 
		LogManager.getSingleton().logMessage("BEFORE") 
		for (i = particles.begin()  i != particles.end(); ++i)

			StringUtil.StrStreamType str 
			str << *i 
			LogManager.getSingleton().logMessage(str.str()) 


		rs.sort(particles, f) 


		LogManager.getSingleton().logMessage("AFTER") 
		for (i = particles.begin()  i != particles.end(); ++i)

			StringUtil.StrStreamType str 
			str << *i 
			LogManager.getSingleton().logMessage(str.str()) 






def testMorphAnimation()

		bool testStencil = True 

		if (testStencil):
			self.sceneManager.setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE) 

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		
		MeshPtr mesh = MeshManager.getSingleton().load("cube.mesh", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 

		SubMesh* sm = mesh.getSubMesh(0) 
		## Re-organise geometry since self mesh has no animation and all 
		## vertex elements are packed into one buffer
		VertexDeclaration* newDecl = 
			sm.vertexData.vertexDeclaration.getAutoOrganisedDeclaration(False, True) 
		sm.vertexData.reorganiseBuffers(newDecl) 
		if (testStencil):
			sm.vertexData.prepareForShadowVolume()  ## need to re-prep since reorganised
		## get the position buffer (which should now be separate) 
		 VertexElement* posElem = 
			sm.vertexData.vertexDeclaration.findElementBySemantic(VES_POSITION) 
		HardwareVertexBufferSharedPtr origbuf = 
			sm.vertexData.vertexBufferBinding.getBuffer(
				posElem.getSource()) 

		## Create a new position buffer with updated values
		HardwareVertexBufferSharedPtr newbuf = 
			HardwareBufferManager.getSingleton().createVertexBuffer(
				VertexElement.getTypeSize(VET_FLOAT3),
				sm.vertexData.vertexCount, 
				HardwareBuffer.HBU_STATIC, True) 
		float* pSrc = static_cast<float*>(origbuf.lock(HardwareBuffer.HBL_READ_ONLY)) 
		float* pDst = static_cast<float*>(newbuf.lock(HardwareBuffer.HBL_DISCARD)) 

		for (size_t v = 0  v < sm.vertexData.vertexCount; ++v)

			## x
			*pDst++ = *pSrc++ 
			## y (translate)
			*pDst++ = (*pSrc++) + 100.0 
			## z
			*pDst++ = *pSrc++ 


		origbuf.unlock() 
		newbuf.unlock() 
		
		## create a morph animation
		Animation* anim = mesh.createAnimation("testAnim", 10.0) 
		VertexAnimationTrack* vt = anim.createVertexTrack(1, sm.vertexData, VAT_MORPH) 
		## re-use start positions for frame 0
		VertexMorphKeyFrame* kf = vt.createVertexMorphKeyFrame(0) 
		kf.setVertexBuffer(origbuf) 

		## Use translated buffer for mid frame
		kf = vt.createVertexMorphKeyFrame(5.0) 
		kf.setVertexBuffer(newbuf) 

		## re-use start positions for final frame
		kf = vt.createVertexMorphKeyFrame(10.0) 
		kf.setVertexBuffer(origbuf) 

		## write
		MeshSerializer ser 
		ser.exportMesh(mesh.get(), "../../../Media/testmorph.mesh") 
		

		Entity* e = self.sceneManager.createEntity("test", "testmorph.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		AnimationState* animState = e.getAnimationState("testAnim") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 

		e = self.sceneManager.createEntity("test2", "testmorph.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(100,0,0)).attachObject(e) 
		## test hardware morph
		e.setMaterialName("Examples/HardwareMorphAnimation") 
		animState = e.getAnimationState("testAnim") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 

		mCamera.setNearClipDistance(0.5) 
		##self.sceneManager.setShowDebugShadows(True) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 200 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 






def testPoseAnimation()

		bool testStencil = False 

		if (testStencil):
			self.sceneManager.setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE) 

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		MeshPtr mesh = MeshManager.getSingleton().load("cube.mesh", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 

		SubMesh* sm = mesh.getSubMesh(0) 
		## Re-organise geometry since self mesh has no animation and all 
		## vertex elements are packed into one buffer
		VertexDeclaration* newDecl = 
			sm.vertexData.vertexDeclaration.getAutoOrganisedDeclaration(False, True) 
		sm.vertexData.reorganiseBuffers(newDecl) 
		if (testStencil):
			sm.vertexData.prepareForShadowVolume()  ## need to re-prep since reorganised
		## get the position buffer (which should now be separate) 
		 VertexElement* posElem = 
			sm.vertexData.vertexDeclaration.findElementBySemantic(VES_POSITION) 
		HardwareVertexBufferSharedPtr origbuf = 
			sm.vertexData.vertexBufferBinding.getBuffer(
			posElem.getSource()) 


		## create 2 poses
		Pose* pose = mesh.createPose(1, "pose1") 
		## Pose1 moves vertices 0, 1, 2 and 3 upward
		Vector3 offset1(0, 50, 0) 
		pose.addVertex(0, offset1) 
		pose.addVertex(1, offset1) 
		pose.addVertex(2, offset1) 
		pose.addVertex(3, offset1) 

		pose = mesh.createPose(1, "pose2") 
		## Pose2 moves vertices 3, 4, and 5 to the right
		## Note 3 gets affected by both
		Vector3 offset2(100, 0, 0) 
		pose.addVertex(3, offset2) 
		pose.addVertex(4, offset2) 
		pose.addVertex(5, offset2) 


		Animation* anim = mesh.createAnimation("poseanim", 20.0) 
		VertexAnimationTrack* vt = anim.createVertexTrack(1, sm.vertexData, VAT_POSE) 
		
		## Frame 0 - no effect 
		VertexPoseKeyFrame* kf = vt.createVertexPoseKeyFrame(0) 

		## Frame 1 - bring in pose 1 (index 0)
		kf = vt.createVertexPoseKeyFrame(3) 
		kf.addPoseReference(0, 1.0) 

		## Frame 2 - remove all 
		kf = vt.createVertexPoseKeyFrame(6) 

		## Frame 3 - bring in pose 2 (index 1)
		kf = vt.createVertexPoseKeyFrame(9) 
		kf.addPoseReference(1, 1.0) 

		## Frame 4 - remove all
		kf = vt.createVertexPoseKeyFrame(12) 


		## Frame 5 - bring in pose 1 at 50%, pose 2 at 100% 
		kf = vt.createVertexPoseKeyFrame(15) 
		kf.addPoseReference(0, 0.5) 
		kf.addPoseReference(1, 1.0) 

		## Frame 6 - bring in pose 1 at 100%, pose 2 at 50% 
		kf = vt.createVertexPoseKeyFrame(18) 
		kf.addPoseReference(0, 1.0) 
		kf.addPoseReference(1, 0.5) 

		## Frame 7 - reset
		kf = vt.createVertexPoseKeyFrame(20) 

		## write
		MeshSerializer ser 
		ser.exportMesh(mesh.get(), "../../../Media/testpose.mesh") 



		## software pose
		Entity* e = self.sceneManager.createEntity("test2", "testpose.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(150,0,0)).attachObject(e) 
		AnimationState* animState = e.getAnimationState("poseanim") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 

		
		## test hardware pose
		e = self.sceneManager.createEntity("test", "testpose.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		e.setMaterialName("Examples/HardwarePoseAnimation") 
		animState = e.getAnimationState("poseanim") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 
		

		mCamera.setNearClipDistance(0.5) 
		self.sceneManager.setShowDebugShadows(True) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 200 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		mCamera.setPosition(0,0,-300) 
		mCamera.lookAt(0,0,0) 





def testPoseAnimation2()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, -0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		/*
		MeshPtr mesh = MeshManager.getSingleton().load("facial.mesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) 
		Animation* anim = mesh.createAnimation("pose", 20) 
		VertexAnimationTrack* track = anim.createVertexTrack(4, VAT_POSE) 
		## Frame 0 - no effect 
		VertexPoseKeyFrame* kf = track.createVertexPoseKeyFrame(0) 

		## bring in pose 4 (happy)
		kf = track.createVertexPoseKeyFrame(5) 
		kf.addPoseReference(4, 1.0) 

		## bring in pose 5 (sad)
		kf = track.createVertexPoseKeyFrame(10) 
		kf.addPoseReference(5, 1.0) 

		## bring in pose 6 (mad)
		kf = track.createVertexPoseKeyFrame(15) 
		kf.addPoseReference(6, 1.0) 
		
		kf = track.createVertexPoseKeyFrame(20) 
		*/

		## software pose
		Entity* e = self.sceneManager.createEntity("test2", "facial.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(150,0,0)).attachObject(e) 
		AnimationState* animState = e.getAnimationState("Speak") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 


		/*
		## test hardware pose
		e = self.sceneManager.createEntity("test", "testpose.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		e.setMaterialName("Examples/HardwarePoseAnimation") 
		animState = e.getAnimationState("poseanim") 
		animState.setEnabled(True) 
		animState.setWeight(1.0) 
		mAnimStateList.push_back(animState) 
		*/


		mCamera.setNearClipDistance(0.5) 
		self.sceneManager.setShowDebugShadows(True) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 200 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 




def testReflectedBillboards()

		## Set ambient light
		self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 

		## Create a light
		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(0.5, -1, 0) 
		dir.normalise() 
		l.setDirection(dir) 
		l.setDiffuseColour(1.0, 1.0f, 0.8f) 
		l.setSpecularColour(1.0, 1.0f, 1.0f) 


		## Create a prefab plane
		Plane plane 
		plane.d = 0 
		plane.normal = Vector3.UNIT_Y 
		MeshManager.getSingleton().createPlane("ReflectionPlane", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			plane, 2000, 2000, 
			1, 1, True, 1, 1, 1, Vector3.UNIT_Z) 
		Entity* planeEnt = self.sceneManager.createEntity( "Plane", "ReflectionPlane" ) 

		## Attach the rtt entity to the root of the scene
		SceneNode* rootNode = self.sceneManager.getRootSceneNode() 
		SceneNode* planeNode = rootNode.createChildSceneNode() 

		## Attach both the plane entity, and the plane definition
		planeNode.attachObject(planeEnt) 

		RenderTexture* rttTex = self.root.getRenderSystem().createRenderTexture( "RttTex", 512, 512, TEX_TYPE_2D, PF_R8G8B8 ) 

			reflectCam = self.sceneManager.createCamera("ReflectCam") 
			reflectCam.setNearClipDistance(mCamera.getNearClipDistance()) 
			reflectCam.setFarClipDistance(mCamera.getFarClipDistance()) 
			reflectCam.setAspectRatio(
				(Real)mWindow.getViewport(0).getActualWidth() / 
				(Real)mWindow.getViewport(0).getActualHeight()) 

			Viewport *v = rttTex.addViewport( reflectCam ) 
			v.setClearEveryFrame( True ) 
			v.setBackgroundColour( ColourValue.Black ) 

			MaterialPtr mat = MaterialManager.getSingleton().create("RttMat",
				ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
			TextureUnitState* t = mat.getTechnique(0).getPass(0).createTextureUnitState("RustedMetal.jpg") 
			t = mat.getTechnique(0).getPass(0).createTextureUnitState("RttTex") 
			## Blend with base texture
			t.setColourOperationEx(LBX_BLEND_MANUAL, LBS_TEXTURE, LBS_CURRENT, ColourValue.White, 
				ColourValue.White, 0.25) 
			t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
			t.setProjectiveTexturing(True, reflectCam) 

			## set up linked reflection
			reflectCam.enableReflection(plane) 
			## Also clip
			reflectCam.enableCustomNearClipPlane(plane) 


		## Give the plane a texture
		planeEnt.setMaterialName("RttMat") 


		## pobillboards
		ParticleSystem* pSys2 = self.sceneManager.createParticleSystem("fountain1", 
			"Examples/Smoke") 
		## Pothe fountain at an angle
		SceneNode* fNode = static_cast<SceneNode*>(rootNode.createChild()) 
		fNode.attachObject(pSys2) 

		## oriented_self billboards
		ParticleSystem* pSys3 = self.sceneManager.createParticleSystem("fountain2", 
			"Examples/PurpleFountain") 
		## Pothe fountain at an angle
		fNode = rootNode.createChildSceneNode() 
		fNode.translate(-200,-100,0) 
		fNode.rotate(Vector3.UNIT_Z, Degree(-20)) 
		fNode.attachObject(pSys3) 



		## oriented_common billboards
		ParticleSystem* pSys4 = self.sceneManager.createParticleSystem("rain", 
			"Examples/Rain") 
		SceneNode* rNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		rNode.translate(0,1000,0) 
		rNode.attachObject(pSys4) 
		## Fast-forward the rain so it looks more natural
		pSys4.fastForward(5) 


		mCamera.setPosition(-50, 100, 500) 
		mCamera.lookAt(0,0,0) 


def testManualObjectNonIndexed()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.createMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 

		man.begin("Examples/OgreLogo") 
		## Define a 40x40 plane, non-indexed
		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.end() 

		man.begin("Examples/BumpyMetal") 

		## Define a 40x40 plane, non-indexed
		man.position(-20, 20, 20) 
		man.normal(0, 1, 0) 
		man.textureCoord(0, 0) 

		man.position(20, 20, 20) 
		man.normal(0, 1, 0) 
		man.textureCoord(0, 1) 

		man.position(20, 20, -20) 
		man.normal(0, 1, 0) 
		man.textureCoord(1, 1) 

		man.position(20, 20, -20) 
		man.normal(0, 1, 0) 
		man.textureCoord(1, 1) 

		man.position(-20, 20, -20) 
		man.normal(0, 1, 0) 
		man.textureCoord(1, 0) 

		man.position(-20, 20, 20) 
		man.normal(0, 1, 0) 
		man.textureCoord(0, 0) 

		man.end() 


		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man) 



def testManualObjectNonIndexedUpdateSmaller()

		testManualObjectNonIndexed() 
		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.getMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 

		## Redefine but make smaller (one tri less)
		man.beginUpdate(0) 
		man.position(-30, 30, 30) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-30, -30, 30) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(30, 30, 30) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 


		man.end() 



def testManualObjectNonIndexedUpdateLarger()

		testManualObjectNonIndexed() 
		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.getMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 

		## Redefine but make larger (2 more tri)
		man.beginUpdate(0) 

		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 


		man.position(-20, 40, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, 40, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 40, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 


		man.end() 


def testManualObjectIndexed()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.createMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 

		man.begin("Examples/OgreLogo") 
		## Define a 40x40 plane, indexed
		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.quad(0, 1, 2, 3) 

		man.end() 

		man.begin("Examples/BumpyMetal") 

		## Define a 40x40 plane, indexed
		man.position(-20, 20, 20) 
		man.normal(0, 1, 0) 
		man.textureCoord(0, 0) 

		man.position(20, 20, 20) 
		man.normal(0, 1, 0) 
		man.textureCoord(0, 1) 

		man.position(20, 20, -20) 
		man.normal(0, 1, 0) 
		man.textureCoord(1, 1) 

		man.position(-20, 20, -20) 
		man.normal(0, 1, 0) 
		man.textureCoord(1, 0) 

		man.quad(0, 1, 2, 3) 

		man.end() 


		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man) 



def testManualObjectIndexedUpdateSmaller()

		testManualObjectIndexed() 
		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.getMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 


		man.beginUpdate(0) 
		## 1 tri smaller
		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.triangle(0, 1, 2) 

		man.end() 



def testManualObjectIndexedUpdateLarger()

		testManualObjectIndexed() 
		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.getMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 


		man.beginUpdate(0) 
		## 1 quad larger
		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, -20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 


		man.position(-20, 40, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 0) 

		man.position(-20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(0, 1) 

		man.position(20, 20, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 1) 

		man.position(20, 40, 20) 
		man.normal(0, 0, 1) 
		man.textureCoord(1, 0) 

		man.quad(0, 1, 2, 3) 
		man.quad(4, 5, 6, 7) 

		man.end() 



def testBillboardChain()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		BillboardChain* chain = static_cast<BillboardChain*>(
			self.sceneManager.createMovableObject("1", "BillboardChain")) 
		chain.setUseTextureCoords(True) 
		chain.setUseVertexColours(False) 
		/*
		BillboardChain.Element elem 
		elem.width = 10 
		elem.texCoord = 0 
		elem.position = Vector3(0,20,0) 
		chain.addChainElement(0, elem) 
		elem.position = Vector3(20,0,0) 
		elem.texCoord = 1.0 
		chain.addChainElement(0, elem) 
		elem.position = Vector3(40,10,0) 
		elem.texCoord = 2.0 
		chain.addChainElement(0, elem) 
		elem.position = Vector3(60,20,0) 
		elem.texCoord = 3.0 
		chain.addChainElement(0, elem) 
		elem.position = Vector3(80,40,0) 
		elem.texCoord = 4.0 
		chain.addChainElement(0, elem) 
		elem.position = Vector3(100,70,0) 
		elem.texCoord = 5.0 
		chain.addChainElement(0, elem) 
		*/
		chain.setMaterialName("Examples/RustySteel") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(chain) 

		self.sceneManager.showBoundingBoxes(True) 


def testCubeDDS()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testcube", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		TextureUnitState* t = p.createTextureUnitState() 
		t.setTextureName("grace_cube.dds", TEX_TYPE_CUBE_MAP) 
		t.setTextureAddressingMode(TextureUnitState.TAM_CLAMP) 
		t.setEnvironmentMap(True, TextureUnitState.ENV_REFLECTION) 
		Entity* e = self.sceneManager.createEntity("1", "sphere.mesh") 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 

		mCamera.setPosition(300,0,0) 
		mCamera.lookAt(Vector3.ZERO) 



def testDxt1()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdxt", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		TextureUnitState* t = p.createTextureUnitState("BumpyMetal_dxt1.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 

		mCamera.setPosition(0,0,-300) 
		mCamera.lookAt(Vector3.ZERO) 


def testDxt1Alpha()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdxt", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		p.setAlphaRejectSettings(CMPF_GREATER, 128) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		TextureUnitState* t = p.createTextureUnitState("gras_02_dxt1.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testDxt3()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdxt", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		p.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		TextureUnitState* t = p.createTextureUnitState("ogreborderUp_dxt3.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testDxt5()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdxt", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		p.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		TextureUnitState* t = p.createTextureUnitState("ogreborderUp_dxt5.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testFloat64DDS()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdds", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		p.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		TextureUnitState* t = p.createTextureUnitState("ogreborderUp_float64.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testFloat128DDS()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdds", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		p.setSceneBlending(SBT_TRANSPARENT_ALPHA) 
		TextureUnitState* t = p.createTextureUnitState("ogreborderUp_float128.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testFloat16DDS()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdds", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		TextureUnitState* t = p.createTextureUnitState("BumpyMetal_float16.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 


def testFloat32DDS()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		MaterialPtr mat = MaterialManager.getSingleton().create("testdds", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setLightingEnabled(False) 
		p.setCullingMode(CULL_NONE) 
		TextureUnitState* t = p.createTextureUnitState("BumpyMetal_float32.dds") 
		Entity *e = self.sceneManager.createEntity("Plane", SceneManager.PT_PLANE) 
		e.setMaterialName(mat.getName()) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(e) 
		mWindow.getViewport(0).setBackgroundColour(ColourValue.Red) 

		mCamera.setPosition(0,0,300) 
		mCamera.lookAt(Vector3.ZERO) 





def testRibbonTrail()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		NameValuePairList pairList 
		pairList["numberOfChains"] = "2" 
		pairList["maxElements"] = "80" 
		RibbonTrail* trail = static_cast<RibbonTrail*>(
			self.sceneManager.createMovableObject("1", "RibbonTrail", &pairList)) 
		trail.setMaterialName("Examples/LightRibbonTrail") 
		trail.setTrailLength(400) 


		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(trail) 

		## Create 3 nodes for trail to follow
		SceneNode* animNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		animNode.setPosition(0,20,0) 
		Animation* anim = self.sceneManager.createAnimation("an1", 10) 
		anim.setInterpolationMode(Animation.IM_SPLINE) 
		NodeAnimationTrack* track = anim.createNodeTrack(1, animNode) 
		TransformKeyFrame* kf = track.createNodeKeyFrame(0) 
		kf.setTranslate(Vector3.ZERO) 
		kf = track.createNodeKeyFrame(2) 
		kf.setTranslate(Vector3(100, 0, 0)) 
		kf = track.createNodeKeyFrame(4) 
		kf.setTranslate(Vector3(200, 0, 300)) 
		kf = track.createNodeKeyFrame(6) 
		kf.setTranslate(Vector3(0, 20, 500)) 
		kf = track.createNodeKeyFrame(8) 
		kf.setTranslate(Vector3(-100, 10, 100)) 
		kf = track.createNodeKeyFrame(10) 
		kf.setTranslate(Vector3.ZERO) 

		AnimationState* animState = self.sceneManager.createAnimationState("an1") 
		animState.setEnabled(True) 
		mAnimStateList.push_back(animState) 

		trail.addNode(animNode) 
		trail.setInitialColour(0, 1.0, 0.8, 0) 
		trail.setColourChange(0, 0.5, 0.5, 0.5, 0.5) 
		trail.setInitialWidth(0, 5) 

		## Add light
		Light* l2 = self.sceneManager.createLight("l2") 
		l2.setDiffuseColour(trail.getInitialColour(0)) 
		animNode.attachObject(l2) 

		## Add billboard
		BillboardSet* bbs = self.sceneManager.createBillboardSet("bb", 1) 
		bbs.createBillboard(Vector3.ZERO, trail.getInitialColour(0)) 
		bbs.setMaterialName("Examples/Flare") 
		animNode.attachObject(bbs) 

		animNode = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		animNode.setPosition(-50,10,0) 
		anim = self.sceneManager.createAnimation("an2", 10) 
		anim.setInterpolationMode(Animation.IM_SPLINE) 
		track = anim.createNodeTrack(1, animNode) 
		kf = track.createNodeKeyFrame(0) 
		kf.setTranslate(Vector3.ZERO) 
		kf = track.createNodeKeyFrame(2) 
		kf.setTranslate(Vector3(-100, 150, -30)) 
		kf = track.createNodeKeyFrame(4) 
		kf.setTranslate(Vector3(-200, 0, 40)) 
		kf = track.createNodeKeyFrame(6) 
		kf.setTranslate(Vector3(0, -150, 70)) 
		kf = track.createNodeKeyFrame(8) 
		kf.setTranslate(Vector3(50, 0, 30)) 
		kf = track.createNodeKeyFrame(10) 
		kf.setTranslate(Vector3.ZERO) 

		animState = self.sceneManager.createAnimationState("an2") 
		animState.setEnabled(True) 
		mAnimStateList.push_back(animState) 

		trail.addNode(animNode) 
		trail.setInitialColour(1, 0.0, 1.0, 0.4) 
		trail.setColourChange(1, 0.5, 0.5, 0.5, 0.5) 
		trail.setInitialWidth(1, 5) 


		## Add light
		l2 = self.sceneManager.createLight("l3") 
		l2.setDiffuseColour(trail.getInitialColour(1)) 
		animNode.attachObject(l2) 

		## Add billboard
		bbs = self.sceneManager.createBillboardSet("bb2", 1) 
		bbs.createBillboard(Vector3.ZERO, trail.getInitialColour(1)) 
		bbs.setMaterialName("Examples/Flare") 
		animNode.attachObject(bbs) 



		##self.sceneManager.showBoundingBoxes(True) 



def testBlendDiffuseColour()

		MaterialPtr mat = MaterialManager.getSingleton().create(
			"testBlendDiffuseColour", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* pass = mat.getTechnique(0).getPass(0) 
		## no lighting, it will mess up vertex colours
		pass.setLightingEnabled(False) 
		## Make sure we pull in vertex colour as diffuse
		pass.setVertexColourTracking(TVC_DIFFUSE) 
		## Base layer
		TextureUnitState* t = pass.createTextureUnitState("BeachStones.jpg") 
		## don't want to bring in vertex diffuse on base layer
		t.setColourOperation(LBO_REPLACE)  
		## Second layer (lerp based on colour)
		t = pass.createTextureUnitState("terr_dirt-grass.jpg") 
		t.setColourOperationEx(LBX_BLEND_DIFFUSE_COLOUR) 
		## third layer (lerp based on alpha)
		ManualObject* man = self.sceneManager.createManualObject("quad") 
		man.begin("testBlendDiffuseColour") 
		man.position(-100, 100, 0) 
		man.textureCoord(0,0) 
		man.colour(0, 0, 0) 
		man.position(-100, -100, 0) 
		man.textureCoord(0,1) 
		man.colour(0.5, 0.5, 0.5) 
		man.position(100, -100, 0) 
		man.textureCoord(1,1) 
		man.colour(1, 1, 1) 
		man.position(100, 100, 0) 
		man.textureCoord(1,0) 
		man.colour(0.5, 0.5, 0.5) 
		man.quad(0, 1, 2, 3) 
		man.end() 

		self.sceneManager.getRootSceneNode().attachObject(man) 



def testSplitPassesTooManyTexUnits()

		MaterialPtr mat = MaterialManager.getSingleton().create(
			"Test", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 

		Pass* p = mat.getTechnique(0).getPass(0) 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 
		p.createTextureUnitState("gras_02.png") 

		mat.compile() 



def testCustomProjectionMatrix()

		testLotsAndLotsOfEntities() 
		Matrix4 mat = mCamera.getProjectionMatrix() 
		mCamera.setCustomProjectionMatrix(True, mat) 
		mat = mCamera.getProjectionMatrix() 



def testPointSprites()

		MaterialPtr mat = MaterialManager.getSingleton().create("spriteTest1", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.setPointSpritesEnabled(True) 
		p.createTextureUnitState("flare.png") 
		p.setLightingEnabled(False) 
		p.setDepthWriteEnabled(False) 
		p.setSceneBlending(SBT_ADD) 
		p.setPointAttenuation(True) 
		p.setPointSize(1) 
		srand((unsigned)time( None ) ) 

		ManualObject* man = self.sceneManager.createManualObject("man") 
		man.begin("spriteTest1", RenderOperation.OT_POINT_LIST) 
		/*
		for (size_t i = 0  i < 1000; ++i)

			man.position(Math.SymmetricRandom() * 500, 
				Math.SymmetricRandom() * 500, 
				Math.SymmetricRandom() * 500) 
			man.colour(Math.RangeRandom(0.5, 1.0f), 
				Math.RangeRandom(0.5, 1.0f), Math::RangeRandom(0.5f, 1.0f)) 

		*/
		for (size_t i = 0  i < 20; ++i)

			for (size_t j = 0  j < 20; ++j)

				for (size_t k = 0  k < 20; ++k)

					man.position(i * 30, j * 30, k * 30) 




		man.end() 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man) 



def testSuppressedShadows(ShadowTechnique shadowTech)

		self.sceneManager.setShadowTechnique(shadowTech) 

		## Setup lighting
		self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 
		Light* light = self.sceneManager.createLight("MainLight") 
		light.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		light.setDirection(dir) 

		## Create a skydome
		##self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

		## Create a floor plane mesh
		Plane plane(Vector3.UNIT_Y, 0.0) 
		MeshManager.getSingleton().createPlane(
			"FloorPlane", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			plane, 200000, 200000, 20, 20, True, 1, 500, 500, Vector3.UNIT_Z) 
	

		## Add a floor to the scene
		Entity* entity = self.sceneManager.createEntity("floor", "FloorPlane") 
		entity.setMaterialName("Examples/RustySteel") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity) 
		entity.setCastShadows(False) 

		## Add the mandatory ogre head
		entity = self.sceneManager.createEntity("head", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(0.0, 10.0, 0.0)).attachObject(entity) 

		## Position and orient the camera
		mCamera.setPosition(-100.0, 50.0, 90.0) 
		mCamera.lookAt(0.0, 10.0, -35.0) 

		## Add an additional viewport on top of the other one
		Viewport* pip = mWindow.addViewport(mCamera, 1, 0.7, 0.0, 0.3, 0.3) 

		## Create a render queue invocation sequence for the pip viewport
		RenderQueueInvocationSequence* invocationSequence =
			self.root.createRenderQueueInvocationSequence("pip") 

		## Add an invocation to the sequence
		RenderQueueInvocation* invocation =
			invocationSequence.add(RENDER_QUEUE_MAIN, "main") 

		## Disable render state changes and shadows for that invocation
		##invocation.setSuppressRenderStateChanges(True) 
		invocation.setSuppressShadows(True) 

		## Set the render queue invocation sequence for the pip viewport
		pip.setRenderQueueInvocationSequenceName("pip") 


def testViewportNoShadows(ShadowTechnique shadowTech)

		self.sceneManager.setShadowTechnique(shadowTech) 

		## Setup lighting
		self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 
		Light* light = self.sceneManager.createLight("MainLight") 
		light.setType(Light.LT_DIRECTIONAL) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		light.setDirection(dir) 

		## Create a skydome
		##self.sceneManager.setSkyDome(True, "Examples/CloudySky", 5, 8) 

		## Create a floor plane mesh
		Plane plane(Vector3.UNIT_Y, 0.0) 
		MeshManager.getSingleton().createPlane(
			"FloorPlane", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
			plane, 200000, 200000, 20, 20, True, 1, 500, 500, Vector3.UNIT_Z) 


		## Add a floor to the scene
		Entity* entity = self.sceneManager.createEntity("floor", "FloorPlane") 
		entity.setMaterialName("Examples/RustySteel") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity) 
		entity.setCastShadows(False) 

		## Add the mandatory ogre head
		entity = self.sceneManager.createEntity("head", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(0.0, 10.0, 0.0)).attachObject(entity) 

		## Position and orient the camera
		mCamera.setPosition(-100.0, 50.0, 90.0) 
		mCamera.lookAt(0.0, 10.0, -35.0) 

		## Add an additional viewport on top of the other one
		Viewport* pip = mWindow.addViewport(mCamera, 1, 0.7, 0.0, 0.3, 0.3) 
		pip.setShadowsEnabled(False) 



def testSerialisedColour()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		/*
		ManualObject* man = static_cast<ManualObject*>(
			self.sceneManager.createMovableObject("test", ManualObjectFactory.FACTORY_TYPE_NAME)) 

		man.begin("BaseWhiteNoLighting") 
		## Define a 40x40 plane, non-indexed
		## Define a 40x40 plane, indexed
		man.position(-20, 20, 20) 
		man.colour(1, 0, 0) 

		man.position(-20, -20, 20) 
		man.colour(1, 0, 0) 

		man.position(20, -20, 20) 
		man.colour(1, 0, 0) 

		man.position(20, 20, 20) 
		man.colour(1, 0, 0) 

		man.quad(0, 1, 2, 3) 
		man.end() 

		MeshPtr mesh = man.convertToMesh("colourtest.mesh") 
		MeshSerializer ms 
		ms.exportMesh(mesh.getPointer(), "colourtest.mesh") 

		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man) 
		*/
		Entity* c = self.sceneManager.createEntity("1", "colourtest.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(c) 





def testBillboardAccurateFacing()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		BillboardSet* bbs = self.sceneManager.createBillboardSet("1") 
		bbs.setDefaultDimensions(50,50) 
		bbs.createBillboard(-150, 25, 0) 
		bbs.setBillboardType(BBT_ORIENTED_COMMON) 
		bbs.setCommonDirection(Vector3.UNIT_Y) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(bbs) 

		bbs = self.sceneManager.createBillboardSet("2") 
		bbs.setDefaultDimensions(50,50) 
		bbs.createBillboard(150, 25, 0) 
		bbs.setUseAccurateFacing(True) 
		bbs.setBillboardType(BBT_ORIENTED_COMMON) 
		bbs.setCommonDirection(Vector3.UNIT_Y) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(bbs) 


def testMultiSceneManagersSimple()

		## Create a secondary scene manager with it's own camera
		SceneManager* sm2 = Root.getSingleton().createSceneManager(ST_GENERIC) 
		camera2 = sm2.createCamera("cam2") 
		camera2.setPosition(0,0,-500) 
		camera2.lookAt(Vector3.ZERO) 
		Entity* ent = sm2.createEntity("knot2", "knot.mesh") 
		sm2.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		Light* l = sm2.createLight("l2") 
		l.setPosition(100,50,-100) 
		l.setDiffuseColour(ColourValue.Green) 
		sm2.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 

		Viewport* vp = mWindow.addViewport(camera2, 1, 0.67, 0, 0.33, 0.25) 
		vp.setOverlaysEnabled(False) 

		## Use original SM for normal scene
		ent = self.sceneManager.createEntity("head", "ogrehead.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		l = self.sceneManager.createLight("l2")  ## note same name, will work since different SM
		l.setPosition(100,50,-100) 
		l.setDiffuseColour(ColourValue.Red) 
		self.sceneManager.setAmbientLight(ColourValue(0.2, 0.2, 0.2)) 




def testMultiSceneManagersComplex()

		## Create a secondary scene manager with it's own camera
		SceneManager* sm2 = Root.getSingleton().createSceneManager("TerrainSceneManager") 
		camera2 = sm2.createCamera("cam2") 

		Viewport* vp = mWindow.addViewport(camera2, 1, 0.5, 0, 0.5, 0.5) 
		vp.setOverlaysEnabled(False) 
		## Fog
		## NB it's VERY important to set self before calling setWorldGeometry 
		## because the vertex program picked will be different
		ColourValue fadeColour(0.93, 0.86, 0.76) 
		sm2.setFog( FOG_LINEAR, fadeColour, .001, 500, 1000) 
		vp.setBackgroundColour(fadeColour) 

		sm2.setWorldGeometry("terrain.cfg") 
		## Infinite far plane?
		if (self.root.getRenderSystem().getCapabilities().hasCapability(RSC_INFINITE_FAR_PLANE)):

			camera2.setFarClipDistance(0) 

		## Set a nice viewpoint
		camera2.setPosition(707,3500,528) 
		camera2.lookAt(Vector3.ZERO) 
		camera2.setNearClipDistance( 1 ) 
		camera2.setFarClipDistance( 1000 ) 


		## Create a tertiary scene manager with it's own camera
		SceneManager* sm3 = Root.getSingleton().createSceneManager("BspSceneManager") 
		Camera* camera3 = sm3.createCamera("cam3") 

		vp = mWindow.addViewport(camera3, 2, 0.5, 0.5, 0.5, 0.5) 
		vp.setOverlaysEnabled(False) 

		## Load Quake3 locations from a file
		ConfigFile cf 

		cf.load("quake3settings.cfg") 

		String pk3 = cf.getSetting("Pak0Location") 
		String level = cf.getSetting("Map") 

		ExampleApplication.setupResources() 
		ResourceGroupManager.getSingleton().createResourceGroup("BSP") 
		ResourceGroupManager.getSingleton().setWorldResourceGroupName("BSP") 
		ResourceGroupManager.getSingleton().addResourceLocation(
			pk3, "Zip", ResourceGroupManager.getSingleton().getWorldResourceGroupName()) 
		ResourceGroupManager.getSingleton().initialiseResourceGroup("BSP") 
		sm3.setWorldGeometry(level) 
		## modify camera for close work
		camera3.setNearClipDistance(4) 
		camera3.setFarClipDistance(4000) 

		## Also change position, and set Quake-type orientation
		## Get random player start point
		ViewPoviewp = sm3.getSuggestedViewpoint(True) 
		camera3.setPosition(viewp.position) 
		camera3.pitch(Degree(90))  ## Quake uses X/Y horizon, Z up
		camera3.rotate(viewp.orientation) 
		## Don't yaw along variable axis, causes leaning
		camera3.setFixedYawAxis(True, Vector3.UNIT_Z) 
		camera3.yaw(Degree(-90)) 





		## Use original SM for normal scene
		testTextureShadows(SHADOWTYPE_TEXTURE_MODULATIVE) 



def testManualBoneMovement((self):)

		Entity *ent = self.sceneManager.createEntity("robot", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		self.sceneManager.setAmbientLight(ColourValue(0.8, 0.8, 0.8)) 

		##ent.setMaterialName("Examples/Rocky") 

		SkeletonInstance* skel = ent.getSkeleton() 
		Animation* anim = skel.getAnimation("Walk")        
		manuallyControlledBone = skel.getBone("Joint10") 
		manuallyControlledBone.setManuallyControlled(True) 
		anim.destroyNodeTrack(manuallyControlledBone.getHandle()) 

		##AnimationState* animState = ent.getAnimationState("Walk") 
		##animState.setEnabled(True) 





def testMaterialSchemes()


		Entity *ent = self.sceneManager.createEntity("robot", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		self.sceneManager.setAmbientLight(ColourValue(0.8, 0.8, 0.8)) 

		MaterialPtr mat = MaterialManager.getSingleton().create("schemetest", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		## default scheme
		mat.getTechnique(0).getPass(0).createTextureUnitState("GreenSkin.jpg") 

		Technique* t = mat.createTechnique() 
		t.setSchemeName("newscheme") 
		t.createPass().createTextureUnitState("rockwall.tga") 
		ent.setMaterialName("schemetest") 

		## create a second viewport using alternate scheme
		Viewport* vp = mWindow.addViewport(mCamera, 1, 0.75, 0, 0.25, 0.25) 
		vp.setMaterialScheme("newscheme") 
		vp.setOverlaysEnabled(False) 


def testMaterialSchemesWithLOD()


		Entity *ent = self.sceneManager.createEntity("robot", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		self.sceneManager.setAmbientLight(ColourValue(0.8, 0.8, 0.8)) 

		MaterialPtr mat = MaterialManager.getSingleton().create("schemetest", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		## default scheme
		mat.getTechnique(0).getPass(0).createTextureUnitState("GreenSkin.jpg") 

		## LOD 0, newscheme 
		Technique* t = mat.createTechnique() 
		t.setSchemeName("newscheme") 
		t.createPass().createTextureUnitState("rockwall.tga") 
		ent.setMaterialName("schemetest") 

		## LOD 1, default
		t = mat.createTechnique() 
		t.setLodIndex(1) 
		t.createPass().createTextureUnitState("WeirdEye.png") 

		## LOD 1, newscheme
		t = mat.createTechnique() 
		t.setLodIndex(1) 
		t.createPass().createTextureUnitState("r2skin.jpg") 
		t.setSchemeName("newscheme") 

		Material.LodDistanceList ldl 
		ldl.push_back(500.0) 
		mat.setLodLevels(ldl) 


		ent.setMaterialName("schemetest") 

		## create a second viewport using alternate scheme
		Viewport* vp = mWindow.addViewport(mCamera, 1, 0.75, 0, 0.25, 0.25) 
		vp.setMaterialScheme("newscheme") 
		vp.setOverlaysEnabled(False) 


def testMaterialSchemesWithMismatchedLOD()


		Entity *ent = self.sceneManager.createEntity("robot", "robot.mesh") 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent) 
		self.sceneManager.setAmbientLight(ColourValue(0.8, 0.8, 0.8)) 

		MaterialPtr mat = MaterialManager.getSingleton().create("schemetest", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		## default scheme
		mat.getTechnique(0).getPass(0).createTextureUnitState("GreenSkin.jpg") 

		## LOD 0, newscheme 
		Technique* t = mat.createTechnique() 
		t.setSchemeName("newscheme") 
		t.createPass().createTextureUnitState("rockwall.tga") 
		ent.setMaterialName("schemetest") 

		## LOD 1, default
		t = mat.createTechnique() 
		t.setLodIndex(1) 
		t.createPass().createTextureUnitState("WeirdEye.png") 

		## LOD 2, default
		t = mat.createTechnique() 
		t.setLodIndex(2) 
		t.createPass().createTextureUnitState("clouds.jpg") 

		## LOD 1, newscheme
		t = mat.createTechnique() 
		t.setLodIndex(1) 
		t.createPass().createTextureUnitState("r2skin.jpg") 
		t.setSchemeName("newscheme") 

		## No LOD 2 for newscheme! Should fallback on LOD 1

		Material.LodDistanceList ldl 
		ldl.push_back(250.0) 
		ldl.push_back(500.0) 
		mat.setLodLevels(ldl) 


		ent.setMaterialName("schemetest") 

		## create a second viewport using alternate scheme
		Viewport* vp = mWindow.addViewport(mCamera, 1, 0.75, 0, 0.25, 0.25) 
		vp.setMaterialScheme("newscheme") 
		vp.setOverlaysEnabled(False) 


    (self): testSkeletonAnimationOptimise((self):)

        self.sceneManager.setShadowTextureSize(512) 
        self.sceneManager.setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE) 
        self.sceneManager.setShadowFarDistance(1500) 
        self.sceneManager.setShadowColour(ColourValue(0.35, 0.35, 0.35)) 
        ##self.sceneManager.setShadowFarDistance(800) 
        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.3, 0.3, 0.3)) 

        mLight = self.sceneManager.createLight("MainLight") 

/*/
        ## Directional test
        mLight.setType(Light.LT_DIRECTIONAL) 
        Vector3 vec(-1,-1,0) 
        vec.normalise() 
        mLight.setDirection(vec) 
/*/
        ## Potest
        mLight.setType(Light.LT_POINT) 
        mLight.setPosition(0, 200, 0) 
##*/

        Entity* pEnt 

        ## Hardware animation
        pEnt = self.sceneManager.createEntity( "1", "robot.mesh" ) 
        mAnimState = pEnt.getAnimationState("Walk") 
        mAnimState.setEnabled(True) 
        mTestNode[0] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[0].attachObject( pEnt ) 
        mTestNode[0].translate(+100,-100,0) 

        ## Software animation
        pEnt = self.sceneManager.createEntity( "2", "robot.mesh" ) 
        pEnt.setMaterialName("BaseWhite") 
/*/
        mAnimState = pEnt.getAnimationState("Walk") 
        mAnimState.setEnabled(True) 
/*/
        pEnt.getAnimationState("Walk").setEnabled(True) 
##*/
        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[1].attachObject( pEnt ) 
        mTestNode[1].translate(-100,-100,0) 


        Plane plane 
        plane.normal = Vector3.UNIT_Y 
        plane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt 
        pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
        pPlaneEnt.setMaterialName("2 - Default") 
        pPlaneEnt.setCastShadows(False) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 


    (self): testBuildTangentOnAnimatedMesh((self):)

        ##self.sceneManager.setShadowTextureSize(512) 
        ##self.sceneManager.setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE) 
        ##self.sceneManager.setShadowFarDistance(1500) 
        ##self.sceneManager.setShadowColour(ColourValue(0.35, 0.35, 0.35)) 
        ##self.sceneManager.setShadowFarDistance(800) 
        ## Set ambient light
        self.sceneManager.setAmbientLight(ColourValue(0.3, 0.3, 0.3)) 

        mLight = self.sceneManager.createLight("MainLight") 

/*/
        ## Directional test
        mLight.setType(Light.LT_DIRECTIONAL) 
        Vector3 vec(-1,-1,0) 
        vec.normalise() 
        mLight.setDirection(vec) 
/*/
        ## Potest
        mLight.setType(Light.LT_POINT) 
        mLight.setPosition(0, 200, 0) 
##*/
        MeshPtr pMesh = MeshManager.getSingleton().load("ninja.mesh",
            ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME/*,    
            HardwareBuffer.HBU_DYNAMIC_WRITE_ONLY, 
            HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
            True, true*/)  ##so we can still read it
        ## Build tangent vectors, all our meshes use only 1 texture coordset 
        unsigned short src, dest 
        if (!pMesh.suggestTangentVectorBuildParams(VES_TANGENT, src, dest)):

            pMesh.buildTangentVectors(VES_TANGENT, src, dest) 


        Entity* pEnt = self.sceneManager.createEntity("Ninja", "ninja.mesh") 

/*/
        mAnimState = pEnt.getAnimationState("Walk") 
        mAnimState.setEnabled(True) 
/*/
        pEnt.getAnimationState("Walk").setEnabled(True) 
##*/
        mTestNode[1] = self.sceneManager.getRootSceneNode().createChildSceneNode() 
        mTestNode[1].attachObject( pEnt ) 
        mTestNode[1].translate(-100,-100,0) 


        Plane plane 
        plane.normal = Vector3.UNIT_Y 
        plane.d = 100 
        MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
        Entity* pPlaneEnt 
        pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
        pPlaneEnt.setMaterialName("2 - Default") 
        pPlaneEnt.setCastShadows(False) 
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

def testVertexTexture()


		## NOTE: DirectX only right now

		Light* l = self.sceneManager.createLight("MainLight") 
		l.setType(Light.LT_POINT) 
		l.setPosition(0, 200, 0) 


		## Create single-channel floating potexture, no mips
		TexturePtr tex = TextureManager.getSingleton().createManual(
			"vertexTexture", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			128, 128, 0, PF_FLOAT32_R) 
		float* pData = static_cast<float*>(
			tex.getBuffer().lock(HardwareBuffer.HBL_DISCARD)) 
		## write concentric circles into the texture
		for (y  = -64  y < 64; ++y)

			for (x = -64  x < 64; ++x)


				float val = Math.Sqrt(x*x + y*y) 
				## repeat every 20 pixels
				val = val * Math.TWO_PI / 20.0 
				*pData++ = Math.Sin(val) 


		tex.getBuffer().unlock() 

		String progSource = 
			"(self): main(\n"
				"float4 pos : POSITION,\n"
				"float2 uv1 : TEXCOORD0,\n"
				"uniform float4x4 world, \n"
				"uniform float4x4 viewProj,\n"
				"uniform float heightscale,\n"
				"uniform sampler2D heightmap,\n"
				"out float4 oPos : POSITION,\n"
				"out float2 oUv1 : TEXCOORD1,\n"
				"out float4 col : COLOR)\n"
			"{\n"
				"oPos = mul(world, pos) \n"
				"## tex2Dlod since no mip\n"
				"float4 t = float4(0,0,0,0) \n"
				"t.xy = uv1.xy \n"
				"float height = tex2Dlod(heightmap, t) \n"
				"oPos.y = oPos.y + (height * heightscale) \n"
				"oPos = mul(viewProj, oPos) \n"
				"oUv1 = uv1 \n"
				"col = float4(1,1,1,1) \n"
			"}\n" 
		HighLevelGpuProgramPtr prog = HighLevelGpuProgramManager.getSingleton().createProgram(
			"TestVertexTextureFetch", ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, 
			"hlsl", GPT_VERTEX_PROGRAM) 
		prog.setSource(progSource) 
		prog.setParameter("target", "vs_3_0") 
		prog.setVertexTextureFetchRequired(True) 
		prog.setParameter("entry_point", "main") 
		prog.load() 


		MaterialPtr mat = MaterialManager.getSingleton().create("TestVertexTexture", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* pass = mat.getTechnique(0).getPass(0) 
		pass.setLightingEnabled(False) 
		pass.setVertexProgram("TestVertexTextureFetch") 
		GpuProgramParametersSharedPtr vp = pass.getVertexProgramParameters() 
		vp.setNamedAutoConstant("world", GpuProgramParameters.ACT_WORLD_MATRIX) 
		vp.setNamedAutoConstant("viewProj", GpuProgramParameters.ACT_VIEWPROJ_MATRIX) 
		vp.setNamedConstant("heightscale", 30.0) 
		## vertex texture
		TextureUnitState* t = pass.createTextureUnitState("vertexTexture") 
		t.setBindingType(TextureUnitState.BT_VERTEX) 
		## regular texture
		pass.createTextureUnitState("BumpyMetal.jpg") 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		## 128 x 128 segment plane
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,128,128,True,1,1,1,Vector3.UNIT_Z) 
		Entity* pPlaneEnt 
		pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("TestVertexTexture") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 




def testBackgroundLoadResourceGroup()

		ResourceGroupManager& rgm = ResourceGroupManager.getSingleton() 
		TextureManager& tm = TextureManager.getSingleton() 
		MeshManager& mm = MeshManager.getSingleton() 

		testBackgroundLoadGroup = "Deferred" 

		rgm.createResourceGroup(testBackgroundLoadGroup) 

		## define a bunch of textures as deferred loading
		rgm.declareResource("egyptrockyfull.jpg", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("fw12b.jpg", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("grass_1024.jpg", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("GreenSkin.jpg", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("MtlPlat2.jpg", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("NMBumpsOut.png", tm.getResourceType(), testBackgroundLoadGroup) 
		rgm.declareResource("ogrehead.mesh", mm.getResourceType(), testBackgroundLoadGroup) 
		## Note: initialise resource group in main thread for self test
		## We will be able to initialise in the background thread too eventually,
		## once resources can be created thread safely as well as loaded
		rgm.initialiseResourceGroup(testBackgroundLoadGroup) 

		## we won't load it yet, we'll wait for 5 seconds


		## Create a basic plane to have something in the scene to look at
		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt 
		pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 








def testOverlayRelativeMode()


		Overlay* o = OverlayManager.getSingleton().create("Test") 
		OverlayContainer* c = (OverlayContainer*)OverlayManager.getSingleton().createOverlayElement("Panel", "panel1") 
		c.setMetricsMode(GMM_RELATIVE) 
		c.setDimensions(1.0, 1.0) 
		c.setMaterialName("Core/StatsBlockCenter") 
		c.setPosition(0.0, 0.0) 

		TextAreaOverlayElement* t = (TextAreaOverlayElement*)OverlayManager.getSingleton().createOverlayElement("TextArea", "text1") 
		t.setMetricsMode(GMM_RELATIVE) 
		t.setCaption("Hello") 
		t.setPosition(0,0) 
		t.setFontName("BlueHighway") 
		t.setDimensions(0.2, 0.5) 
		t.setCharHeight(0.2) 
		c.addChild(t) 

		o.add2D(c) 
		o.show() 



def testBillboardOrigins()

		self.sceneManager.setAmbientLight(ColourValue(0.5, 0.5, 0.5)) 
		Vector3 dir(-1, -1, 0.5) 
		dir.normalise() 
		Light* l = self.sceneManager.createLight("light1") 
		l.setType(Light.LT_DIRECTIONAL) 
		l.setDirection(dir) 

		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 0 
		MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,10,10,True,1,5,5,Vector3.UNIT_Z) 
		Entity* pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		pPlaneEnt.setMaterialName("2 - Default") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		BillboardSet* bbs = self.sceneManager.createBillboardSet("1") 
		bbs.setDefaultDimensions(50,50) 
		bbs.createBillboard(0, 0, 0) 
		bbs.setBillboardOrigin(BBO_TOP_LEFT) 
		##bbs.setBillboardType(BBT_ORIENTED_COMMON) 
		bbs.setCommonDirection(Vector3.UNIT_Y) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(bbs) 



def testDepthBias()

		ResourceGroupManager.getSingleton().addResourceLocation(
			"../../../../Tests/Media", "FileSystem") 

		self.sceneManager.setAmbientLight(ColourValue.White) 
		
		MaterialPtr mat = MaterialManager.getSingleton().create("mat1", 
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
		Pass* p = mat.getTechnique(0).getPass(0) 
		p.createTextureUnitState("BumpyMetal.jpg") 
		
		 String meshName("cube.mesh")  
		Entity* entity = self.sceneManager.createEntity("base", meshName) 
		entity.setMaterialName("mat1") 
		self.sceneManager.getRootSceneNode().attachObject(entity) 


		entity = self.sceneManager.createEntity("base2", meshName) 
		entity.setMaterialName("Examples/EnvMappedRustySteel") 
		SceneNode* n = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		n.setPosition(-30, 0, 0) 
		n.yaw(Degree(45)) 
		n.attachObject(entity) 

		for (size_t i = 0  i <= 6;++i)

			String name("decal") 
			name += StringConverter.toString(i) 

			MaterialPtr pMat = static_cast<MaterialPtr>(MaterialManager.getSingleton().create(
				name, ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)) 

			pMat.getTechnique(0).getPass(0).setLightingEnabled(False) 
			pMat.getTechnique(0).getPass(0).setAlphaRejectSettings(CMPF_GREATER_EQUAL, 128) 
			pMat.getTechnique(0).getPass(0).setDepthBias(i) 
			pMat.getTechnique(0).getPass(0).createTextureUnitState(name + ".png") 

			entity = self.sceneManager.createEntity(name, meshName) 
			entity.setMaterialName(name) 
			self.sceneManager.getRootSceneNode().attachObject(entity) 




		mCamera.setPosition(0,0,200) 
		mCamera.lookAt(Vector3.ZERO) 





def testCustomSequenceTextureShadows()

		##self.sceneManager.setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_CUSTOM_SEQUENCE) 
		MaterialManager.getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC) 
		MaterialManager.getSingleton().setDefaultAnisotropy(5) 

		self.sceneManager.setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE) 
		self.sceneManager.setShadowTextureSettings(1024, 2) 

		self.sceneManager.setAmbientLight(ColourValue.Black) 
		Light* l = self.sceneManager.createLight("Spot1") 
		l.setType(Light.LT_SPOTLIGHT) 
		l.setAttenuation(5000,1,0,0) 
		l.setSpotlightRange(Degree(30),Degree(45),1.0) 
		SceneNode* lightNode1 = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		lightNode1.attachObject(l) 
		lightNode1.setPosition(200, 250, 500) 
		lightNode1.lookAt(Vector3(0,-200,0), Node.TS_WORLD) 
		l.setDirection(Vector3.NEGATIVE_UNIT_Z) 
		l.setDiffuseColour(0.5, 0.7, 0.5) 

		l = self.sceneManager.createLight("Spot2") 
		l.setType(Light.LT_SPOTLIGHT) 
		l.setAttenuation(5000,1,0,0) 
		l.setSpotlightRange(Degree(30),Degree(45),1.0) 
		SceneNode* lightNode2 = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		lightNode2.attachObject(l) 
		lightNode2.setPosition(-500, 200, 500) 
		lightNode2.lookAt(Vector3(0,-200,0), Node.TS_WORLD) 
		l.setDirection(Vector3.NEGATIVE_UNIT_Z) 
		l.setDiffuseColour(1, 0.2, 0.2) 

		## Create a basic plane to have something in the scene to look at
		Plane plane 
		plane.normal = Vector3.UNIT_Y 
		plane.d = 100 
		MeshPtr msh = MeshManager.getSingleton().createPlane("Myplane",
			ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
			4500,4500,100,100,True,1,40,40,Vector3.UNIT_Z) 
		msh.buildTangentVectors(VES_TANGENT) 
		Entity* pPlaneEnt 
		pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" ) 
		##pPlaneEnt.setMaterialName("Examples/OffsetMapping/Specular") 
		pPlaneEnt.setMaterialName("Examples/OffsetMapping/CustomShadows") 
		pPlaneEnt.setCastShadows(False) 
		self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt) 

		pPlaneEnt = self.sceneManager.createEntity( "plane2", "Myplane" ) 
		##pPlaneEnt.setMaterialName("Examples/OffsetMapping/Specular") 
		pPlaneEnt.setMaterialName("Examples/OffsetMapping/CustomShadows") 
		pPlaneEnt.setCastShadows(False) 
		SceneNode* n = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		n.roll(Degree(90)) 
		n.translate(100,0,0) 
		##n.attachObject(pPlaneEnt) 

		pPlaneEnt = self.sceneManager.createEntity( "plane3", "Myplane" ) 
		##pPlaneEnt.setMaterialName("Examples/OffsetMapping/Specular") 
		pPlaneEnt.setMaterialName("Examples/OffsetMapping/CustomShadows") 
		pPlaneEnt.setCastShadows(False) 
		n = self.sceneManager.getRootSceneNode().createChildSceneNode() 
		n.pitch(Degree(90)) 
		n.yaw(Degree(-90)) 
		n.translate(0,0,-100) 
		##n.attachObject(pPlaneEnt) 

		mCamera.setPosition(-50, 500, 1000) 
		mCamera.lookAt(Vector3(-50,-100,0)) 

		Entity* ent = self.sceneManager.createEntity("athene", "athene.mesh") 
		ent.setMaterialName("Examples/Athene/NormalMapped") 
		self.sceneManager.getRootSceneNode().createChildSceneNode(Vector3(0,-20,0)).attachObject(ent) 





	## Just override the mandatory create scene method
    (self): createScene((self):)

		/*
		AnyNumeric anyInt1(43) 
		AnyNumeric anyInt2(5) 
		AnyNumeric anyInt3 = anyInt1 + anyInt2 

		bool tst = StringConverter.isNumber("3") 
		tst = StringConverter.isNumber("-0.3") 
		tst = StringConverter.isNumber("") 
		tst = StringConverter.isNumber("  ") 
		tst = StringConverter.isNumber(" -0.3 ") 
		tst = StringConverter.isNumber(" a-0.3 ") 

		 StringVector& l = mCamera.getAnimableValueNames() 

		std.cout << anyInt3 

		##Any anyString("test") 
		*/

        ##testMatrices() 
        ##testBsp() 
        ##testAlpha() 
        ##testAnimation() 
		##testAnimationBlend() 

        ##testGpuPrograms() 
        ##testMultiViewports() 
        ##testDistortion() 
        ##testEdgeBuilderSingleIndexBufSingleVertexBuf() 
        ##testEdgeBuilderMultiIndexBufSingleVertexBuf() 
        ##testEdgeBuilderMultiIndexBufMultiVertexBuf() 
        ##testPrepareShadowVolume() 
        ##testWindowedViewportMode() 
        ##testSubEntityVisibility() 
        ##testAttachObjectsToBones() 
        ##testSkeletalAnimation() 
        ##testOrtho() 
        ##testClearScene() 
		##testInfiniteAAB() 

        ##testProjection() 
        ##testStencilShadows(SHADOWTYPE_STENCIL_ADDITIVE, True, true) 
        ##testStencilShadows(SHADOWTYPE_STENCIL_MODULATIVE, False, True) 
        ##testTextureShadows(SHADOWTYPE_TEXTURE_ADDITIVE) 
		##testTextureShadows(SHADOWTYPE_TEXTURE_MODULATIVE) 
		##testCustomSequenceTextureShadows() 
		##testTextureShadowsCustomCasterMat(SHADOWTYPE_TEXTURE_ADDITIVE) 
		##testTextureShadowsCustomReceiverMat(SHADOWTYPE_TEXTURE_MODULATIVE) 
		##testCompositorTextureShadows(SHADOWTYPE_TEXTURE_MODULATIVE) 
		##testSplitPassesTooManyTexUnits() 
        ##testOverlayZOrder() 
		##testReflectedBillboards() 
		##testBlendDiffuseColour() 

        ##testRaySceneQuery() 
        ##testIntersectionSceneQuery() 

        ##test2Spotlights() 
		testDepthBias() 

		##testManualLOD() 
		##testGeneratedLOD() 
		##testLotsAndLotsOfEntities() 
		##testSimpleMesh() 
		##test2Windows() 
		##testStaticGeometry() 
		##testBillboardTextureCoords() 
		##testBillboardOrigins() 
		##testReloadResources() 
		##testTransparencyMipMaps() 
		##testRadixSort() 
		##testMorphAnimation() 
		##testPoseAnimation() 
		##testPoseAnimation2() 
		##testBug() 
		##testManualBlend() 
		##testManualObjectNonIndexed() 
		##testManualObjectIndexed() 
		##testManualObjectNonIndexedUpdateSmaller() 
		##testManualObjectNonIndexedUpdateLarger() 
		##testManualObjectIndexedUpdateSmaller() 
		##testManualObjectIndexedUpdateLarger() 
		##testCustomProjectionMatrix() 
		##testPointSprites() 
		##testFallbackResourceGroup() 
		##testSuppressedShadows(SHADOWTYPE_TEXTURE_ADDITIVE) 
		##testViewportNoShadows(SHADOWTYPE_TEXTURE_ADDITIVE) 
		##testBillboardChain() 
		##testRibbonTrail() 
		##testSerialisedColour() 
		##testBillboardAccurateFacing() 
		##testMultiSceneManagersSimple() 
		##testMultiSceneManagersComplex() 
		##testManualBoneMovement() 
		##testMaterialSchemes() 
		##testMaterialSchemesWithLOD() 
		##testMaterialSchemesWithMismatchedLOD() 
        ##testSkeletonAnimationOptimise() 
        ##testBuildTangentOnAnimatedMesh() 
		##testOverlayRelativeMode() 

		##testCubeDDS() 
		##testDxt1() 
		##testDxt1Alpha() 
		##testDxt3() 
		##testDxt5() 
		##testFloat64DDS() 
		##testFloat128DDS() 
		##testFloat16DDS() 
		##testFloat32DDS() 

		##testVertexTexture() 
		##testGLSLTangent() 
		##testBackgroundLoadResourceGroup() 
		

    ## Create new frame listener
    (self): createFrameListener((self):)

        mFrameListener= new PlayPenListener(self.sceneManager, mWindow, mCamera) 
        mFrameListener.showDebugOverlay(True) 
		self.root.addFrameListener(mFrameListener) 
        ##self.root.addFrameListener(fl) 


    

public:
    (self): go((self):)

        if (!setup()):
            return 

        self.root.startRendering() 






##-----------------------------------------------------------------------------
##-----------------------------------------------------------------------------
bool gReload 

## Listener class for frame updates
class MemoryTestFrameListener : public FrameListener

protected:
	time 
	Keyboard* self.keyboard 
public:
	MemoryTestFrameListener(RenderWindow * win)

		time = 0 
		ParamList pl 	
		size_t windowHnd = 0 
		std.ostringstream windowHndStr 

		win.getCustomAttribute("WINDOW", &windowHnd) 
		windowHndStr << windowHnd 
		pl.insert(std.make_pair(std::string("WINDOW"), windowHndStr.str())) 

		InputManager &im = *InputManager.createInputSystem( pl ) 

		##Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		self.keyboard = static_cast<Keyboard*>(im.createInputObject( OISKeyboard, False )) 

	virtual ~MemoryTestFrameListener()

		time = 0             


	bool frameStarted( FrameEvent& evt)

		if( self.keyboard.isKeyDown( OIS.KC_ESCAPE) ):
OIS.OIS.KC_
			gReload = False 
			return False 


		time += evt.timeSinceLastFrame 
		if(time>5):

			LogManager.getSingleton().logMessage("Reloading scene after 5 seconds") 
			gReload = True 
			time=0 
			return False 

		else:

			gReload = False 
			return True 



def keyClicked(KeyEvent* e) {} 
def keyPressed(KeyEvent* e) {} 
def keyReleased(KeyEvent* e) {} 
def keyFocusIn(KeyEvent* e) {}
def keyFocusOut(KeyEvent* e) {}


# /** Application class */

# class MemoryTestApplication : public ExampleApplication

# protected:
# 	MemoryTestFrameListener * mTestFrameListener 
# public:

# 	(self): go((self):)

# 		self.root = 0 
# 		if (!setup()):
# 			return 

# 		self.root.startRendering() 

# 		while(gReload)

# 			## clean up
# 			destroyScene() 
# 			destroyResources() 
# 			if (!setup()):
# 				return 
# 			self.root.startRendering() 

# 		## clean up
# 		destroyScene() 


# 	bool setup((self):)

# 		if(!gReload):
# 			self.root = new Root() 

# 		setupResources() 

# 		if(!gReload):

# 			bool carryOn = configure() 
# 			if (!carryOn):
# 				return False 

# 			chooseSceneManager() 
# 			createCamera() 
# 			createViewports() 

# 			## Set default mipmap level (NB some APIs ignore self)
# 			TextureManager.getSingleton().setDefaultNumMipmaps(5) 

# 			## Create any resource listeners (for loading screens)
# 			createResourceListener() 

# 			createFrameListener() 

# 		## Load resources
# 		loadResources() 

# 		## Create the scene
# 		createScene()         

# 		return True 



# 	##/ Method which will define the source of resources (other than current folder)
# 	virtual (self): setupResources((self):)

# 		## Custom setup
# 		ResourceGroupManager.getSingleton().createResourceGroup("CustomResourceGroup") 
# 		ResourceGroupManager.getSingleton().addResourceLocation(
# 			"../../../media/ogrehead.zip", "Zip", "CustomResourceGroup") 

# 	(self): loadResources((self):)

# 		## Initialise, parse scripts etc
# 		ResourceGroupManager.getSingleton().initialiseAllResourceGroups() 

# 	(self): destroyResources()

# 		LogManager.getSingleton().logMessage("Destroying resources") 
# 		ResourceGroupManager.getSingleton().removeResourceLocation(
# 			"../../../media/ogrehead.zip") 
# 		ResourceGroupManager.getSingleton().destroyResourceGroup("CustomResourceGroup") 


# 	(self): createScene((self):)

# 		## Set a very low level of ambient lighting
# 		self.sceneManager.setAmbientLight(ColourValue(0.1, 0.1, 0.1)) 

# 		## Load ogre head
# 		MeshManager.getSingleton().load("ogrehead.mesh","CustomResourceGroup") 
# 		Entity* head = self.sceneManager.createEntity("head", "ogrehead.mesh") 

# 		## Attach the head to the scene
# 		self.sceneManager.getRootSceneNode().attachObject(head) 



# 	(self): createFrameListener((self):)

# 		## This is where we instantiate our own frame listener
# 		mTestFrameListener= new MemoryTestFrameListener(mWindow) 
# 		self.root.addFrameListener(mTestFrameListener) 
# 		/*if(!gReload):

# 		ExampleApplication.createFrameListener() 



# 	(self): destroyScene((self):)

# 		LogManager.getSingleton().logMessage("Clearing scene") 
# 		self.sceneManager.clearScene() 




if __name__ == '__main__':
    try:
        application = PlayPenApplication()
        application.go()
    except ogre.Exception, e:
        pre
        prdir(e)
    
