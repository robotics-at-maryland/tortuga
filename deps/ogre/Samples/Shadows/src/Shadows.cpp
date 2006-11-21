/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

/**
    \file 
        Shadows.cpp
    \brief
        Shows a few ways to use Ogre's shadowing techniques
*/

#include "ExampleApplication.h"

/*
#include "OgreNoMemoryMacros.h"
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>
#include "OgreMemoryMacros.h"
*/

/*
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "OgreNoMemoryMacros.h"
#include <crtdbg.h>
#endi*/

Entity* mAthene;
AnimationState* mAnimState = 0;
Entity* pPlaneEnt;
std::vector<Entity*> pColumns;
Light* mLight;
Light* mSunLight;
SceneNode* mLightNode = 0;
AnimationState* mLightAnimationState = 0;
ColourValue mMinLightColour(0.3, 0.0, 0);
ColourValue mMaxLightColour(0.5, 0.3, 0.1);
Real mMinFlareSize = 40;
Real mMaxFlareSize = 80;
/// Plane that defines plane-optimal shadow mapping basis
MovablePlane posmPlane("Plane");

#define NUM_ATHENE_MATERIALS 2
String mAtheneMaterials[NUM_ATHENE_MATERIALS] = 
{
    "Examples/Athene/NormalMapped",
    "Examples/Athene/Basic"
};
#define NUM_SHADOW_TECH 8
String mShadowTechDescriptions[NUM_SHADOW_TECH] = 
{
    "Stencil Shadows (Additive)",
    "Stencil Shadows (Modulative)",
	"Texture Shadows (Additive)",
    "Texture Shadows (Modulative)",
	"Texture Shadows (Soft Modulative)",
	"Texture Shadows (Additive + Plane Optimal)",
	"Custom Shadowmapping (Additive + Plane Optimal)",
    "None"
};
enum DemoShadowTech
{
	STENCIL_ADDITIVE = 0,
	STENCIL_MODULATIVE = 1,
	TEXTURE_ADDITIVE = 2,
	TEXTURE_MODULATIVE = 3,
	TEXTURE_SOFT_MODULATIVE = 4,
	TEXTURE_ADDITIVE_PLANE_OPTIMAL = 5,
	CUSTOM_DEPTH_SHADOWMAPPING = 6,
	NONE = 7
};
bool mShadowTechSupported[NUM_SHADOW_TECH];

int mCurrentAtheneMaterial;
int mCurrentShadowTechnique = 0;
String SHADOW_COMPOSITOR_NAME("Gaussian Blur");
String CUSTOM_ROCKWALL_MATERIAL("Ogre/CustomShadows/SimpleRock");	
String BASIC_ROCKWALL_MATERIAL("Examples/Rockwall");


OverlayElement* mShadowTechniqueInfo;
OverlayElement* mMaterialInfo;
OverlayElement* mInfo;


/** This class 'wibbles' the light and billboard */
class LightWibbler : public ControllerValue<Real>
{
protected:
    Light* mLight;
    Billboard* mBillboard;
    ColourValue mColourRange;
    ColourValue mMinColour;
    Real mMinSize;
    Real mSizeRange;
    Real intensity;
public:
    LightWibbler(Light* light, Billboard* billboard, const ColourValue& minColour, 
        const ColourValue& maxColour, Real minSize, Real maxSize)
    {
        mLight = light;
        mBillboard = billboard;
        mMinColour = minColour;
        mColourRange.r = maxColour.r - minColour.r;
        mColourRange.g = maxColour.g - minColour.g;
        mColourRange.b = maxColour.b - minColour.b;
        mMinSize = minSize;
        mSizeRange = maxSize - minSize;
    }

    virtual Real  getValue (void) const
    {
        return intensity;
    }

    virtual void  setValue (Real value)
    {
        intensity = value;

        ColourValue newColour;

        // Attenuate the brightness of the light
        newColour.r = mMinColour.r + (mColourRange.r * intensity);
        newColour.g = mMinColour.g + (mColourRange.g * intensity);
        newColour.b = mMinColour.b + (mColourRange.b * intensity);

        mLight->setDiffuseColour(newColour);
        mBillboard->setColour(newColour);
        // set billboard size
        Real newSize = mMinSize + (intensity * mSizeRange);
        mBillboard->setDimensions(newSize, newSize);

    }
};

