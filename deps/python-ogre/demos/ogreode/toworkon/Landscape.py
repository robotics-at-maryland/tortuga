#include "Landscape.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
    LandscapeApplication app;

    SET_TERM_HANDLER;
    
    try 
	{
		app.go();
    } 
	catch( Ogre::Exception& e )
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( 0, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}

//------------------------------------------------------------------------------------------------
static const String carNames[] = {
	"Jeep",
	"JeepSway",
	"Subaru"
};
static const String carFileNames[] = {
	"jeep.ogreode",
	"jeep.ogreode",
	"subaru.ogreode"
};
static int sSelectedCar = 0;
static int maxNumCar = 3;

//enable this to test it against PLSM2 instead of TSM.
//#define _PLSM2


//------------------------------------------------------------------------------------------------
LandscapeFrameListener::LandscapeFrameListener(RenderWindow* win, Camera* cam,
											   Real time_step,
											   Root *root) : 
	ExampleFrameListener(win, cam),
	_vehicle(0)
{
	// Reduce move speed
    mMoveSpeed = 25;

	// Create something that will step the world automatically
	//_stepper = new OgreOde::ForwardFixedQuickStepper(time_step);
	//_stepper = new OgreOde::ExactVariableQuickStepper(time_step);
	_stepper = new OgreOde::ForwardFixedInterpolatedQuickStepper (time_step);

	_stepper->setAutomatic(OgreOde::Stepper::AutoMode_PostFrame,root);
 

    _ray_query = mCamera->getSceneManager()->createRayQuery(Ray());
	Root::getSingleton().setFrameSmoothingPeriod(5.0f);
	changeCar();

	// Load up our UI and display it
	Overlay* pOver = (Overlay*)OverlayManager::getSingleton().getByName("OgreOdeDemos/Overlay"); 
	OverlayManager::getSingleton().getOverlayElement("OgreOdeDemos/Name")->setCaption(String("Name: ") + "Landscape (Terrain + vehicle + box)");
	OverlayManager::getSingleton().getOverlayElement("OgreOdeDemos/Keys")->setCaption(String("Keys: ") + "I/K - Accelerate/Brake, J/L - Turn, X - Change drive mode, N - Change Car");
	OverlayManager::getSingleton().getOverlayElement("OgreOdeDemos/OtherKeys")->setCaption(String("Extra: ") + "E - Debug Object");
	pOver->show();
}

LandscapeFrameListener::~LandscapeFrameListener()
{
	delete _stepper;
	mCamera->getSceneManager()->destroyQuery(_ray_query);
}

