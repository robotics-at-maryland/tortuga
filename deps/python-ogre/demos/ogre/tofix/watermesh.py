# /*
# -----------------------------------------------------------------------------
# This source file is part of OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http:##www.ogre3d.org/

# Copyright (c) 2000-2006 Torus Knot Software Ltd
# Also see acknowledgements in Readme.html

# You may use self sample code for anything you like, it is not covered by the
# LGPL like the rest of the engine.
# -----------------------------------------------------------------------------
# */



ANIMATIONS_PER_SECOND = 100.0
class WaterMesh:
    def __init__ ( self, meshName, planeSize, complexity ):
    	self.meshName = meshName  
    	self.complexity =  complexity  
    	numFaces = 2 * complexity * complexity 
    	numVertices = (complexity + 1) * (complexity + 1)  
    	lastTimeStamp = 0  
    	lastAnimationTimeStamp = 0 
    	lastFrameTime = 0  
    
    	## initialize algorithm parameters
    	PARAM_C = 0.3   ## ripple speed
    	PARAM_D = 0.4   ## distance
    	PARAM_U = 0.05   ## viscosity
    	PARAM_T = 0.13   ## time
    	useFakeNormals = False  
    
    	## allocate space for normal calculation
    	vNormals = Ogre.Vector3[numVertices] 
    
    	## create mesh and submesh
    	mesh = MeshManager.getSingleton().createManual(meshName,
            ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
    	subMesh = mesh.createSubMesh() 
    	subMesh.useSharedVertices=False 
    
    	## Vertex buffers
    	subMesh.vertexData = Ogre.VertexData() 
    	subMesh.vertexData.vertexStart = 0 
    	subMesh.vertexData.vertexCount = numVertices 
    
    	vdecl = subMesh.vertexData.vertexDeclaration 
    	vbind = subMesh.vertexData.vertexBufferBinding 
    
    
    	vdecl.addElement(0, 0, VET_FLOAT3, VES_POSITION) 
    	vdecl.addElement(1, 0, VET_FLOAT3, VES_NORMAL) 
    	vdecl.addElement(2, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES) 
    
    	## Prepare buffer for positions - todo: first attempt, slow
    	posVertexBuffer =
             HardwareBufferManager.getSingleton().createVertexBuffer(
                3*sizeof(float),
    			numVertices,
    			HardwareBuffer.HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE) 
    	vbind.setBinding(0, posVertexBuffer) 
    
    	## Prepare buffer for normals - write only
    	normVertexBuffer =
             HardwareBufferManager.getSingleton().createVertexBuffer(
                3*sizeof(float),
    			numVertices,
    			HardwareBuffer.HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE) 
    	vbind.setBinding(1, normVertexBuffer) 
    
    	## Prepare texture coords buffer - static one
    	## todo: optimize to write directly into buffer
    	float *texcoordsBufData = new float[numVertices*2] 
    	for(y in range (complexity) :
    		for(x in range(complexity) :
    			texcoordsBufData[2*(y*(complexity+1)+x)+0] = (float)x / complexity  
    			texcoordsBufData[2*(y*(complexity+1)+x)+1] = 1.0 - ((float)y / (complexity))  
    
    
    	texcoordsVertexBuffer =
             HardwareBufferManager.getSingleton().createVertexBuffer(
                2*sizeof(float),
    			numVertices,
    			HardwareBuffer.HBU_STATIC_WRITE_ONLY) 
    	texcoordsVertexBuffer.writeData(0,
    		texcoordsVertexBuffer.getSizeInBytes(),
    		texcoordsBufData,
    		True)  ## true?
    	del texcoordsBufData 
        vbind.setBinding(2, texcoordsVertexBuffer) 
    
    	## Prepare buffer for indices
    	indexBuffer =
    		HardwareBufferManager.getSingleton().createIndexBuffer(
    			HardwareIndexBuffer.IT_16BIT,
    			3*numFaces,
    			HardwareBuffer.HBU_STATIC, True) 
    	unsigned short *faceVertexIndices = (unsigned short*)
    		indexBuffer.lock(0, numFaces*3*2, HardwareBuffer.HBL_DISCARD) 
    	for y in range (complexity) :
    		for  x in range (complexity ) :
    			unsigned short *twoface = faceVertexIndices + (y*complexity+x)*2*3 
    			p0 = y*(complexity+1) + x  
    			p1 = y*(complexity+1) + x + 1  
    			p2 = (y+1)*(complexity+1) + x  
    			p3 = (y+1)*(complexity+1) + x + 1  
    			twoface[0]=p2  ##first tri
    			twoface[1]=p1 
    			twoface[2]=p0 
    			twoface[3]=p2  ##second tri
    			twoface[4]=p3 
    			twoface[5]=p1 
    
    
    	indexBuffer.unlock() 
    	## Set index buffer for self submesh
    	subMesh.indexData.indexBuffer = indexBuffer 
    	subMesh.indexData.indexStart = 0 
    	subMesh.indexData.indexCount = 3*numFaces 
    
    # 	/*	prepare vertex positions
    # 	 *	note - we use 3 vertex buffers, since algorighm uses two last phases
    # 	 *	to calculate the next one
    # 	 */
    	for b in range(3) :
    		vertexBuffers[b] = new float[numVertices * 3]  
    		for y in range(complexity) :
    			for x in range (complexity) :
    				numPoint = y*(complexity+1) + x  
    				float* vertex = vertexBuffers[b] + 3*numPoint  
    				vertex[0]= (x) / (complexity) * planeSize  
    				vertex[1]= 0   ## rand() % 30 ;
    				vertex[2]= (y) / (complexity) * planeSize  
    
    	AxisAlignedBox meshBounds(0,0,0,
    		planeSize,0, planeSize) 
    	mesh._setBounds(meshBounds) 
    
    	currentBuffNumber = 0  
    	posVertexBuffer.writeData(0,
    		posVertexBuffer.getSizeInBytes(), ## size
    		vertexBuffers[currentBuffNumber], ## source
    		True)  ## discard?
    
        mesh.load() 
        mesh.touch() 
    
    # /* ========================================================================= */
    def __del__ (self):
    
     	del vertexBuffers[0] 
     	del vertexBuffers[1] 
     	del vertexBuffers[2] 
    
    	del vNormals 
    
    /* ========================================================================= */
    def push( self, x, y, depth, absolute):
    
    	*buf = vertexBuffers[currentBuffNumber]+1  
    	## scale pressure according to time passed
    	depth = depth * lastFrameTime * ANIMATIONS_PER_SECOND  
    	self._PREP(0,0) 
    	self._PREP(0,1) 
    	self._PREP(1,0) 
    	self._PREP(1,1)
    	
    def _PREP(self, addx,addy) :
    	*vertex=buf+3*((int)(y+addy)*(complexity+1)+(int)(x+addx)) 
    	diffy = y - floor(y+addy)
    	diffx = x - floor(x+addx)
    	dist=sqrt(diffy*diffy + diffx*diffx)
    	power = 1 - dist
    	if (power<0):
    		power = 0
    	if (absolute):
    		*vertex = depth*power
    	else:
    		*vertex += depth*power
    
    def fefine hat(self, _x,_y):
        buf[3*(_y*(complexity+1)+(_x))]
    	 
    
    # /* ========================================================================= */
    def getHeight(self,  x, y):
    
    	float *buf = vertexBuffers[currentBuffNumber]  
    	xa = floor(x) 
    	xb = xa + 1  
    	ya = floor(y) 
    	yb = ya + 1  
    	yaxavg = hat(xa,ya) * (1.0-fabs(xa-x)) + hat(xb,ya) * (1.0f-fabs(xb-x)) 
    	ybxavg = hat(xa,yb) * (1.0-fabs(xa-x)) + hat(xb,yb) * (1.0f-fabs(xb-x)) 
    	yavg = yaxavg * (1.0-fabs(ya-y)) + ybxavg * (1.0f-fabs(yb-y))  
    	return yavg  
    
    /* ========================================================================= */
    def calculateFakeNormals(self):
    	*buf = vertexBuffers[currentBuffNumber] + 1 
    	*pNormals = (float*) normVertexBuffer.lock(
    		0,normVertexBuffer.getSizeInBytes(), HardwareBuffer.HBL_DISCARD) 
    	for y in (complexity) :
    		*nrow = pNormals + 3*y*(complexity+1) 
    		*row = buf + 3*y*(complexity+1)  
    		*rowup = buf + 3*(y-1)*(complexity+1)  
    		*rowdown = buf + 3*(y+1)*(complexity+1)  
    		for x in (complexity) :
    			xdiff = row[3*x+3] - row[3*x-3]  
    			ydiff = rowup[3*x] - rowdown[3*x-3]  
    			norm = Ogre.Vector3(xdiff,30,ydiff) 
    			norm.normalise() 
    			nrow[3*x+0] = norm.x 
    			nrow[3*x+1] = norm.y 
    			nrow[3*x+2] = norm.z 
    
    
    	normVertexBuffer.unlock() 
    
    /* ========================================================================= */
    def calculateNormals(self):
    	*buf = vertexBuffers[currentBuffNumber] + 1 
    	## zero normals
    	for i in range(numVertices) :
    		vNormals[i] = Vector3.ZERO 
    
    	## first, calculate normals for faces, add them to proper vertices
    	buf = vertexBuffers[currentBuffNumber]  
    	vinds = (unsigned short*) indexBuffer.lock(
    		0, indexBuffer.getSizeInBytes(), HardwareBuffer.HBL_READ_ONLY) 
    	*pNormals = (float*) normVertexBuffer.lock(
    		0, normVertexBuffer.getSizeInBytes(), HardwareBuffer.HBL_DISCARD) 
    	for i in range(numFaces) :
    		p0 = vinds[3*i]  
    		p1 = vinds[3*i+1]  
    		p2 = vinds[3*i+2]  
    		Vector3 v0(buf[3*p0], buf[3*p0+1], buf[3*p0+2]) 
    		Vector3 v1(buf[3*p1], buf[3*p1+1], buf[3*p1+2]) 
    		Vector3 v2(buf[3*p2], buf[3*p2+1], buf[3*p2+2]) 
    		Vector3 diff1 = v2 - v1  
    		Vector3 diff2 = v0 - v1  
    		Vector3 fn = diff1.crossProduct(diff2) 
    		vNormals[p0] += fn  
    		vNormals[p1] += fn  
    		vNormals[p2] += fn  
    
    	## now normalize vertex normals
    	for y in (complexity) :
    		for x in (complexity) :
    			numPoint = y*(complexity+1) + x  
    			Vector3 n = vNormals[numPoint]  
    			n.normalise()  
    			normal = pNormals + 3*numPoint  
    			normal[0]=n.x 
    			normal[1]=n.y 
    			normal[2]=n.z 
    
    
    	indexBuffer.unlock() 
    	normVertexBuffer.unlock() 
    
    /* ========================================================================= */
    def updateMesh(self, timeSinceLastFrame):
    	lastFrameTime = timeSinceLastFrame  
    	lastTimeStamp += timeSinceLastFrame  
    
    	## do rendering to get ANIMATIONS_PER_SECOND
    	while(lastAnimationTimeStamp <= lastTimeStamp):
    
    		## switch buffer numbers
    		currentBuffNumber = (currentBuffNumber + 1) % 3  
    		*buf = vertexBuffers[currentBuffNumber] + 1   ## +1 for Y coordinate
    		*buf1 = vertexBuffers[(currentBuffNumber+2)%3] + 1  
    		*buf2 = vertexBuffers[(currentBuffNumber+1)%3] + 1 
    
    # 		/* we use an algorithm from
    # 		 * http:##collective.valve-erc.com/index.php?go=water_simulation
    # 		 * The params could be dynamically changed every frame ofcourse
    # 		 */
    		C = PARAM_C  ## ripple speed
    		D = PARAM_D  ## distance
    		U = PARAM_U  ## viscosity
    		T = PARAM_T  ## time
    		TERM1 = ( 4.0 - 8.0f*C*C*T*T/(D*D) ) / (U*T+2)  
    		TERM2 = ( U*T-2.0 ) / (U*T+2.0f)  
    		TERM3 = ( 2.0 * C*C*T*T/(D*D) ) / (U*T+2)  
    		for y in range(complexity) : ## don't do anything with border values
    			*row = buf + 3*y*(complexity+1)  
    			*row1 = buf1 + 3*y*(complexity+1)  
    			*row1up = buf1 + 3*(y-1)*(complexity+1)  
    			*row1down = buf1 + 3*(y+1)*(complexity+1)  
    			*row2 = buf2 + 3*y*(complexity+1)  
    			for x in range (complexity) :
    				row[3*x] = TERM1 * row1[3*x]
    					+ TERM2 * row2[3*x]
    					+ TERM3 * ( row1[3*x-3] + row1[3*x+3] + row1up[3*x]+row1down[3*x] )  
    		lastAnimationTimeStamp += (1.0 / ANIMATIONS_PER_SECOND) 
    
    
    	if (useFakeNormals):
    		calculateFakeNormals() 
    	else :
    		calculateNormals() 
    
    
    	## set vertex buffer
    	posVertexBuffer.writeData(0,
    		posVertexBuffer.getSizeInBytes(), ## size
    		vertexBuffers[currentBuffNumber], ## source
    		True)  ## discard?