Real timeDelay = 0;
#define KEY_PRESSED(_key,_timeDelay, _macro) \
{ \
    if (mKeyboard->isKeyDown(_key) && timeDelay <= 0) \
{ \
    timeDelay = _timeDelay; \
    _macro ; \
} \
}


//---------------------------------------------------------------------------
class GaussianListener: public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	GaussianListener() {}
	virtual ~GaussianListener() {}
	void notifyViewportSize(int width, int height)
	{
		mVpWidth = width;
		mVpHeight = height;
		// Calculate gaussian texture offsets & weights
		float deviation = 3.0f;
		float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

		// central sample, no offset
		mBloomTexOffsetsHorz[0][0] = 0.0f;
		mBloomTexOffsetsHorz[0][1] = 0.0f;
		mBloomTexOffsetsVert[0][0] = 0.0f;
		mBloomTexOffsetsVert[0][1] = 0.0f;
		mBloomTexWeights[0][0] = mBloomTexWeights[0][1] = 
			mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
		mBloomTexWeights[0][3] = 1.0f;

		// 'pre' samples
		for(int i = 1; i < 8; ++i)
		{
			mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = 
				mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
			mBloomTexWeights[i][3] = 1.0f;
			mBloomTexOffsetsHorz[i][0] = i * texelSize;
			mBloomTexOffsetsHorz[i][1] = 0.0f;
			mBloomTexOffsetsVert[i][0] = 0.0f;
			mBloomTexOffsetsVert[i][1] = i * texelSize;
		}
		// 'post' samples
		for(int i = 8; i < 15; ++i)
		{
			mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = 
				mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
			mBloomTexWeights[i][3] = 1.0f;

			mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
			mBloomTexOffsetsHorz[i][1] = 0.0f;
			mBloomTexOffsetsVert[i][0] = 0.0f;
			mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
		}

	}
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		// Prepare the fragment params offsets
		switch(pass_id)
		{
		case 701: // blur horz
			{
				// horizontal bloom
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams = 
					mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				// A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
				if (progName.find("GLSL") != Ogre::String::npos)
				{
					fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
					fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);
				}
				else
				{
					fparams->setNamedConstant("sampleOffsets[0]", mBloomTexOffsetsHorz[0], 15);
					fparams->setNamedConstant("sampleWeights[0]", mBloomTexWeights[0], 15);
				}

				break;
			}
		case 700: // blur vert
			{
				// vertical bloom 
				mat->load();
				Ogre::GpuProgramParametersSharedPtr fparams = 
					mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
				const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
				// A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
				if (progName.find("GLSL") != Ogre::String::npos)
				{
					fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
					fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);
				}
				else
				{
					fparams->setNamedConstant("sampleOffsets[0]", mBloomTexOffsetsVert[0], 15);
					fparams->setNamedConstant("sampleWeights[0]", mBloomTexWeights[0], 15);
				}

				break;
			}
		}

	}
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{

	}
};
GaussianListener gaussianListener;


class ShadowsListener : public ExampleFrameListener
{
protected:
    SceneManager* mSceneMgr;
	Viewport *mShadowVp;
	CompositorInstance* mShadowCompositor;
public:
    ShadowsListener(RenderWindow* win, Camera* cam, SceneManager* sm)
        : ExampleFrameListener(win, cam), mSceneMgr(sm), mShadowVp(0), 
		mShadowCompositor(0)
    {
    }

	void configureShadowTechnique(int preTechnique, int currentTechnique)
	{
		switch(currentTechnique)
		{
		case STENCIL_ADDITIVE:
			mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
			break;
		case STENCIL_MODULATIVE:
			mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
			break;
		case TEXTURE_ADDITIVE:
		case TEXTURE_ADDITIVE_PLANE_OPTIMAL:
		case CUSTOM_DEPTH_SHADOWMAPPING:
			mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
			break;
		case TEXTURE_MODULATIVE:
		case TEXTURE_SOFT_MODULATIVE:
			mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
			break;
		case NONE:
			mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
			break;

		}

	}

