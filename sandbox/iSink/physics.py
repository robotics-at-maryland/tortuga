############################################################################
#                                                                          #
#    iSink ROV Simulator                                                   #
#                                                                          #
#    written for Robotics@Maryland 2006                                    #
#    University of Maryland, College Park                                  #
#                                                                          #
#    Copyright (C) 2006 by Leo Singer                                      #
#    aronnax@umd.edu                                                       #
#                                                                          #
############################################################################

"""Underwater physics engine based on Open Dynamics Engine"""

import ode
from glutil import *
from OpenGL.GL import *
from OpenGL.GLUT import *


class bWorld(ode.World):
	
	__waterDensity = 1
	
	def __init__(self):
		ode.World.__init__(self)
		self.bodies = []
		self.setGravity((0,0,-9.81))
		
	def addBody(self, body):
		self.bodies.append(body)
		
	def removeBody(self, body):
		self.bodies.remove(body)
		
	def getWaterDensity(self):
		return self.__waterDensity
		
	def __preStep(self):
		for x in self.bodies:
			x.preStep()
		
	def __postStep(self):
		for x in self.bodies:
			x.postStep()
		
	def step(self, stepsize):
		self.__preStep()
		ode.World.step(self, stepsize)
		self.__postStep()
		
	def quickStep(self, stepsize):
		self.__preStep()
		ode.World.quickStep(self, stepsize)
		self.__postStep()
	
	def display(self):
		for x in self.bodies:
			x.display()


class bBody(ode.Body):
	
	drag = 1
	
	def __init__(self,w):
		ode.Body.__init__(self,w)
		self.gyroscopes = []
		self.__world = w
		self.__world.addBody(self)
		self.volume = 0.15
		self.cob = (0,0,0)
		
	def __addFluidForces(self):
		if self.isEnabled():
			# apply buoyant force acting through center of buoyancy
			if self.getGravityMode():
				wmass = self.getSubmergedVolume() * self.__world.getWaterDensity()
				bforce = [-x * wmass for x in self.__world.getGravity()]
				self.addForceAtRelPos(bforce, self.getCenterOfBuoyancy())
			# apply linear drag acting through center of drag
			# is this the right way to model drag?
			cod = self.getCenterOfDrag()
			self.addForceAtRelPos([-x * self.drag for x in self.getRelPointVel(cod)], cod)
	
	def preStep(self):
		self.__addFluidForces()
	
	def postStep(self):
		for x in self.gyroscopes:
			x.setAngularRate(dotProduct(self.getAngularVel(), self.vectorToWorld(x.axis)))
	
	def getSubmergedVolume(self):
		# TODO some time soon this should take into account the fact that
		# the vehicle might be only partially submerged
		return self.volume
	
	def getCenterOfBuoyancy(self):
		# TODO some time soon this should take into account the fact that
		# the vehicle might be only partially submerged
		return self.cob
	
	def getCenterOfDrag(self):
		# TODO center of drag might not be the same as center of buoyancy
		return self.getCenterOfBuoyancy()
	
	def display(self):
		pass


class Thruster:
	
	mag = 0
	orientation = (0,0,1)
	position = (0,0,0)
	
	def __init__(self, position, orientation):
		self.orientation = orientation
		self.position = position
	
	def setMagnitude(self, mag):
		self.mag = mag
	
	def getMagnitude(self):
		return self.mag
	
	def display(self):
		glPushMatrix()
		glTranslateVec(self.position)
		glAlignWithVec(self.orientation)
		glScaled(1,1,3)
		glutSolidTorus(0.01,0.05,10,10)
		glPopMatrix()


class Gyroscope:
	
	axis = (0,0,1)
	__angularRate = 0
	
	def __init__(self, axis):
		self.axis = axis
	
	def setAngularRate(self, ar):
		self.__angularRate = ar
	
	def getAngularRate(self):
		return self.__angularRate


class Submersible(bBody):
	
	def __init__(self, w):
		bBody.__init__(self, w)
		self.thrusters = []
		self.gyroscopes.append(Gyroscope((1,0,0)))
		self.gyroscopes.append(Gyroscope((0,1,0)))
		self.gyroscopes.append(Gyroscope((0,0,1)))
		self.setGravityMode(True)
	
	def __addThrusterForces(self):
		for thruster in self.thrusters:
			force = [x * thruster.getMagnitude() for x in thruster.orientation]
			self.addRelForceAtRelPos(force, thruster.position)
	
	def preStep(self):
		bBody.preStep(self)
		self.__addThrusterForces()
	
	def display(self):
		# draw the center of mass as a red sphere
		# and the center of buoyancy as a blue sphere
		# thrusters are gray toruses
		glPushMatrix()
		glTranslateVec(self.getPosition())
		glRotateQuat(self.getQuaternion())
		glColor3d(1,0,0)
		glutSolidSphere(0.1,16,16)
		glPushMatrix()
		glTranslateVec(self.getCenterOfBuoyancy())
		glColor3d(0,0,1)
		glutSolidSphere(0.1,16,16)
		glPopMatrix()
		glColor3d(0.5,0.5,0.5)
		for x in self.thrusters:
			x.display()
		glPopMatrix()
