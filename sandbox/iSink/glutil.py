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

"""Helper routines for graphics and vector math"""

from OpenGL.GL import *
import math


zaxis = (0,0,1)

def crossProduct(v1, v2):
	"""Return the cross product of two 3-vectors"""
	return (v1[1]*v2[2]-v1[2]*v2[1],v1[0]*v2[2]-v1[2]*v2[0],v1[0]*v2[1]-v1[1]*v2[0])

def dotProduct(v1, v2):
	"""Return the dot product of two 3-vectors"""
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

def glTranslateVec(v):
	glTranslated(v[0], v[1], v[2])

def glRotateQuat(q):
	angle = math.acos(q[0]) * 2 * 180 / math.pi
	glRotated(angle, q[1], q[2], q[3])

def glAlignWithVec(v):
	if v[0] != 0 or v[1] != 0:
		axis = crossProduct(v, zaxis)
		angle = math.acos(dotProduct(v, zaxis)) * 180 / math.pi
		glRotated(angle, axis[0], axis[1], axis[2])
	elif v[2] < 0:
		glRotated(180, 1, 0, 0)
