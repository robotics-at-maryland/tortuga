# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  tools/oci/ociapp.py


'''
The entry point for the robot operation. This module creates all the controllers and models that
are necessary for robot operation, and then starts the state machine.
'''

    
# Python Imports
import os
import sys
import time
    
# Library Imports
import yaml

# Project Imports
import ociapp

def main():            
    app = ociapp.OCIApp('controltest.yml')
    print 'Test'
    app.main_loop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)