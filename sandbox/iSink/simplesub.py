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

from physics import *


class SimpleSub(Submersible):
	"""Readymade submersible with four thrusters and three gyroscopes"""
	
	def __init__(self, w):
		Submersible.__init__(self, w)
		self.starboard = Thruster((0.25,0,0.3),(0,1,0))
		self.port = Thruster((-0.25,0,0.3),(0,1,0))
		self.fore = Thruster((0,-0.25,0),(0,0,1))
		self.aft = Thruster((0,0.25,0),(0,0,1))
		self.thrusters.append(self.port)
		self.thrusters.append(self.starboard)
		self.thrusters.append(self.fore)
		self.thrusters.append(self.aft)
		self.xgyro = Gyroscope((1,0,0))
		self.ygyro = Gyroscope((0,1,0))
		self.zgyro = Gyroscope((0,0,1))
		self.gyroscopes.append(self.xgyro)
		self.gyroscopes.append(self.ygyro)
		self.gyroscopes.append(self.zgyro)
		mass = ode.Mass()
		mass.setBox(1,0.5,0.5,0.5)
		self.setMass(mass)
		self.cob = (0,0,0.3)
		self.volume = 0.125