	void configureLights(int preTechnique, int currentTechnique)
	{
		Vector3 dir;
		switch (currentTechnique)
		{
		case STENCIL_ADDITIVE:
			// Fixed light, dim
			mSunLight->setCastShadows(true);

			// Point light, movable, reddish
			mLight->setType(Light::LT_POINT);
			mLight->setCastShadows(true);
			mLight->setDiffuseColour(mMinLightColour);
			mLight->setSpecularColour(1, 1, 1);
			mLight->setAttenuation(8000,1,0.0005,0);

			break;
		case STENCIL_MODULATIVE:
			// Multiple lights cause obvious silhouette edges in modulative mode
			// So turn off shadows on the direct light
			// Fixed light, dim
			mSunLight->setCastShadows(false);

			// Point light, movable, reddish
			mLight->setType(Light::LT_POINT);
			mLight->setCastShadows(true);
			mLight->setDiffuseColour(mMinLightColour);
			mLight->setSpecularColour(1, 1, 1);
			mLight->setAttenuation(8000,1,0.0005,0);
			break;
		case TEXTURE_SOFT_MODULATIVE:
		case TEXTURE_MODULATIVE:
		case TEXTURE_ADDITIVE:
			// Fixed light, dim
			mSunLight->setCastShadows(currentTechnique != TEXTURE_SOFT_MODULATIVE);

			// Change moving light to spotlight
			// Point light, movable, reddish
			mLight->setType(Light::LT_SPOTLIGHT);
			mLight->setDirection(Vector3::NEGATIVE_UNIT_Z);
			mLight->setCastShadows(true);
			mLight->setDiffuseColour(mMinLightColour);
			mLight->setSpecularColour(1, 1, 1);
			mLight->setAttenuation(8000,1,0.0005,0);
			mLight->setSpotlightRange(Degree(80),Degree(90));


			break;
		default:
			break;
		};

	}
	void cleanupCompositors(int preTechnique, int currentTechnique)
	{
		if (preTechnique == TEXTURE_SOFT_MODULATIVE && 
			preTechnique != currentTechnique)
		{
			// Clean up compositors
			mShadowCompositor->removeListener(&gaussianListener);
			CompositorManager::getSingleton().setCompositorEnabled(mShadowVp, 
				SHADOW_COMPOSITOR_NAME, false);
			// Remove entire compositor chain
			CompositorManager::getSingleton().removeCompositorChain(mShadowVp);
			mShadowVp = 0;
			mShadowCompositor = 0;
		}


	}
	void configureCompositors(int preTechnique, int currentTechnique)
	{
		RenderTarget* shadowRtt;
		TexturePtr shadowTex;
		switch(currentTechnique)
		{
		case TEXTURE_SOFT_MODULATIVE:
			// set up compositors
			shadowTex = mSceneMgr->getShadowTexture(0);
			shadowRtt = shadowTex->getBuffer()->getRenderTarget();
			mShadowVp = shadowRtt->getViewport(0);
			mShadowCompositor = 
				CompositorManager::getSingleton().addCompositor(mShadowVp, SHADOW_COMPOSITOR_NAME);
			CompositorManager::getSingleton().setCompositorEnabled(
				mShadowVp, SHADOW_COMPOSITOR_NAME, true);
			mShadowCompositor->addListener(&gaussianListener);
			gaussianListener.notifyViewportSize(mShadowVp->getActualWidth(), mShadowVp->getActualHeight());

			break;
		default:
			break;

		};

	}
	void configureTextures(int preTechnique, int currentTechnique)
	{
		switch(currentTechnique)
		{
		case CUSTOM_DEPTH_SHADOWMAPPING:
			mSceneMgr->setShadowTexturePixelFormat(PF_FLOAT32_R);
			break;
		default:
			mSceneMgr->setShadowTexturePixelFormat(PF_X8R8G8B8);
		}

	}
	void configureShadowCasterReceiverMaterials(int preTechnique, int currentTechnique)
	{
		switch(currentTechnique)
		{
		case CUSTOM_DEPTH_SHADOWMAPPING:
			mSceneMgr->setShadowTextureCasterMaterial("CustomShadows/ShadowCaster");
			mSceneMgr->setShadowTextureReceiverMaterial("CustomShadows/ShadowReceiver");
			mSceneMgr->setShadowTextureSelfShadow(true);	// NOTE: need to perform depth biasing for this to work
			// Sort out base materials
			pPlaneEnt->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
			for (std::vector<Entity*>::iterator i = pColumns.begin();
				i != pColumns.end(); ++i)
			{
				(*i)->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
			}
			break;

		default:
			mSceneMgr->setShadowTextureCasterMaterial(StringUtil::BLANK);
			mSceneMgr->setShadowTextureReceiverMaterial(StringUtil::BLANK);
			mSceneMgr->setShadowTextureSelfShadow(false);	
			// Sort out base materials for additive modes
			pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
			for (std::vector<Entity*>::iterator i = pColumns.begin();
				i != pColumns.end(); ++i)
			{
				(*i)->setMaterialName(BASIC_ROCKWALL_MATERIAL);
			}

		}

	}

