# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/simulator/src/sim/__init__.py

# The following imports are to make sure that all classes have a chance to
# register themselvers with the makers system
import sim.control as _control
import sim.subsystems as _subsystems
import sim.vehicle as _vehicle
import sim.vision as _vision
import sim.sonar as _sonar