//------------------------------------------------------------------------------------------------
void LandscapeFrameListener::changeCar()
{
	sSelectedCar = (sSelectedCar + 1) % maxNumCar;

	delete _vehicle;
	//_vehicle = new OgreOde_Prefab::Vehicle(carNames[sSelectedCar]);
	//_vehicle->load(carFileNames[sSelectedCar]);
	_vehicle = dotOgreOdeLoader.loadVehicle (carFileNames[sSelectedCar], carNames[sSelectedCar]);
	// Initially (i.e. in the config file) it's rear wheel drive
	_drive = 'R';

	// Move the vehicle
	Vector3 v_pos = mCamera->getPosition() + (mCamera->getDirection() * 15.0);
	v_pos.y += 300;
	_vehicle->setPosition(v_pos);

	updateInfo();
}
//------------------------------------------------------------------------------------------------
void LandscapeFrameListener::updateInfo()
{
	Overlay* pOver = (Overlay*)OverlayManager::getSingleton().getByName("OgreOdeDemos/Overlay"); 
	String newInfo (String("Info: ") + carNames[sSelectedCar]);
	switch(_drive)
	{
		// Switch from rear to front
		case 'R':
			newInfo = newInfo + " & Front wheel drive";
			break;

			// Switch from front to all
		case 'F':
			newInfo = newInfo + " & All wheel drive";
			break;

			// Switch from all to rear
		case '4':
			newInfo = newInfo + " & Rear wheel drive";
			break;
	}
	OverlayManager::getSingleton().getOverlayElement("OgreOdeDemos/Info")->setCaption(newInfo);   
}
//------------------------------------------------------------------------------------------------
bool LandscapeFrameListener::frameStarted(const FrameEvent& evt)
{
	Real time = evt.timeSinceLastFrame;

    bool ret = ExampleFrameListener::frameStarted(evt);

	if (mTimeUntilNextToggle <= 0) 
	{
		// Switch debugging objects on or off
		if (mInputDevice->isKeyDown(KC_E))
		{
			World::getSingleton ().setShowDebugObjects(!World::getSingleton ().getShowDebugObjects());
			mTimeUntilNextToggle = 0.5;
		}

		if(mInputDevice->isKeyDown(KC_N)) 
		{
			changeCar();
			mTimeUntilNextToggle = 0.5;
		}

		if(mInputDevice->isKeyDown(KC_U)) 
		{
			_stepper->pause(false);
			mTimeUntilNextToggle = 0.5;
		}
		if(mInputDevice->isKeyDown(KC_P)) 
		{
			_stepper->pause(true);
			mTimeUntilNextToggle = 0.5;
		}
		// Change the drive mode between front, rear and 4wd
		if ((mInputDevice->isKeyDown(KC_X)))
		{
			switch(_drive)
			{
				// Switch from rear to front
			case 'R':
				_drive = 'F';

				_vehicle->getWheel(0)->setPowerFactor(1);
				_vehicle->getWheel(1)->setPowerFactor(1);
				_vehicle->getWheel(2)->setPowerFactor(0);
				_vehicle->getWheel(3)->setPowerFactor(0);

				updateInfo();
				break;

				// Switch from front to all
			case 'F':
				_drive = '4';

				_vehicle->getWheel(0)->setPowerFactor(0.6);
				_vehicle->getWheel(1)->setPowerFactor(0.6);
				_vehicle->getWheel(2)->setPowerFactor(0.4);
				_vehicle->getWheel(3)->setPowerFactor(0.4);

				updateInfo();
				break;

				// Switch from all to rear
			case '4':
				_drive = 'R';

				_vehicle->getWheel(0)->setPowerFactor(0);
				_vehicle->getWheel(1)->setPowerFactor(0);
				_vehicle->getWheel(2)->setPowerFactor(1);
				_vehicle->getWheel(3)->setPowerFactor(1);

				updateInfo();
				break;
			}
			mTimeUntilNextToggle = 0.5;
		}
	}

	if(!_stepper->isPaused())
	{
		_vehicle->setInputs(mInputDevice->isKeyDown(KC_J),mInputDevice->isKeyDown(KC_L),mInputDevice->isKeyDown(KC_I),mInputDevice->isKeyDown(KC_K));
		_vehicle->update(time);
	}

	// Thanks to Ahmed!
	const Real followFactor = 0.05; 
	const Real camHeight = 5.0; 
	const Real camDistance = 10.0; 
	const Real camLookAhead = 6.0;

	Quaternion q = _vehicle->getSceneNode()->getOrientation(); 
	Vector3 toCam = _vehicle->getSceneNode()->getPosition(); 

	toCam.y += camHeight; 
	toCam.z -= camDistance * q.zAxis().z; 
	toCam.x -= camDistance * q.zAxis().x; 
      
	mCamera->move( (toCam - mCamera->getPosition()) * followFactor ); 
	mCamera->lookAt(_vehicle->getSceneNode()->getPosition() + ((_vehicle->getSceneNode()->getOrientation() * Vector3::UNIT_Z) * camLookAhead));

	static Ray ray;
    ray.setOrigin(mCamera->getPosition() - Vector3(0,5,0));
    ray.setDirection(Vector3::UNIT_Y);
    _ray_query->setRay(ray);
    RaySceneQueryResult& result = _ray_query->execute();
    RaySceneQueryResult::iterator i = result.begin();
    if (i != result.end() && i->worldFragment)
    {
        SceneQuery::WorldFragment* wf = i->worldFragment;
        mCamera->setPosition(mCamera->getPosition().x,i->worldFragment->singleIntersection.y + 5.0,mCamera->getPosition().z);
    }

	return ret;
}

//------------------------------------------------------------------------------------------------
LandscapeApplication::LandscapeApplication()
{
	_world = 0;
	_time_step = 0.01;
	_terrain = 0;
}

//------------------------------------------------------------------------------------------------
bool LandscapeApplication::setup(void)
{
#ifndef _PLSM2
	mRoot = new Root("plugins.cfg", "ogre.cfg", "ogreode_landscape_tsm.log");
#else //_PLSM2
	mRoot = new Root("plugins_plsm2.cfg", "plsm2_display.cfg", "ogreode_landscape_plsm2.log");
#endif //_PLSM2

	setupResources();

	bool carryOn = configure();
	if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(20);

	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();

	// Create the scene
	createScene();

	createFrameListener();

	return true;

}

