#I think it is possible to create code that will do that automaticly.

def aliases( version ):
    if version == "CVS":
        return {
            "map<unsigned, Ogre::StaticGeometry::Region*, std::less<unsigned>, std::allocator<std::pair<unsigned const, Ogre::StaticGeometry::Region*> > >"
              : "MapOfRegionIterator"

            , "map<std::string, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >, std::less<std::string>, std::allocator<std::pair<std::string const, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > > > >"
              : "MapOfStringToMultiMapFromStringToString"

            , "multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
              : "MultiMapStringToString"
            , "MapIterator<std::map<std::string, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >, std::less<std::string>, std::allocator<std::pair<std::string const, std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > > > > >"
                : "MapIteratorStringToMultiMapFromStringToString"
            ,  "map<std::string, bool, std::less<std::string>, std::allocator<std::pair<std::string const, bool> > >"
              : "MapOfStringToBool"
            , "map<std::string, Ogre::AnimationState*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::AnimationState*> > >" 
              :  "MapOfAnimationState"
            , "map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > >"
              : "MapOfStringToOverlayElement"
            , "map<std::string, Ogre::MovableObject*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObject*> > >"
              : "MapOfStringToMovableObject"
            , "map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
              : "MapOfStringToString"
            , "map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
              : "NamedValuePairList"
            , "vector<float, std::allocator<float> >"
              : "VectorOfFloat"
            , "vector<std::string, std::allocator<std::string> >"
              : "VectorOfString"
            , "pair<int, Ogre::CompositorInstance::RenderSystemOperation*>"
              : "PairOfIntAndRenderSystemOperationPtr"
              
              
             , "list<Ogre::Particle*, std::allocator<Ogre::Particle*> >"
               : "ListOfParticle"
             , "set<unsigned char, std::less<unsigned char>, std::allocator<unsigned char> >"
               : "SetOfUnsignedShort"
             , "vector<std::pair<std::string, std::string>, std::allocator<std::pair<std::string, std::string> > >"
               : "VectorStringPairs"

              
            , "list<Ogre::Plane, std::allocator<Ogre::Plane> >"
              : "ListOfPlane"
            , "set<Ogre::SceneQuery::WorldFragmentType, std::less<Ogre::SceneQuery::WorldFragmentType>, std::allocator<Ogre::SceneQuery::WorldFragmentType> >"
              : "SetOfWorldFragmentTypes"
            , "set<std::string, std::less<std::string>, std::allocator<std::string> >"
              : "SetOfStrings"
            , "map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > >"
              : "MapOfOverlayElement"
            , "MapIterator<std::map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > > >"
              : "MapIteratorOfOverlayElement"
            , "Controller<float>"
              : "FloatController"
            , "ControllerFunction<float>"
              : "FloatControllerFunction"
            , "ControllerValue<float>"
              : "FloatControllerValue"
            , "TRect<float>"
              : "FloatRect"
            , "map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,Ogre::_ConfigOption,std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Ogre::_ConfigOption> > >"
              : "MapOfStringToConfigOption"
            , "vector<Ogre::StaticGeometry::SubMeshLodGeometryLink,std::allocator<Ogre::StaticGeometry::SubMeshLodGeometryLink> >"
              : "VectorOfSubMeshLodGeometryLink"
            , "vector<Ogre::HardwareBuffer::Usage,std::allocator<Ogre::HardwareBuffer::Usage> >"
              : "VectorOfHardwareBufferUsage"
            , "vector<const Ogre::Image*,std::allocator<const Ogre::Image*> >"
              : "VectorOfImagePtr"
              
              ### Updates to fix UGLY classes
              
                ,"VectorIterator<std::vector<Ogre::RenderQueueInvocation*, std::allocator<Ogre::RenderQueueInvocation*> > >"
                : "VectorIterRenderQueue"
                ,"AlignedAllocator<Ogre::Vector4, 0>"
                : "AlignedAllocatorVector4"
                

                ,"RadixSort<std::vector<Ogre::RenderablePass, std::allocator<Ogre::RenderablePass> >, Ogre::RenderablePass, float>"
                : "RadixSortRenderablePassFloat"
                ,"RadixSort<std::vector<Ogre::RenderablePass, std::allocator<Ogre::RenderablePass> >, Ogre::RenderablePass, unsigned>"
                : "RadixSortRenderablePassUInt"
                ,"RadixSort<std::list<Ogre::Particle*, std::allocator<Ogre::Particle*> >, Ogre::Particle*, float>"
                : "RadixSortParticleFloat"
                ,"RadixSort<std::list<Ogre::Billboard*, std::allocator<Ogre::Billboard*> >, Ogre::Billboard*, float>"
                : "RadixSortBillboardFloat"
                ,"Singleton<Ogre::ArchiveManager>"
                : "SingletonArchiveManager"
                ,"Singleton<Ogre::CompositorManager>"
                : "SingletonCompositorManager"
                ,"Singleton<Ogre::SkeletonManager>"
                : "SingletonSkeletonManager"
                ,"Singleton<Ogre::Root>"
                : "SingletonRoot"
                ,"Singleton<Ogre::SceneManagerEnumerator>"
                : "SingletonSceneManagerEnumerator"
                ,"Singleton<Ogre::ShadowTextureManager>"
                : "SingletonShadowTextureManager"
                ,"Singleton<Ogre::ResourceBackgroundQueue>"
                : "SingletonResourceBackgroundQueue"
                ,"Singleton<Ogre::TextureManager>"
                : "SingletonTextureManager"
                ,"Singleton<Ogre::Profiler>"
                : "SingletonProfiler"
                ,"Singleton<Ogre::ParticleSystemManager>"
                : "SingletonParticleSystemManager"
                ,"Singleton<Ogre::OverlayManager>"
                : "SingletonOverlayManager"
                ,"Singleton<Ogre::MeshManager>"
                : "SingletonMeshManager"
                ,"Singleton<Ogre::MaterialManager>"
                : "SingletonMaterialManager"
                ,"Singleton<Ogre::LogManager>"
                : "SingletonLogManager"
                ,"Singleton<Ogre::HighLevelGpuProgramManager>"
                : "SingletonHighLevelGpuProgramManager"
                ,"Singleton<Ogre::GpuProgramManager>"
                : "SingletonGpuProgramManager"
                ,"Singleton<Ogre::HardwareBufferManager>"
                : "SingletonHardwareBufferManager"
                ,"Singleton<Ogre::ControllerManager>"
                : "SingletonControllerManager"
                ,"Singleton<Ogre::ResourceGroupManager>"
                : "SingletonResourceGroupManager"
                ,"SharedPtr<std::vector<Ogre::FileInfo, std::allocator<Ogre::FileInfo> > >"
                : "SharedPtrFileInfo"
                ,"SharedPtr<Ogre::MemoryDataStream>"
                : "SharedPtrMemoryDataStream"
                ,"SharedPtr<std::list<Ogre::SharedPtr<Ogre::DataStream>, std::allocator<Ogre::SharedPtr<Ogre::DataStream> > > >"
                : "SharedPtrListDataStream"
                
                ,"ConstMapIterator<stdext::hash_map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Ogre::Node*, stdext::hash_compare<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Ogre::Node*> > > >"
                : "ConstMapIteratorBasicStringNode"
                ,"ConstVectorIterator<std::vector<Ogre::Node*, std::allocator<Ogre::Node*> > >"
                : "ConstMapIteratorVectorNode"
                ,"ConstVectorIterator<std::vector<Ogre::TextureUnitState*, std::allocator<Ogre::TextureUnitState*> > >"
                : "ConstMapIteratorVectorTectUnitState"
                ,"ConstVectorIterator<std::vector<Ogre::GpuProgramParameters::AutoConstantEntry, std::allocator<Ogre::GpuProgramParameters::AutoConstantEntry> > >"
                : "ConstMapIteratorVectorGpuProgramParametersAutoConstantEntry"
                ,"ConstVectorIterator<std::vector<Ogre::GpuProgramParameters::IntConstantEntry, std::allocator<Ogre::GpuProgramParameters::IntConstantEntry> > >"
                : "ConstMapIteratorVectorGpuProgramParametersIntConstantEntry"
                ,"ConstVectorIterator<std::vector<float, std::allocator<float> > >"
                : "ConstMapIteratorVectorFloat"
                ,"ConstVectorIterator<std::vector<Ogre::VertexPoseKeyFrame::PoseRef, std::allocator<Ogre::VertexPoseKeyFrame::PoseRef> > >"
                : "ConstMapIteratorVectorVertexPoseKeyFrame"
                 ,"MapIterator<std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > >"
                : "MapIteratorBasicStringAllocator"
                  ,"MapIterator<stdext::hash_map<std::string, Ogre::MovableObject*, stdext::hash_compare<std::string, std::less<std::string> >, std::allocator<std::pair<std::string const, Ogre::MovableObject*> > > >"
                  : "MapIteratorStringHashMovableObjectPtr"
                 ,"MapIterator<std::map<std::string, Ogre::MovableObject*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObject*> > > >"
                  : "MapIteratorStringMovableObjectPtr"
                    ,"MapIterator<std::map<std::string, Ogre::ParticleAffectorFactory*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::ParticleAffectorFactory*> > > >"
                 : "MapIteratorStringParticleAffectorFactoryPtr"
                ,"MapIterator<std::map<std::string, Ogre::MovableObjectFactory*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObjectFactory*> > > >"
                  : "MapIteratorStringMovableObjectFactoryPtr"
                ,"MapIterator<std::map<std::string, Ogre::OverlayContainer*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayContainer*> > > >"
                : "MapIteratorBasicStringOverlayContainer"
                ,"MapIterator<std::map<unsigned char, Ogre::RenderQueueGroup*, std::less<unsigned char>, std::allocator<std::pair<unsigned char const, Ogre::RenderQueueGroup*> > > >"
                : "MapIteratorRenderQueueGroupPtr"
                ,"MapIterator<std::map<unsigned short, Ogre::VertexAnimationTrack*, std::less<unsigned short>, std::allocator<std::pair<unsigned short const, Ogre::VertexAnimationTrack*> > > >"
                : "MapIteratorVertexAnimationTrackPtr"
                ,"MapIterator<std::map<unsigned short, Ogre::NumericAnimationTrack*, std::less<unsigned short>, std::allocator<std::pair<unsigned short const, Ogre::NumericAnimationTrack*> > > >"
                : "MapIteratorNumericAnimationTrackPtr"
                ,"MapIterator<std::map<unsigned short, Ogre::NodeAnimationTrack*, std::less<unsigned short>, std::allocator<std::pair<unsigned short const, Ogre::NodeAnimationTrack*> > > >"
                : "MapIteratorNodeAnimationTrackPtr"
                ,"VectorIterator<std::vector<Ogre::CompositionTargetPass*, std::allocator<Ogre::CompositionTargetPass*> > >"
                : "VectorIteratorCompositionTargetPassPtr"
                ,"VectorIterator<std::vector<Ogre::CompositorInstance*, std::allocator<Ogre::CompositorInstance*> > >"
                : "VectorIteratorCompositorInstancePtr"
                ,"VectorIterator<std::vector<Ogre::IlluminationPass*, std::allocator<Ogre::IlluminationPass*> > >"
                : "VectorIteratorIlluminationPassPtr"
                ,"VectorIterator<std::vector<Ogre::Pass*, std::allocator<Ogre::Pass*> > >"
                : "VectorIteratorPassPtr"
                ,"VectorIterator<std::vector<Ogre::TextureUnitState*, std::allocator<Ogre::TextureUnitState*> > >"
                : "VectorIteratorTextureUnitStatePtr"
                ,"VectorIterator<std::vector<Ogre::Bone*, std::allocator<Ogre::Bone*> > >"
                : "VectorIteratorBonePtr"
                ,"VectorIterator<std::vector<Ogre::Technique*, std::allocator<Ogre::Technique*> > >"
                : "VectorIteratorTechniquePtr"
                ,"VectorIterator<std::vector<Ogre::VertexPoseKeyFrame::PoseRef, std::allocator<Ogre::VertexPoseKeyFrame::PoseRef> > >"
                : "VectorIteratorVertexPoseKeyFrame"
                                
                ,"pair<unsigned, unsigned>"
                : "StdPairUnsignedUnsigned"
                ,"pair<bool, float>"
                : "StdPairBoolFloat"
             , "vector<Ogre::Vector4, std::allocator<Ogre::Vector4> >"
               : "VectorOfVector4"
            , "vector<unsigned short, std::allocator<unsigned short> >"
              : "VectorOfUnsignedShort"
            ,"map<std::string, Ogre::MovableObject*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObject*> > >"
		        : "MovableObjectMapIterator"
               , "map<Ogre::Vector3, unsigned, Ogre::EdgeListBuilder::vectorLess, std::allocator<std::pair<Ogre::Vector3 const, unsigned> > >"
                : "Vector3EdgeMap"
              , "map<unsigned, Ogre::Vector3, std::less<unsigned>, std::allocator<std::pair<unsigned const, Ogre::Vector3> > >"
                : "Vector3Map"    
   
             ,"MapIterator<std::map<std::string, Ogre::MovableObjectFactory*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObjectFactory*> > > >"
             : "MapIteratorMoveableObjectFactoryPtr"

            ## these were generating passes/techniques/instances classes...
            , "vector<Ogre::CompositionPass*, std::allocator<Ogre::CompositionPass*> >"
              : "VectorCompostionPass"
            , "vector<Ogre::Pass*, std::allocator<Ogre::Pass*> >"
              : "VectorPass"
            , "vector<Ogre::Technique*, std::allocator<Ogre::Technique*> >"
              : "VectorTechnique"
            , "vector<Ogre::CompositionTechnique*, std::allocator<Ogre::CompositionTechnique*> >"
              : "VectorCompositionTechnique"
            , "vector<Ogre::CompositorInstance*, std::allocator<Ogre::CompositorInstance*> >"
              : "VectorCompositorInstance"
            , "map<std::string, Ogre::SceneManager*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::SceneManager*> > >"
              : "MapSceneManagerInstances"
               
              
        }
    else: #1.2
        return {
          "map<std::string, bool, std::less<std::string>, std::allocator<std::pair<std::string const, bool> > >"
          : "MapOfStringToBool"
        , "map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > >"
          : "MapOfStringToOverlayElement"
        , "map<std::string, Ogre::MovableObject*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::MovableObject*> > >"
          : "MapOfStringToMovableObject"
        , "map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
          : "MapOfStringToString"
        , "map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >"
          : "NamedValuePairList"
        , "vector<float, std::allocator<float> >"
          : "VectorOfFloat"
        , "vector<std::string, std::allocator<std::string> >"
          : "VectorOfString"
        , "pair<int, Ogre::CompositorInstance::RenderSystemOperation*>"
          : "PairOfIntAndRenderSystemOperationPtr"
        , "list<Ogre::Plane, std::allocator<Ogre::Plane> >"
          : "ListOfPlane"
        , "set<Ogre::SceneQuery::WorldFragmentType, std::less<Ogre::SceneQuery::WorldFragmentType>, std::allocator<Ogre::SceneQuery::WorldFragmentType> >"
          : "SetOfWorldFragmentTypes"
        , "set<std::string, std::less<std::string>, std::allocator<std::string> >"
          : "SetOfStrings"
        , "map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > >"
          : "MapOfOverlayElement"
        , "MapIterator<std::map<std::string, Ogre::OverlayElement*, std::less<std::string>, std::allocator<std::pair<std::string const, Ogre::OverlayElement*> > > >"
          : "MapIteratorOfOverlayElement"
        , "Controller<float>"
          : "FloatController"
        , "ControllerFunction<float>"
          : "FloatControllerFunction"
        , "ControllerValue<float>"
          : "FloatControllerValue"
        , "TRect<float>"
          : "FloatRect"
        , "map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,Ogre::_ConfigOption,std::less<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >,std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, Ogre::_ConfigOption> > >"
          : "MapOfStringToConfigOption"
        , "vector<Ogre::StaticGeometry::SubMeshLodGeometryLink,std::allocator<Ogre::StaticGeometry::SubMeshLodGeometryLink> >"
          : "VectorOfSubMeshLodGeometryLink"
        , "vector<Ogre::HardwareBuffer::Usage,std::allocator<Ogre::HardwareBuffer::Usage> >"
          : "VectorOfHardwareBufferUsage"
        , "vector<const Ogre::Image*,std::allocator<const Ogre::Image*> >"
          : "VectorOfImagePtr"
        , "set<Ogre::Entity*,std::less<Ogre::Entity*>,std::allocator<Ogre::Entity*> >"
          : "SetOfEntityPtr"
  
    }

