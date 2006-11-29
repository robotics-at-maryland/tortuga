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

"""Compute buoyant volumes of partially submerged solids"""


import math
from glutil import *

def sphereSubmerged(radius, origin, waterNormal):
	pd = __pointDepth(origin, waterNormal)
	vol = __sphereChordVolume(radius, pd)
	cm = scaleVec(waterNormal, -__sphereChordCMDepth(radius, pd))
	cm = addVec(cm, origin)
	return (vol, cm)

def __pointDepth(r, v):
	return dotProduct((projectOntoUnit(r, v)),v)

def __sphereChordVolume(r, y):
	"""Compute the submerged volume a sphere whose center
lies y units below the water level"""
	return 2 * math.pi / 3 * math.pow(r, 3) * (1 - float(y) / r)

def __sphereChordCMDepth(r, y):
	"""Compute the z-coordinate of the CM of a partially
submerged sphere"""
	# TODO
	return 0
