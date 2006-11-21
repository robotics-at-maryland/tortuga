INCLUDES = $(STLPORT_CFLAGS) $(FT2_CFLAGS) -I$(top_srcdir)/OgreMain/include -DOGRE_NONCLIENT_BUILD $(ZZIPLIB_CFLAGS) $(DEVIL_CFLAGS) $(FREEIMAGE_CFLAGS) $(PLATFORM_CFLAGS)

noinst_LTLIBRARIES = libSSEsupport.la
libSSEsupport_la_SOURCES = OgreOptimisedUtilSSE.cpp
libSSEsupport_la_CPPFLAGS = -msse

lib_LTLIBRARIES=libOgreMain.la

libOgreMain_la_SOURCES = OgreAlignedAllocator.cpp \
                         OgreAnimation.cpp \
			 OgreAnimable.cpp \
                         OgreAnimationState.cpp \
                         OgreAnimationTrack.cpp \
                         OgreArchiveManager.cpp \
			 OgreAutoParamDataSource.cpp \
                         OgreAxisAlignedBox.cpp \
                         OgreBillboard.cpp \
			 OgreBillboardChain.cpp \
                         OgreBillboardParticleRenderer.cpp \
                         OgreBillboardSet.cpp \
                         OgreBitwise.cpp \
                         OgreBone.cpp \
                         OgreBorderPanelOverlayElement.cpp \
                         OgreCamera.cpp \
                         OgreCodec.cpp \
                         OgreColourValue.cpp \
                         OgreCommon.cpp \
                         OgreConfigFile.cpp \
                         OgreConfigOptionMap.cpp \
                         OgreController.cpp \
                         OgreControllerManager.cpp \
                         OgreDataStream.cpp \
                         OgreDDSCodec.cpp \
                         OgreDefaultHardwareBufferManager.cpp \
			 OgreDefaultSceneQueries.cpp \
                         OgreDynLib.cpp \
                         OgreDynLibManager.cpp \
                         OgreEdgeListBuilder.cpp \
                         OgreEntity.cpp \
                         OgreException.cpp \
			 OgreExternalTextureSource.cpp \
			 OgreExternalTextureSourceManager.cpp \
                         OgreFileSystem.cpp \
                         OgreFont.cpp \
                         OgreFontManager.cpp \
			 OgreFrustum.cpp \
                         OgreGpuProgram.cpp \
                         OgreGpuProgramManager.cpp \
                         OgreGpuProgramUsage.cpp \
			 OgreHardwareBufferManager.cpp \
			 OgreHardwareIndexBuffer.cpp \
			 OgreHardwareOcclusionQuery.cpp \
			 OgreHardwareVertexBuffer.cpp \
			 OgreHardwarePixelBuffer.cpp \
			 OgreHighLevelGpuProgram.cpp \
			 OgreHighLevelGpuProgramManager.cpp \
                         OgreImage.cpp \
                         OgreKeyFrame.cpp \
                         OgreLight.cpp \
                         OgreLog.cpp \
                         OgreLogManager.cpp \
			 OgreManualObject.cpp \
                         OgreMaterial.cpp \
                         OgreMaterialManager.cpp \
                         OgreMaterialSerializer.cpp \
                         OgreMaterialScriptCompiler.cpp \
                         OgreMath.cpp \
                         OgreMatrix3.cpp \
                         OgreMatrix4.cpp \
                         OgreMemoryManager.cpp \
                         OgreMesh.cpp \
                         OgreMeshManager.cpp \
                         OgreMeshSerializer.cpp \
			 OgreMeshSerializerImpl.cpp \
                         OgreMovableObject.cpp \
			 OgreMovablePlane.cpp \
                         OgreNode.cpp \
						 OgreNumerics.cpp \
                         OgreOptimisedUtil.cpp \
                         OgreOptimisedUtilGeneral.cpp \
                         OgreOverlay.cpp \
                         OgreOverlayContainer.cpp \
			 OgreOverlayElement.cpp \
                         OgreOverlayElementCommands.cpp \
                         OgreOverlayManager.cpp \
                         OgrePixelFormat.cpp \
                         OgrePanelOverlayElement.cpp \
                         OgreParticle.cpp \
                         OgreParticleEmitter.cpp \
                         OgreParticleEmitterCommands.cpp \
                         OgreParticleIterator.cpp \
                         OgreParticleSystem.cpp \
                         OgreParticleSystemManager.cpp \
                         OgrePass.cpp \
			 OgrePatchMesh.cpp \
                         OgrePatchSurface.cpp \
                         OgrePlane.cpp \
                         OgrePlatformInformation.cpp \
			 OgrePose.cpp \
                         OgrePredefinedControllers.cpp \
			 OgreProfiler.cpp \
                         OgreProgressiveMesh.cpp \
                         OgreQuaternion.cpp \
			 OgreRectangle2D.cpp \
                         OgreRenderQueue.cpp \
			 OgreRenderQueueInvocation.cpp \
			 OgreRenderQueueSortingGrouping.cpp \
                         OgreRenderSystem.cpp \
                         OgreRenderSystemCapabilities.cpp \
                         OgreRenderTarget.cpp \
                         OgreRenderTexture.cpp \
                         OgreRenderWindow.cpp \
                         OgreResource.cpp \
                         OgreResourceBackgroundQueue.cpp \
                         OgreResourceGroupManager.cpp \
                         OgreResourceManager.cpp \
			 OgreRibbonTrail.cpp \
                         OgreRoot.cpp \
                         OgreRotationSpline.cpp \
                         OgreSceneManager.cpp \
                         OgreSceneManagerEnumerator.cpp \
                         OgreSceneNode.cpp \
                         OgreSceneQuery.cpp \
                         OgreSearchOps.cpp \
                         OgreSerializer.cpp \
						 OgreShadowCameraSetup.cpp \
			 OgreShadowCaster.cpp \
			 OgreShadowTextureManager.cpp \
			 OgreShadowVolumeExtrudeProgram.cpp \
                         OgreSimpleRenderable.cpp \
                         OgreSimpleSpline.cpp \
                         OgreSingleton.cpp \
                         OgreSkeleton.cpp \
                         OgreSkeletonInstance.cpp \
                         OgreSkeletonManager.cpp \
                         OgreSkeletonSerializer.cpp \
			 OgreStaticGeometry.cpp \
                         OgreString.cpp \
                         OgreStringConverter.cpp \
                         OgreStringInterface.cpp \
                         OgreStringVector.cpp \
                         OgreSubEntity.cpp \
                         OgreSubMesh.cpp \
                         OgreTagPoint.cpp \
                         OgreTechnique.cpp \
                         OgreTextAreaOverlayElement.cpp \
                         OgreTexture.cpp \
                         OgreTextureManager.cpp \
                         OgreTextureUnitState.cpp \
                         OgreUserDefinedObject.cpp \
                         OgreVector2.cpp \
                         OgreVector3.cpp \
                         OgreVector4.cpp \
                         OgreVertexIndexData.cpp \
                         OgreViewport.cpp \
                         OgreWireBoundingBox.cpp \
                         OgreZip.cpp \
                         OgreCompositionPass.cpp \
                         OgreCompositionTargetPass.cpp \
                         OgreCompositionTechnique.cpp \
                         OgreCompositor.cpp \
                         OgreCompositorChain.cpp \
                         OgreCompositorInstance.cpp \
                         OgreCompositorManager.cpp \
                         OgreCompositorSerializer.cpp \
                         OgreCompositorScriptCompiler.cpp \
                         OgreCompiler2Pass.cpp \
                         $(OGRE_PLATFORM)/OgreTimer.cpp \
                         $(OGRE_PLATFORM)/OgreConfigDialog.cpp \
                         $(OGRE_PLATFORM)/OgreErrorDialog.cpp \
                         OgreWindowEventUtilities.cpp

if USE_FREEIMAGE
libOgreMain_la_SOURCES += OgreFreeImageCodec.cpp

endif

platformdir = $(libdir)
libOgreMain_la_LIBADD= $(STLPORT_LIBS) $(FT2_LIBS) $(ZZIPLIB_LIBS) $(OGRE_THREAD_LIBS) $(PLATFORM_LIBS) libSSEsupport.la
libOgreMain_la_LDFLAGS = $(SHARED_FLAGS) -release @PACKAGE_VERSION@ -Wl,-rpath,$(platformdir)
#-version-info @OGREMAIN_VERSION_INFO@ -Wl,-rpath,$(platformdir)
#-Wl,--version-script=$(top_srcdir)/OgreMain/src/ogremain.map

