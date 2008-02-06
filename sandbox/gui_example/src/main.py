# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sandbox/gui_example/src/main.py

# STD Imports
import sys

# Library Imports
import yaml

# Project Imports
import oci.app as app

def main():            
    application = app.Application()
    application.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)