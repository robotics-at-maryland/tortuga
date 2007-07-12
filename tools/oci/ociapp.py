# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/oci/ociapp.py

# Project Imports
import baseapp
from vehicle.vehicle import Vehicle
import cli
import ai.cleanAI

class OCIApp(baseapp.AppBase):
    def __init__(self, config_path):     
        baseapp.AppBase.__init__(self, config_path)