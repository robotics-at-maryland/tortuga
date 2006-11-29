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


def sphereChordVolume(r, y):
	"""Compute the submerged volume a sphere whose center
lies y units below the water level"""
	return 2 * math.pi / 3 * math.pow(r, 3) * (1 - float(y) / r)
