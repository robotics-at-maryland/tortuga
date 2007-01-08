#include ".\Ragdoll.h"
import Ogre
import OgreNewt

class RagDoll:
    
    class RagBone:
        BS_BOX =1
        BS_ELLIPSOID=2
        BS_CYLINDER=3
        BS_CAPSULE=4
        BS_CONE=5
        BS_CONVEXHULL=6

        def __init__( self, creator,world, parent, ogreBone, mesh, 
                          dir1, shape, size, mass ):

            self.Doll = creator 
            self.Parent = parent 
            self.OgreBone = ogreBone 
        
            ## in the case of the cylindrical primitives, they need to be rotated to align the main axis with the direction vector.
            orient = Ogre.Quaternion.IDENTITY 
            pos = Ogre.Vector3.ZERO 
            rot = Ogre.Matrix3()
        
            if (dir1 == Ogre.Vector3.UNIT_Y):
                rot.FromEulerAnglesXYZ(Ogre.Degree(0), Ogre.Degree(0), Ogre.Degree(90)) 
                orient.FromRotationMatrix( rot ) 
        
            if (dir1 == Ogre.Vector3.UNIT_Z):
                rot.FromEulerAnglesXYZ(Ogre.Degree(0), Ogre.Degree(90), Ogre.Degree(0)) 
                orient.FromRotationMatrix( rot ) 

            ## make the rigid body.
            if shape == RagDoll.RagBone.BS_BOX:
                col = OgreNewt.CollisionPrimitives.Box( world, size ) 
            elif shape == RagDoll.RagBone.BS_CAPSULE:
                col = OgreNewt.CollisionPrimitives.Capsule( world, size.y, size.x, orient, pos ) 
            elif shape == RagDoll.RagBone.BS_CONE:
                col = OgreNewt.CollisionPrimitives.Cone( world, size.y, size.x, orient, pos ) 
            elif shape == RagDoll.RagBone.BS_CYLINDER:
                col = OgreNewt.CollisionPrimitives.Cylinder( world, size.y, size.x, orient, pos ) 
            elif shape == RagDoll.RagBone.BS_ELLIPSOID:
                col = OgreNewt.CollisionPrimitives.Ellipsoid( world, size ) 
            elif shape == RagDoll.RagBone.BS_CONVEXHULL:
                col = _makeConvexHull( world, mesh, size.x ) 
            else:
                col = OgreNewt.CollisionPrimitives.Box( world, size ) 
        
            self.Body = OgreNewt.Body( world, col ) 
            self.Body.setUserData( self ) 
            self.Body.setStandardForceCallback() 
        
            inertia, om = col.calculateInertialMatrix( ) 
            
            self.Body.setMassMatrix( mass, inertia * mass ) 
            self.Body.setCenterOfMass( com ) 
        
            self.Body.setCustomTransformCallback( self, "_placementCallback" ) 


        def __del__(self): ## ragbone
            self.OgreBone.setManuallyControlled( False ) 
            del self.Body 


        def _hingeCallback( self, me ):
            bone = me.getUserData() 
            angle = me.getJointAngle() 
            lim1 = bone.getLimit1() 
            lim2 = bone.getLimit2() 
            if (angle < lim1):
                accel = me.calculateStopAlpha( lim1 ) 
                me.setCallbackAccel( accel ) 
            if (angle > lim2):
                accel = me.calculateStopAlpha( lim2 ) 
                me.setCallbackAccel( accel ) 


        def _makeConvexHull( self, world, mesh, minWeight ):
            ## for self bone, gather all of the vertices linked to it, and make an individual convex hull.
            boneName = self.OgreBone.getName() 
            boneIndex = self.OgreBone.getHandle() 
            invMatrix  = Ogre.Matrix4 ()
            invMatrix.makeInverseTransform( -self.OgreBone._getBindingPoseInversePosition(), 
                                Ogre.Vector3.UNIT_SCALE / self.OgreBone._getBindingPoseInverseScale(), 
                                self.OgreBone._getBindingPoseInverseOrientation().Inverse()) 
        
            num_sub = mesh.getNumSubMeshes() 
        
            for i in range (num_sub):
                submesh = mesh.getSubMesh(i) 
                bai = submesh.getBoneAssignmentIterator() 
        
                if (submesh.useSharedVertices):
                    v_data = mesh.sharedVertexData 
                    v_count = v_data.vertexCount 
                    v_decl = v_data.vertexDeclaration 
                    p_elem = v_decl.findElementBySemantic( Ogre.VES_POSITION ) 
                else:
                    v_data = submesh.vertexData 
                    v_count = v_data.vertexCount 
                    v_decl = v_data.vertexDeclaration 
                    p_elem = v_decl.findElementBySemantic( Ogre.VES_POSITION ) 
                    
                start = v_data.vertexStart 
                ##pointer
                v_sptr = v_data.vertexBufferBinding.getBuffer( p_elem.getSource() ) 
                v_ptr = v_sptr.lock( Ogre.HardwareBuffer.HBL_READ_ONLY )
                
                while (bai.hasMoreElements()):
                    vba = bai.getNext() 
                    if (vba.boneIndex == boneIndex):
                        ##found a vertex that is attached to self bone.
                        if (vba.weight >= minWeight):
                            ##get offset to Position data!
                            v_offset = v_ptr + (vba.vertexIndex * v_sptr.getVertexSize()) 
                            ## AJM
