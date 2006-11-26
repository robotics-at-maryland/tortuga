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
#    Requires:                                                             #
#      PyODE                                                               #
#      PyOpenGL                                                            #
#                                                                          #
############################################################################

#!/usr/bin/env python

from simplesub import *
from sim import *
from OpenGL.GLUT import *
from sys import *
from threading import *
import time
import thread


class BrainThread(ControlThread):
	# this method is the control routine for the submersible
	# edit this method to change the submersible's behavior
	def run(self):
		# turn on port and starboard thrusteres
		self.sub.port.setMagnitude(1)
		self.sub.starboard.setMagnitude(1)
		# get rid of pitching motion by firing fore and aft thrusters
		while 1:
			n = self.sub.xgyro.getAngularRate() * 2
			self.sub.fore.setMagnitude(n)
			self.sub.aft.setMagnitude(-n)
			sleep(0.001)


# set up simulator
bworld = bWorld()
sub = SimpleSub(bworld)
glutInit(argv)
simthread = Simulator(bworld)
brthread = BrainThread(sub)

# defer execution of control thread for one second to
# give simulation time to initialize
Timer(1,brthread.start).start()

# start simulation
simthread.go()