def header_files( version ):
    if version == "CVS":
        return [ 'Ogre.h'
                , 'OgreErrorDialog.h'
                , 'OgreConfigDialog.h'
                , 'OgreTagPoint.h'
                , 'OgreTargetManager.h'
                , 'OgreOverlayElementFactory.h'
                , 'OgreArchiveFactory.h'
                , 'OgreParticleSystemRenderer.h'
                , 'OgreParticleEmitterFactory.h'
                , 'OgreParticleAffectorFactory.h'
                , 'OgreSkeletonInstance.h'
                , 'OgreSkeletonManager.h'
                , 'OgreSkeleton.h'
                , 'OgreCompositor.h'
                , 'py_shared_ptr.h'
                ,  'OgreBillboardParticleRenderer.h'
                
            ]
    else: #1.2
        return [ 'Ogre.h'
                , 'OgreErrorDialog.h'
                , 'OgreConfigDialog.h'
                , 'OgreInputEvent.h'
                , 'OgreEventListeners.h'
                , 'OgreTagPoint.h'
                , 'OgreTargetManager.h'
                , 'OgreOverlayElementFactory.h'
                , 'OgreArchiveFactory.h'
                , 'OgreParticleSystemRenderer.h'
                , 'OgreParticleEmitterFactory.h'
                , 'OgreParticleAffectorFactory.h'
                , 'OgreSkeletonInstance.h'
                , 'OgreSkeletonManager.h'
                , 'OgreSkeleton.h'
                , 'OgreCompositor.h'
                , 'OgrePatchMesh.h'
                , 'OgreMesh.h'
                , 'OgreHighLevelGpuProgram.h'
                , 'OgreHardwarePixelBuffer.h'
                , 'OgreEventQueue.h'
                , 'OgreKeyEvent.h'
                , 'OgreHardwareOcclusionQuery.h'
                , 'py_shared_ptr.h'

        ]

def huge_classes( version ):
    if version == "CVS":
        return [ 'RenderSystem'
                , 'StaticGeometry'
                , 'Node'
                , 'Pass'
                , 'BillboardSet'
                , 'ParticleEmitter'
                , 'ParticleSystem'
                , 'SceneManager'
                , 'MovableObject'
                , 'OverlayElement'
                , 'ManualObject'
                , 'RenderTarget'
                , 'Frustum'
                , 'SceneNode'
            ]
    else: #1.2
        return [ 'RenderSystem'
            , 'StaticGeometry'
            , 'Node'
            , 'Pass'
            , 'BillboardSet'
            , 'ParticleEmitter'
            , 'ParticleSystem'
            , 'SceneManager'
            , 'MovableObject'
            , 'OverlayElement'
            , 'ManualObject'
            , 'RenderTarget'
            , 'Frustum'
            , 'SceneNode'
        ]