##                            p_elem.baseVertexPointerToElement( v_offset, &v_Posptr ) 
                            p_elem.baseVertexPointerToElement( v_offset, v_Posptr ) 
                            vert = Ogre.Vector3()
                            vert.x = v_Posptr
                            v_Posptr+=1
                            vert.y = v_Posptr
                            v_Posptr+=1
                            vert.z = v_Posptr  
                            ## apply transformation in to local space.
                            vert = invMatrix * vert 
                            vertexVector.push_back( vert ) 
        ##                  Ogre.LogManager.getSingletonPtr().logMessage("  vertex found! id:"+Ogre.StringConverter.toString(vba.vertexIndex)) 
                v_sptr.unlock() 
                
            ## okay, we have gathered all verts for self bone.  make a convex hull!
            numVerts = vertexVector.size() 
            verts = Ogre.Vector3[ numVerts ] 
            j = 0 
            while (not vertexVector.empty()):
                verts[j] = vertexVector.back() 
                vertexVector.pop_back() 
                j+=1 
        
            ##////////////////////////////////////////////////////////////////////////////////
            col = OgreNewt.CollisionPrimitives.ConvexHull( world, verts, numVerts ) 
            del verts 
            return col  


    def __init__( self, filename, world, node ):    # ragdoll __init__
        self.Node = node 
        self.World = world 
        ## get the skeleton.
        self.Skeleton = self.Node.getAttachedObject(0).getSkeleton() 
        ## get the mesh.
        self.Mesh = self.Node.getAttachedObject(0).getMesh() 
        
        #######  AJM - need to convert to Python XML handling :)
#     	doc = OgreNewt.TiXmlDocument()
#         doc.LoadFile( filename ) 
        from xml.dom.minidom import parse, parseString
        doc = parse(filename) # parse an XML file by name
        root = doc.documentElement