//------------------------------------------------------------------------------------------------
void LandscapeApplication::chooseSceneManager(void)
{
#ifndef _PLSM2
    mSceneMgr = mRoot->createSceneManager( ST_EXTERIOR_CLOSE, "basicsm" );
#else //_PLSM2
	mSceneMgr = mRoot->createSceneManager( "PagingLandScapeSceneManager", "basicsm" );
	assert(mSceneMgr);
#endif //_PLSM2
}

//------------------------------------------------------------------------------------------------
void LandscapeApplication::setupResources(void)
{
	ExampleApplication::setupResources();

	ResourceGroupManager *rsm = ResourceGroupManager::getSingletonPtr();
	StringVector groups = rsm->getResourceGroups();        
	if (std::find(groups.begin(), groups.end(), String("OgreOde")) == groups.end())
	{
		rsm->createResourceGroup("OgreOde");
		try
		{
			rsm->addResourceLocation("../../../../../ogreaddons/OgreOde/demos/Media","FileSystem", "OgreOde");
		}
		catch (Ogre::Exception& e)
		{
			String error = e.getFullDescription();
			rsm->addResourceLocation("../../../OgreOde/demos/Media","FileSystem", "OgreOde");
		}
	}
#ifdef _PLSM2
	if (std::find(groups.begin(), groups.end(), String("PLSM2")) == groups.end())
	{
		rsm->createResourceGroup("PLSM2");
		try
		{
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/models","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/overlays","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/materials","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/materials/scripts","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/materials/textures","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/materials/programs","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/datasrcs","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../../../ogreaddons/paginglandscape/Samples/Media/paginglandscape2/terrains","FileSystem", "PLSM2");
		}
		catch (Ogre::Exception& e)
		{
			String error = e.getFullDescription();
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/models","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/overlays","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/materials","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/materials/scripts","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/materials/textures","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/materials/programs","FileSystem", "PLSM2");

			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/datasrcs","FileSystem", "PLSM2");
			rsm->addResourceLocation("../../../paginglandscape/Samples/Media/paginglandscape2/terrains","FileSystem", "PLSM2");
		}
	}
#endif //_PLSM2
}

//------------------------------------------------------------------------------------------------
void LandscapeApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

	// Set a nice viewpoint
	mCamera->setPosition(25,15,25);
	mCamera->setOrientation(Quaternion(-0.3486, 0.0122, 0.9365, 0.0329));

    mCamera->setNearClipDistance( 1 );
    mCamera->setFarClipDistance( 1000 );
}

//------------------------------------------------------------------------------------------------
// Just override the mandatory create scene method
void LandscapeApplication::createScene(void)
{
    // Set ambient light
    mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

    // Create a light
    Light* l = mSceneMgr->createLight("MainLight");

	// Accept default settings: point light, white diffuse, just set position
    // NB I could attach the light to a SceneNode if I wanted it to move automatically with
    //  other objects, but I don't
    l->setPosition(20,800,50);
	l->setSpecularColour(1,0.9,0);
	l->setCastShadows(true);

#ifndef _DEBUG
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5,0.5,0.5));
#else
	mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
#endif

    // Fog
    // NB it's VERY important to set this before calling setWorldGeometry 
    // because the vertex program picked will be different
    ColourValue fadeColour(0.93, 0.86, 0.76);
    mSceneMgr->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
    mWindow->getViewport(0)->setBackgroundColour(fadeColour);

	// Set up the terrain according to our own settings	
#ifndef _PLSM2
	String config_file("landscape1.cfg");
	//String config_file("landscape2.cfg");
	//String config_file("landscape3.cfg");
	//String config_file("landscape4.cfg");
	//String config_file("landscape5.cfg");
#else  //_PLSM2
	String config_file("paginglandscape2.cfg");
