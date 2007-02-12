
##------------------------------------------------------------------------------------------------
carNames = [
	"Jeep",
	"JeepSway",
	"Subaru"
]
carFileNames= [
	"jeep.ogreode",
	"jeep.ogreode",
	"subaru.ogreode"
]
sSelectedCar = 1
maxNumCar = 3

##------------------------------------------------------------------------------------------------
class GranTurismOgreFrameListener.GranTurismOgreFrameListener

    def __init__(self, RenderWindow* win, Camera* cam,Real time_step,Root *root) : 
    	ExampleFrameListener(win, cam),
    	_vehicle(0)
    	## Reduce move speed
        mMoveSpeed = 25
    
    	## _stepper = new OgreOde.QuickStepper(time_step)
    	## _stepper = new OgreOde.ExactVariableQuickStepper(time_step)
    	## _stepper = new OgreOde.ForwardFixedQuickStepper(time_step)
    	_stepper = OgreOde.ForwardFixedInterpolatedQuickStepper (time_step)
    	
    	_stepper.setAutomatic(OgreOde.Stepper.AutoMode_PostFrame, root)
    	##_stepper.setAutomatic(OgreOde.Stepper.AutoMode_PreFrame, root)
    
    	##Root.getSingleton().setFrameSmoothingPeriod(5.0)
    	Root.getSingleton().setFrameSmoothingPeriod(0.0)
    	
    	changeCar()
    
    	## Load up our UI and display it
    	pOver = OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay")    
    	OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Name").setCaption(String("Name: ") + "GranTurismo (car + trimesh)")
    	OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Keys").setCaption(String("Keys: ") + "I/K - Accelerate/Brake, J/L - Turn, X - Change drive mode, N - Change Car")
    	OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/OtherKeys").setCaption(String("Extra: ") + "E - Debug Object")
    	pOver.show()
    
    ##------------------------------------------------------------------------------------------------
    def __del__(self ):
    	del _stepper

    ##------------------------------------------------------------------------------------------------
    def changeCar( self ):
        if sSelectedCar + 1 > maxNumCar:    
            sSelectedCar += 1
        else 
            sSelectedCar = 0
    	### sSelectedCar = (sSelectedCar + 1) % maxNumCar
    
    	del _vehicle	
    	_vehicle = dotOgreOdeLoader.loadVehicle (carFileNames[sSelectedCar], carNames[sSelectedCar])
    
    	## Initially (i.e. in the config file) it's rear wheel drive
    	_drive = 'R'
    
    	## Move the vehicle
    	v_pos = mCamera.getPosition() + (mCamera.getDirection() * 15.0)
    	##v_pos.y += 10
    	_vehicle.setPosition(v_pos)
    
    	updateInfo()  
    ##------------------------------------------------------------------------------------------------
    def updateInfo( self ):
    	pOver = OverlayManager.getSingleton().getByName("OgreOdeDemos/Overlay") 
    	newInfo = "Info: " + carNames[sSelectedCar])
    	if _drive =='R':
    		## Switch from rear to front
    		newInfo = newInfo + " & Front wheel drive"
        elif _drive == 'F':
    		## Switch from front to all
    		newInfo = newInfo + " & All wheel drive"
    		## Switch from all to rear
    	elif _drive == '4':
    		newInfo = newInfo + " & Rear wheel drive"
    	OverlayManager.getSingleton().getOverlayElement("OgreOdeDemos/Info").setCaption(newInfo)   
    
    ##------------------------------------------------------------------------------------------------
    def frameStarted( self, evt):
    	time = evt.timeSinceLastFrame
    
        ret = ExampleFrameListener.frameStarted(evt)
    
    	if (mTimeUntilNextToggle <= 0) :
    		## Switch debugging objects on or off
    		if (mInputDevice.isKeyDown(KC_E)):
    			World.getSingleton ().setShowDebugObjects(!World.getSingleton ().getShowDebugObjects())
    			mTimeUntilNextToggle = 0.5
    
    		if(mInputDevice.isKeyDown(KC_N)): 
    			changeCar()
    			mTimeUntilNextToggle = 0.5
    
    		if(mInputDevice.isKeyDown(KC_U)): 
    			_stepper.pause(False)
    			mTimeUntilNextToggle = 0.5
    		if(mInputDevice.isKeyDown(KC_P)): 
    			_stepper.pause(True)
    			mTimeUntilNextToggle = 0.5
    		## Change the drive mode between front, rear and 4wd
    		if ((mInputDevice.isKeyDown(KC_X))):
    			if _drive == 'R':
    				_drive = 'F'
    
    				_vehicle.getWheel(0).setPowerFactor(1)
    				_vehicle.getWheel(1).setPowerFactor(1)
    				_vehicle.getWheel(2).setPowerFactor(0)
    				_vehicle.getWheel(3).setPowerFactor(0)
    
    				updateInfo()
    
    				## Switch from front to all
    			elif _drive ==  'F':
    				_drive = '4'
    
    				_vehicle.getWheel(0).setPowerFactor(0.6)
    				_vehicle.getWheel(1).setPowerFactor(0.6)
    				_vehicle.getWheel(2).setPowerFactor(0.4)
    				_vehicle.getWheel(3).setPowerFactor(0.4)
    
    				updateInfo()
    
    				## Switch from all to rear
    			elif _drive == '4':
    				_drive = 'R'
    
    				_vehicle.getWheel(0).setPowerFactor(0)
    				_vehicle.getWheel(1).setPowerFactor(0)
    				_vehicle.getWheel(2).setPowerFactor(1)
    				_vehicle.getWheel(3).setPowerFactor(1)
    
    				updateInfo()
    			mTimeUntilNextToggle = 0.5
    			
    	if(!_stepper.isPaused()):
    		_vehicle.setInputs(mInputDevice.isKeyDown(KC_J),
                                mInputDevice.isKeyDown(KC_L),
                                mInputDevice.isKeyDown(KC_I),
                                mInputDevice.isKeyDown(KC_K))
    		_vehicle.update(time)
    
    		## Thanks to Ahmed!
    		followFactor = 0.2 
    		camHeight = 2.0 
    		camDistance = 7.0 
    		camLookAhead = 8.0
    
    		q = _vehicle.getSceneNode().getOrientation() 
    		toCam = _vehicle.getSceneNode().getPosition() 
    
    		toCam.y += camHeight 
    		toCam.z -= camDistance * q.zAxis().z 
    		toCam.x -= camDistance * q.zAxis().x 
    	      
    		mCamera.move( (toCam - mCamera.getPosition()) * followFactor ) 
    		mCamera.lookAt(_vehicle.getSceneNode().getPosition() + ((_vehicle.getSceneNode().getOrientation() * Vector3.UNIT_Z) * camLookAhead))
    	return ret