#         root = doc.RootElement() 
        if (not root):
            ## error!
            Ogre.LogManager.getSingleton().logMessage(" ERROR! cannot find 'root' in xml file: "+filename ) 
            return 
        ## found the root ragdoll.  find the root bone, and go!
        parent = None 
        bone = root.getElementsByTagName ("Bone")
        ##bone = root.FirstChildElement("Bone") 
        if (bone):
            self._addAllBones( None, bone ) 


    def _addAllBones ( self, parent,  bone):
    
        ## get the information for the bone represented by self element.
        dire = Ogre.StringConverter.parseVector3( bone.getAttribute("dir") ) 
        length = Ogre.StringConverter.parseReal( bone.getAttribute("length") ) 
            
        size = Ogre.StringConverter.parseVector3( bone.getAttribute("size") ) 
        
        skeleton_bone = bone.getAttribute("skeleton_bone") 
        ogrebone = self.Skeleton.getBone( skeleton_bone ) 
    
        shapestr = bone.getAttribute("shape") 
        shape = RagDoll.RagBone.BS_BOX 
    
        if (shapestr=="box"):
            shape = RagDoll.RagBone.BS_BOX 
        elif (shapestr=="capsule"):
            shape = RagDoll.RagBone.BS_CAPSULE 
        elif (shapestr=="cylinder"):
            shape = RagDoll.RagBone.BS_CYLINDER 
        elif (shapestr=="cone"):
            shape = RagDoll.RagBone.BS_CONE 
        elif (shapestr=="ellipsoid"):
            shape = RagDoll.RagBone.BS_ELLIPSOID 
        elif (shapestr=="hull"):
            shape = RagDoll.RagBone.BS_CONVEXHULL 
    
        mass = Ogre.StringConverter.parseReal( bone.Attribute("mass") ) 
        
        ##/////////////////////////////////////////////////////////////////////////////
        me = self._addBone( self.World, parent, dire, shape, size, mass, ogrebone ) 
        ##/////////////////////////////////////////////////////////////////////////////
    
        ## position the bone.
        boneorient = self.Node.getWorldOrientation() * ogrebone._getDerivedOrientation() 
        if (shape != RagDoll.RagBone.BS_CONVEXHULL):
            bonepos = self.Node._getFullTransform() * ogrebone._getDerivedPosition() + (boneorient * (dir * (length*0.5))) 
        else:
            bonepos = self.Node._getFullTransform() * ogrebone._getDerivedPosition() 
        me.getBody().setPositionOrientation( bonepos, boneorient ) 
    
        ## set offsets
        if (not parent):
            offsetorient = (boneorient.Inverse()) * self.Node.getWorldOrientation() 
            offsetpos = boneorient.Inverse() * (self.Node.getWorldPosition() - bonepos) 
            me.setOffset( offsetorient, offsetpos ) 
    
        ## get the joint to connect self bone with it's parent.
        if (parent):
            joint = bone.FirstChildElement("Joint") 
            if (not joint):
                ## error!
                Ogre.LogManager.getSingleton().logMessage(" ERROR! cannot find 'Joint' in xml file!") 
                return 
            jointpin = Ogre.StringConverter.parseVector3( joint.Attribute("pin") ) 
            jointtypestr = joint.Attribute("type") 
            jointtype = RagDoll.JT_BALLSOCKET 
    
            if (jointtypestr == "ballsocket"):
                jointtype = RagDoll.JT_BALLSOCKET 
            elif (jointtypestr == "hinge"):
                jointtype = RagDoll.JT_HINGE 
    
            limit1 = Ogre.StringConverter.parseReal( joint.Attribute("limit1") ) 
            limit2 = Ogre.StringConverter.parseReal( joint.Attribute("limit2") ) 
    
            jpos = self.Node._getFullTransform() * ogrebone._getDerivedPosition() 
            jpin = (self.Node.getWorldOrientation() * parent.getOgreBone()._getDerivedOrientation()) * jointpin 
    
            _joinBones( jointtype, parent, me, jpos, jpin, limit1, limit2) 
    
        ##/////////////////////////////////////////////////////////////////////////////
        ##/////////////////////////////////////////////////////////////////////////////
        ## add all children of self bone.
        child = bone.FirstChildElement("Bone") 
    
        while (child):
            self._addAllBones( me, child ) 
            child = child.NextSiblingElement("Bone") 
    
    
    
    
    #RagDoll.~RagDoll()
    def __del__(self) :
        while (self.Bones.size() > 0):
            bone = self.Bones.back() 
            del bone 
            self.Bones.pop_back() 

    #RagDoll.RagBone* RagDoll
    def _addBone( self, world, parent, dire, shape, size, mass, ogrebone ):
        bone = RagDoll.RagBone( self, world, parent, ogrebone, self.Mesh, dire, shape, size, mass ) 
        ogrebone.setManuallyControlled( True ) 
        self.Bones.push_back( bone ) 
        return bone 
    
    
    def _joinBones( self, typein, parent, child, pos, pin, limit1, limit2 ):
        pin.normalise() 
        if typein == RagDoll.JT_BALLSOCKET:
            joint = OgreNewt.BasicJoints.BallAndSocket( child.getBody().getWorld(), child.getBody(), parent.getBody(), pos ) 
            joint.setLimits(pin, Ogre.Degree(limit1), Ogre.Degree(limit2)) 
        elif typein == RagDoll.JT_HINGE:
            joint = OgreNewt.BasicJoints.Hinge( child.getBody().getWorld(), child.getBody(), parent.getBody(), pos, pin ) 
            joint.setCallback( self, "_hingeCallback" ) 
            joint.setUserData( child ) 
            child.setLimits( limit1, limit2 ) 
    
    
    def _placementCallback(  self, me,  orient,  pos ):
        bone = me.getUserData() 
        doll = bone.getDoll() 
    
        ## is self the root bone?
        if (not bone.getParent()):
            finalorient = (orient * bone.getOffsetOrient()) 
            finalpos = pos + (orient * bone.getOffsetPos()) 
            doll.self.Node.setPosition( finalpos ) 
            doll.self.Node.setOrientation( finalorient ) 
        else:
            ## standard bone, calculate the local orientation between it and it's parent.
            parentpos, parentorient = bone.getParent().getBody().getPositionOrientation(  ) 
            localorient = parentorient.Inverse() * orient 
            bone.getOgreBone().setOrientation( localorient ) 
            

    def inheritVelOmega( self, vel, omega ):
        ## find main position.
        mainpos = self.Node.getWorldPosition() 
        it = self.Bones.begin()
        while it:
        ## for (RagBoneListIterator it = self.Bones.begin()  it != self.Bones.end(); it++)
            pos, orient = it.getBody().getPositionOrientation(  ) 
            it.getBody().setVelocity( vel + omega.crossProduct( pos - mainpos ) ) 
    
