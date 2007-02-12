import ODE as ode

NUM= 10			## number of boxes
SIDE = 0.2		## side length of a box
MASS= 1.0		## mass of a box
RADIUS =0.1732	## sphere radius


## dynamics and collision objects

body=[]
box=[]
joint = []
for i in range(NUM):
    body.append(ode.dBody() )
    box.append(ode.dBox() )
    joint.append (ode.dBallJoint() )
contactgroup= ode.dJointGroup()
space = ode.dSimpleSpace(None)
world = ode.dWorld()

## this is called by space.collide when two objects in space are
## potentially colliding.

def nearCallback (data, o1, o2):

  ## exit without doing anything if the two bodies are connected by a joint
  b1 = ode.dGeomGetBody(o1)
  b2 = ode.dGeomGetBody(o2)
  if b1 and b2 and ode.dAreConnected (b1,b2):
    return

  ## @@@ it's still more convenient to use the C interface here.

  contact = ode.dContact()
  contact.surface.mode = 0
  contact.surface.mu = ode.dInfinity
  if ode.dCollide ( o1,o2,0,contact.geom,sizeof(ode.dContactGeom) ):
     c = dJointCreateContact (world.id(),contactgroup.id(),&contact)
     ode.dJointAttach (c,b1,b2)



## start simulation - set viewpoint

def start():
#   xyz[] = {2.1640,-1.3079,1.7600}
#   hpr[] = {125.5000,-17.0000,0.0000}
#   dsSetViewpoint (xyz,hpr)
    pass

## simulation loop
def simLoop (pause) :
  if ( not pause):
    angle = 0
    angle += 0.05
    body[NUM-1].addForce (0,0,1.5*(sin(angle)+1.0))

    space.collide (0,nearCallback)
    world.step (0.05)

    ## remove all contact joints
    contactgroup.empty()
  sides = (SIDE,SIDE,SIDE)
  dsSetColor (1,1,0)
  dsSetTexture (DS_WOOD)
  for i in range (NUM):
    dsDrawBox (body[i].getPosition(),body[i].getRotation(),sides)



# def main (argc, argv):

  ## setup pointers to drawstuff callback functions
#   dsFunctions fn
#   fn.version = DS_VERSION
#   fn.start = &start
#   fn.step = &simLoop
#   fn.command = 0
#   fn.stop = 0
#   fn.path_to_textures = "../../drawstuff/textures"
#   if(argc==2)
#     {
#         fn.path_to_textures = argv[1]
#     }

  ## create world
ode.dInitODE()

contactgroup.create (0)
world.setGravity (0,0,-0.5)
ode.dWorldSetCFM (world.id(),1e-5)
plane = ode.dPlane(space.id(),0.0,0.0,1.0,0.0)

for i in range (NUM):
    body[i].create (world.id())
    k = i*SIDE
    body[i].setPosition (k,k,k+0.4)
    m = ode.dMass()
    m.setBox (1,SIDE,SIDE,SIDE)
    m.adjust (MASS)
    body[i].setMass (m)
    body[i].setData (i)
    
    box[i].create (space.id(),SIDE,SIDE,SIDE)
    box[i].setBody (body[i].id())
for i in range(NUM-1):
    joint[i].create (world.id())
    joint[i].attach (body[i].id(),body[i+1].id())
    k = (i+0.5)*SIDE
    joint[i].setAnchor (k,k,k+0.4)

## run simulation
#dsSimulationLoop (argc,argv,352,288,&fn)

ode.dCloseODE()