##------------------------------------------------------------------------------------------------
class GranTurismOgreApplication:

    def __init__ (self):
    	_world = 0
    	_time_step = 0.01
    	_track = 0
    
    ##------------------------------------------------------------------------------------------------
    def chooseSceneManager(self)
        mSceneMgr = mRoot.createSceneManager( ST_GENERIC, "ExampleGrandTurismo" )
    
    ##------------------------------------------------------------------------------------------------
    def setupResources(self):
    	ExampleApplication.setupResources()
    	rsm = ResourceGroupManager.getSingletonPtr()
    	groups = rsm.getResourceGroups()        
    	if (std.find(groups.begin(), groups.end(), String("OgreOde")) == groups.end()):
    		rsm.createResourceGroup("OgreOde")
    		try:
    			rsm.addResourceLocation("../../../../../ogreaddons/OgreOde/demos/Media","FileSystem", "OgreOde")
    		except: (Ogre.Exception& e)
    			String error = e.getFullDescription()
    			rsm.addResourceLocation("../../../OgreOde/demos/Media","FileSystem", "OgreOde")
    
    ##------------------------------------------------------------------------------------------------
    def createCamera(self):
        ## Create the camera
        mCamera = mSceneMgr.createCamera("PlayerCam")
    
        mCamera.setPosition(Vector3(125,-14,8))
        mCamera.lookAt(mCamera.getPosition() + Vector3(0,0,1))
        mCamera.setNearClipDistance( 1 )
        mCamera.setFarClipDistance( 1000 )
    
    ##------------------------------------------------------------------------------------------------
    ## Just override the mandatory create scene method
    def createScene(self):
        ## Set ambient light
        mSceneMgr.setAmbientLight(ColourValue(0.5, 0.5, 0.5))
    	mSceneMgr.setSkyBox(True,"GranTurismOgre/Skybox")
    
        ## Create a light
        l = mSceneMgr.createLight("MainLight")
    
        l.setPosition(20,800,50)
    	l.setSpecularColour(1,0.9,0)
    
    	l.setCastShadows(True)
    
    	mSceneMgr.setShadowTechnique(Ogre.SHADOWTYPE_STENCIL_MODULATIVE)
    	mSceneMgr.setShadowColour(ColourValue(0.5,0.5,0.5))
    
    	_world = OgreOde.World(mSceneMgr)
    
    	_world.setGravity(Vector3(0,-9.80665,0))
    	_world.setCFM(10e-5)
    	_world.setERP(0.8)
    	_world.setAutoSleep(True)
    	_world.setContactCorrectionVelocity(1.0)
    
    	## Create something that will step the world automatically
    	_world.setCollisionListener(self)
    
    	track_node = mSceneMgr.getRootSceneNode().createChildSceneNode("track")
    	track_mesh = mSceneMgr.createEntity("track","racingcircuit.mesh")
    	track_node.attachObject(track_mesh)
    
    	OgreOde.EntityInformer ei(track_mesh)
    	_track = ei.createStaticTriangleMesh(_world.getDefaultSpace())
    	
    ##------------------------------------------------------------------------------------------------
    ## Create new frame listener
    def createFrameListener(self):
        mFrameListener= GranTurismOgreFrameListener(mWindow, mCamera,_time_step,mRoot)
        mRoot.addFrameListener(mFrameListener)
    ##------------------------------------------------------------------------------------------------
    def collision(self, contact):
    	if(!OgreOde_Prefab.Vehicle.handleTyreCollision(contact))
    		contact.setBouncyness(0.0)
    		contact.setCoulombFriction(18.0)
    	return True
    ##------------------------------------------------------------------------------------------------
    def __del__(self):
    	delete _track
    	delete _world
