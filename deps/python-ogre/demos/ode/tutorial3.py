# pyODE example 3: Collision detection

import sys, os, random
from math import *
import ODE as ode



# prepare_GL
def prepare_GL():
    """Prepare drawing.
    """
    pass

# draw_body
def draw_body(body):
    """Draw an ODE body.
    """
    return
    x,y,z = body.getPosition()
    R = body.getRotation()
    T = mat4()
    T[0,0] = R[0]
    T[0,1] = R[1]
    T[0,2] = R[2]
    T[1,0] = R[3]
    T[1,1] = R[4]
    T[1,2] = R[5]
    T[2,0] = R[6]
    T[2,1] = R[7]
    T[2,2] = R[8]
    T[3] = (x,y,z,1.0)

    glPushMatrix()
    glMultMatrixd(T.toList())
    if body.shape=="box":
        sx,sy,sz = body.boxsize
        glScale(sx, sy, sz)
        glutSolidCube(1)
    glPopMatrix()


# create_box
def create_box(world, space, density, lx, ly, lz):
    """Create a box body and its corresponding geom."""

    # Create body
    
    body = ode.dBody(world.id())
    M = ode.dMass()
    M.setBox(density, lx, ly, lz)
    body.setMass(M)

    # Set parameters for drawing the body
    body.shape = "box"
    body.boxsize = (lx, ly, lz)

    # Create a box geom for collision detection
    geom = ode.dCreateBox(space.id(), lx,ly,lz )
    ode.dGeomSetBody ( geom, body.id() )
    

    return body

# drop_object
def drop_object():
    """Drop an object into the scene."""
    
    global bodies, counter, objcount
    
    body = create_box(world, space, 1000, 1.0,0.2,0.2)
    body.setPosition( random.gauss(0,0.1),3.0,random.gauss(0,0.1) )
    
    #### m = mat4().rotation(random.uniform(0,2*pi), (0,1,0))  ## BUG  Need a Matrix 4 class
    #### body.setRotation(0.0)  ## m.toList())  ##BUG , needs to be a pointer to the float (array??)
    bodies.append(body)
    counter=0
    objcount+=1

# explosion
def explosion():
    """Simulate an explosion.

    Every object is pushed away from the origin.
    The force is dependent on the objects distance from the origin.
    """
    global bodies

    for b in bodies:
        p = vec3(b.getPosition())
        d = p.length()
        a = max(0, 40000*(1.0-0.2*d*d))
        p = vec3(p.x/4, p.y, p.z/4)
        b.addForce(a*p.normalize())

# pull
def pull():
    """Pull the objects back to the origin.

    Every object will be pulled back to the origin.
    Every couple of frames there'll be a thrust upwards so that
    the objects won't stick to the ground all the time.
    """
    global bodies, counter
    
    for b in bodies:
        p = vec3(b.getPosition())
        b.addForce(-1000*p.normalize())
        if counter%60==0:
            b.addForce((0,10000,0))
    
# Collision callback
def near_callback(args, geom1, geom2):
    """Callback function for the collide() method.

    This function checks if the given geoms do collide and
    creates contact joints if they do.
    """

    print "In Call back !!"
    return
    # Check if the objects do collide
    contactCount = ode.dCollide(geom1, geom2)

    # Create contact joints
    world,contactgroup = args
    for c in contacts:
        c.setBounce(0.2)
        c.setMu(5000)
        j = ode.ContactJoint(world, contactgroup, c)
        j.attach(geom1.getBody(), geom2.getBody())


######################################################################


# Create a world object
world = ode.dWorld()
world.setGravity( 0,-9.81,0 )
world.setERP(0.8)
world.setCFM(1E-5)

# Create a space object
space = ode.CreateSimpleSpace()

# Create a plane geom which prevent the objects from falling forever
floor = ode.dPlane(space.id(), 0.0, 1.0, 0.0, 0.0)

# A list with ODE bodies
bodies = []

# A joint group for the contact joints that are generated whenever
# two bodies collide
contactgroup = ode.dJointGroup()

# Some variables used inside the simulation loop
fps = 50
dt = 1.0/fps
running = True
state = 0
counter = 0
objcount = 0
#clk = pygame.time.Clock()
#frame=1
while running:

    counter+=1
    # State 0: Drop objects
    if state==0:
        if counter==20:
            drop_object()
        if objcount==30:
            state=1
            counter=0
    # State 1: Explosion and pulling back the objects
    elif state==1:
        if counter==100:
            explosion()
        if counter>300:
            pull()
        if counter==500:
            counter=20

    # Draw the scene
    for b in bodies:
        draw_body(b)
    
    # Simulate
    n = 2
    for i in range(n):
        # Detect collisions and create contact joints
        space.collide((world,contactgroup), near_callback, "")

        # Simulation step
        world.step(dt/n)

        # Remove all contact joints
        contactgroup.empty()
    
#    clk.tick(fps)
    