#endif //_PLSM2
	DataStreamPtr cfgFile = ResourceGroupManager::getSingleton().openResource (config_file, "OgreOde");
    mSceneMgr -> setWorldGeometry( cfgFile );

	//String blank("");
	//mSceneMgr->setOption("CustomMaterialName",&blank);

    // Infinite far plane?
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
    {
		mCamera->setFarClipDistance(0);
    }

    // Define the required skyplane
    Plane plane;
    
	plane.d = 5000;
    plane.normal = -Vector3::UNIT_Y;


	_world = new OgreOde::World(mSceneMgr);

	_world->setGravity(Vector3(0,-9.80665,0));
	_world->setCFM(10e-5);
	_world->setERP(0.8);
	_world->setAutoSleep(true);
	_world->setContactCorrectionVelocity(1.0);



	bool does_center_is_corner = false;
	Vector3 scale = Vector3::ZERO;
	int nodes_per_side = 0;
	bool center;
	// Terrain Scene Manager should support those... 
	mSceneMgr->getOption("Scale", &scale);
	mSceneMgr->getOption("PageSize", &nodes_per_side);

#ifndef _PLSM2
	if (nodes_per_side == 0)
	{
		Ogre::ConfigFile config;
		Ogre::String val;

		config.load(config_file, "OgreOde", "=", true);

		val = config.getSetting( "PageSize" );
		assert( !val.empty() );
		nodes_per_side = atoi( val.c_str() );

		val = config.getSetting( "PageWorldX" );
		assert( !val.empty() );
		scale.x = atof( val.c_str() ) / nodes_per_side;

		val = config.getSetting( "MaxHeight" );
		assert( !val.empty() );
		scale.y = atof( val.c_str() );

		val = config.getSetting( "PageWorldZ" );
		assert( !val.empty() );
		scale.z = atof( val.c_str() ) / nodes_per_side;

	}
	center = false;
#else
	center = true;
#endif 

	_terrain = new OgreOde::TerrainGeometry(_world->getDefaultSpace(), 
											scale,
											nodes_per_side,
											center);
	_terrain->setHeightListener(this);

	
	_world->setCollisionListener(this);

	// Create some boxes to play with
    int i = 0;
#ifndef _DEBUG
    for(Real z = -12.0;z <= 12.0;z += 6.0)
    {
		for(Real x = -12.0;x <= 12.0;x += 6.0)
         {
#else
    for(Real z = -12.0;z <= 12.0;z += 12.0)
    {
		for(Real x = -12.0;x <= 12.0;x += 12.0)
         {
#endif
            // Create the Ogre box
            String name = String("Box_") + StringConverter::toString(i);
            Entity* box = mSceneMgr->createEntity(name,"Crate.mesh");
            box->setCastShadows(true);

            SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
            node->attachObject(box);
            node->setScale(0.15,0.15,0.15);
            
			// Set the position
            Vector3 pos(50 + x,130,50 + z);
            node->setPosition(pos);

            // Create a box for ODE and attach it to the Ogre version
            OgreOde::Body* body = new OgreOde::Body();
            node->attachObject(body);
            body->setMass(OgreOde::BoxMass(0.5,Vector3(1.5,1.5,1.5)));

            OgreOde::BoxGeometry* geom = new OgreOde::BoxGeometry(Vector3(1.5,1.5,1.5),_world->getDefaultSpace());
            geom->setBody(body);
            
			i++;
         }
      }
}

//------------------------------------------------------------------------------------------------
// Create new frame listener
void LandscapeApplication::createFrameListener(void)
{
    mFrameListener= new LandscapeFrameListener(mWindow, mCamera,_time_step,mRoot);
    mRoot->addFrameListener(mFrameListener);
}

//------------------------------------------------------------------------------------------------
Real LandscapeApplication::heightAt(const Vector3& position)
{
#ifndef _PLSM2
	return _terrain->getHeightAt(position);

#else _PLMS2
	
	/*
	Vector3 pos = position;
	pos.x-=256; //half the number of row quads
	pos.x*=(150000/512); //terrain width / number of row quads
	pos.z-=256;
	pos.z*=(150000/512);
	*/

	return _terrain->getHeightAt(position);
#endif _PLSM2
}


//------------------------------------------------------------------------------------------------
bool LandscapeApplication::collision(OgreOde::Contact* contact)
{
	if(!OgreOde_Prefab::Vehicle::handleTyreCollision(contact))
	{
		contact->setBouncyness(0.0);
		contact->setCoulombFriction(18.0);
	}
	return true;
}

//------------------------------------------------------------------------------------------------
LandscapeApplication::~LandscapeApplication()
{
	delete _terrain;
	delete _world;
}