	void configureShadowCameras(int preTechnique, int currentTechnique)
	{
		switch(currentTechnique)
		{
		case TEXTURE_ADDITIVE_PLANE_OPTIMAL:
			// Create custom camera setup class
			{
				ShadowCameraSetupPtr planeOptPtr1(new PlaneOptimalShadowCameraSetup(&posmPlane));
				mSunLight->setCustomShadowCameraSetup(planeOptPtr1);
				ShadowCameraSetupPtr planeOptPtr2(new PlaneOptimalShadowCameraSetup(&posmPlane));
				mLight->setCustomShadowCameraSetup(planeOptPtr2);
			}
			break;
		default:
			// Default shadow camera setup
			mSunLight->resetCustomShadowCameraSetup();
			mLight->resetCustomShadowCameraSetup();
			break;

		}

	}

    void changeShadowTechnique()
    {
		int prevTech = mCurrentShadowTechnique;
        mCurrentShadowTechnique = ++mCurrentShadowTechnique % NUM_SHADOW_TECH;
		if (!mShadowTechSupported[mCurrentShadowTechnique])
		{
			// Skip unsupported
			mCurrentShadowTechnique = ++mCurrentShadowTechnique % NUM_SHADOW_TECH;
		}
        mShadowTechniqueInfo->setCaption("Current: " + mShadowTechDescriptions[mCurrentShadowTechnique]);

		cleanupCompositors(prevTech, mCurrentShadowTechnique);

		configureShadowTechnique(prevTech, mCurrentShadowTechnique);

		if (mCurrentShadowTechnique != NONE)
		{
			configureShadowCameras(prevTech, mCurrentShadowTechnique);
			configureLights(prevTech, mCurrentShadowTechnique);
			configureCompositors(prevTech, mCurrentShadowTechnique);
			configureTextures(prevTech, mCurrentShadowTechnique);
			configureShadowCasterReceiverMaterials(prevTech, mCurrentShadowTechnique);
		}

    }

    void changeAtheneMaterial()
    {
        mCurrentAtheneMaterial = ++mCurrentAtheneMaterial % NUM_ATHENE_MATERIALS;
        mMaterialInfo->setCaption("Current: " + mAtheneMaterials[mCurrentAtheneMaterial]);
        mAthene->setMaterialName(mAtheneMaterials[mCurrentAtheneMaterial]);
    }

    bool frameEnded(const FrameEvent& evt)
    {
	using namespace OIS;

        if (timeDelay >= 0) 
            timeDelay -= evt.timeSinceLastFrame;

        if (mAnimState)
            mAnimState->addTime(evt.timeSinceLastFrame);

        KEY_PRESSED(KC_O, 1, changeShadowTechnique());
        KEY_PRESSED(KC_M, 0.5, changeAtheneMaterial());

        return ExampleFrameListener::frameStarted(evt) && ExampleFrameListener::frameEnded(evt);        
    }

    


};

class ShadowsApplication : public ExampleApplication
{
public:
    ShadowsApplication() {


    }

    ~ShadowsApplication() 
    {
    }
protected:

	// Override this to ensure FPU mode
	bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
		{
			// Custom option - to use PlaneOptimalShadowCameraSetup we must have
			// double-precision. Thus, set the D3D floating point mode if present, 
			// no matter what was chosen
			ConfigOptionMap& optMap = mRoot->getRenderSystem()->getConfigOptions();
			ConfigOptionMap::iterator i = optMap.find("Floating-point mode");
			if (i != optMap.end())
			{
				if (i->second.currentValue != "Consistent")
				{
					i->second.currentValue = "Consistent";
					LogManager::getSingleton().logMessage("Demo_Shadows: overriding "
						"D3D floating point mode to 'Consistent' to ensure precision "
						"for plane-optimal camera setup option");

				}
			}

			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			mWindow = mRoot->initialise(true);
			return true;
		}
		else
		{
			return false;
		}
	}

	void generalSceneSetup()
    {
        // do this first so we generate edge lists
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

		// default to all techs supported
		for (int i = 0; i < NUM_SHADOW_TECH; ++i)
			mShadowTechSupported[i] = true;

        // Set the default Athene material
        // We'll default it to the normal map for ps_2_0 capable hardware
        // everyone else will default to the basic
        if (GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") ||
            GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1"))
        {
            mCurrentAtheneMaterial = 0;
        }
        else
        {
            mCurrentAtheneMaterial = 1;
			// no SM2
			mShadowTechSupported[CUSTOM_DEPTH_SHADOWMAPPING] = false;
			mShadowTechSupported[TEXTURE_SOFT_MODULATIVE] = false;
        }

		if (!mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_TEXTURE_FLOAT))
		{
			mShadowTechSupported[CUSTOM_DEPTH_SHADOWMAPPING] = false;
		}

        // Set ambient light off
        mSceneMgr->setAmbientLight(ColourValue(0.0, 0.0, 0.0));

        // Fixed light, dim
        mSunLight = mSceneMgr->createLight("SunLight");
        mSunLight->setType(Light::LT_SPOTLIGHT);
        mSunLight->setPosition(1000,1250,500);
        mSunLight->setSpotlightRange(Degree(30), Degree(50));
        Vector3 dir;
        dir = -mSunLight->getPosition();
        dir.normalise();
        mSunLight->setDirection(dir);
        mSunLight->setDiffuseColour(0.35, 0.35, 0.38);
        mSunLight->setSpecularColour(0.9, 0.9, 1);

        // Point light, movable, reddish
        mLight = mSceneMgr->createLight("Light2");
        mLight->setDiffuseColour(mMinLightColour);
        mLight->setSpecularColour(1, 1, 1);
        mLight->setAttenuation(8000,1,0.0005,0);

        // Create light node
        mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "MovingLightNode");
        mLightNode->attachObject(mLight);
        // create billboard set
        BillboardSet* bbs = mSceneMgr->createBillboardSet("lightbbs", 1);
        bbs->setMaterialName("Examples/Flare");
        Billboard* bb = bbs->createBillboard(0,0,0,mMinLightColour);
        // attach
        mLightNode->attachObject(bbs);

        // create controller, after this is will get updated on its own
        ControllerFunctionRealPtr func = ControllerFunctionRealPtr(
            new WaveformControllerFunction(Ogre::WFT_SINE, 0.75, 0.5));
        ControllerManager& contMgr = ControllerManager::getSingleton();
        ControllerValueRealPtr val = ControllerValueRealPtr(
            new LightWibbler(mLight, bb, mMinLightColour, mMaxLightColour, 
            mMinFlareSize, mMaxFlareSize));
        Controller<Real>* controller = contMgr.createController(
            contMgr.getFrameTimeSource(), val, func);

        //mLight->setPosition(Vector3(300,250,-300));
        mLightNode->setPosition(Vector3(300,250,-300));


        // Create a track for the light
        Animation* anim = mSceneMgr->createAnimation("LightTrack", 20);
        // Spline it for nice curves
        anim->setInterpolationMode(Animation::IM_SPLINE);
        // Create a track to animate the camera's node
        NodeAnimationTrack* track = anim->createNodeTrack(0, mLightNode);
        // Setup keyframes
        TransformKeyFrame* key = track->createNodeKeyFrame(0); // A startposition
        key->setTranslate(Vector3(300,250,-300));
        key = track->createNodeKeyFrame(2);//B
        key->setTranslate(Vector3(150,300,-250));
        key = track->createNodeKeyFrame(4);//C
        key->setTranslate(Vector3(-150,350,-100));
        key = track->createNodeKeyFrame(6);//D
        key->setTranslate(Vector3(-400,200,-200));
        key = track->createNodeKeyFrame(8);//E
        key->setTranslate(Vector3(-200,200,-400));
        key = track->createNodeKeyFrame(10);//F
        key->setTranslate(Vector3(-100,150,-200));
        key = track->createNodeKeyFrame(12);//G
        key->setTranslate(Vector3(-100,75,180));
        key = track->createNodeKeyFrame(14);//H
        key->setTranslate(Vector3(0,250,300));
        key = track->createNodeKeyFrame(16);//I
        key->setTranslate(Vector3(100,350,100));
        key = track->createNodeKeyFrame(18);//J
        key->setTranslate(Vector3(250,300,0));
        key = track->createNodeKeyFrame(20);//K == A
        key->setTranslate(Vector3(300,250,-300));
        // Create a new animation state to track this
        mAnimState = mSceneMgr->createAnimationState("LightTrack");
        mAnimState->setEnabled(true);
        // Make light node look at origin, this is for when we
        // change the moving light to a spotlight
        mLightNode->setAutoTracking(true, mSceneMgr->getRootSceneNode());

        // Prepare athene mesh for normalmapping
        MeshPtr pAthene = MeshManager::getSingleton().load("athene.mesh", 
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        unsigned short src, dest;
        if (!pAthene->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
        {
            pAthene->buildTangentVectors(VES_TANGENT, src, dest);
        }

        SceneNode* node;
        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mAthene = mSceneMgr->createEntity( "athene", "athene.mesh" );
        mAthene->setMaterialName(mAtheneMaterials[mCurrentAtheneMaterial]);
        node->attachObject( mAthene );
        node->translate(0,-20, 0);
        node->yaw(Degree(90));

        Entity* pEnt;

        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        pEnt = mSceneMgr->createEntity( "col1", "column.mesh" );
        pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
		pColumns.push_back(pEnt);
        node->attachObject( pEnt );
        node->translate(200,0, -200);

        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        pEnt = mSceneMgr->createEntity( "col2", "column.mesh" );
        pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
		pColumns.push_back(pEnt);
        node->attachObject( pEnt );
        node->translate(200,0, 200);

        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        pEnt = mSceneMgr->createEntity( "col3", "column.mesh" );
		pColumns.push_back(pEnt);
        pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
        node->attachObject( pEnt );
        node->translate(-200,0, -200);

        node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        pEnt = mSceneMgr->createEntity( "col4", "column.mesh" );
		pColumns.push_back(pEnt);
        pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
        node->attachObject( pEnt );
        node->translate(-200,0, 200);
        // Skybox
        mSceneMgr->setSkyBox(true, "Examples/StormySkyBox");

        // Floor plane (use POSM plane def)
        posmPlane.normal = Vector3::UNIT_Y;
        posmPlane.d = 100;
        MeshManager::getSingleton().createPlane("Myplane",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, posmPlane,
            1500,1500,50,50,true,1,5,5,Vector3::UNIT_Z);
        pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
        pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
        pPlaneEnt->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

        // show overlay
        Overlay* pOver = OverlayManager::getSingleton().getByName("Example/ShadowsOverlay");    
        mShadowTechniqueInfo = OverlayManager::getSingleton().getOverlayElement("Example/Shadows/ShadowTechniqueInfo");
        mMaterialInfo = OverlayManager::getSingleton().getOverlayElement("Example/Shadows/MaterialInfo");
        mInfo = OverlayManager::getSingleton().getOverlayElement("Example/Shadows/Info");

        mShadowTechniqueInfo->setCaption("Current: " + mShadowTechDescriptions[mCurrentShadowTechnique]);
        mMaterialInfo->setCaption("Current: " + mAtheneMaterials[mCurrentAtheneMaterial]);
        pOver->show();

		if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
        {
            // In D3D, use a 1024x1024 shadow texture
            mSceneMgr->setShadowTextureSettings(1024, 2);
        }
        else
        {
            // Use 512x512 texture in GL since we can't go higher than the window res
            mSceneMgr->setShadowTextureSettings(512, 2);
        }
        mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));

        // incase infinite far distance is not supported
        mCamera->setFarClipDistance(100000);

        //mSceneMgr->setShowDebugShadows(true);

		const RenderSystemCapabilities* caps = Root::getSingleton().getRenderSystem()->getCapabilities();
		if (!caps->hasCapability(RSC_VERTEX_PROGRAM) || !(caps->hasCapability(RSC_FRAGMENT_PROGRAM)))
		{
			mShadowTechSupported[TEXTURE_SOFT_MODULATIVE] = false;
			mShadowTechSupported[CUSTOM_DEPTH_SHADOWMAPPING] = false;
		}
		else
		{
			if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
				!GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") 
				)
			{
				mShadowTechSupported[TEXTURE_SOFT_MODULATIVE] = false;
				mShadowTechSupported[CUSTOM_DEPTH_SHADOWMAPPING] = false;
			}
		}



    }


    // Just override the mandatory create scene method
    void createScene(void)
    {
        // set up general scene (this defaults to additive stencils)
        generalSceneSetup();
    }
    // Create new frame listener
    void createFrameListener(void)
    {
        mFrameListener= new ShadowsListener(mWindow, mCamera, mSceneMgr);
        mFrameListener->showDebugOverlay(true);
        mRoot->addFrameListener(mFrameListener);

    }


public:
    void go(void)
    {
        if (!setup())
            return;

        mRoot->startRendering();
    }


};



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
    // Create application object
    ShadowsApplication app;

    SET_TERM_HANDLER;
    
    try {
        app.go();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " <<
            e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
