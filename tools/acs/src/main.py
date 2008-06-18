# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sandbox/gui_example/src/oci/app.py

# Python Imports
import time
import sys
import os
import optparse

# Library Imports
import yaml

# Project Imports
import ext.core

# To register the makers
import ram
import ext.vision

try:
    import ctypes
    ctypes.cdll.LoadLibrary("libram_network.so")
    #ctypes.cdll.LoadLibrary("libram_network_d.so")
except:
    pass

def main(argv = None):
    if argv is None:
        argv = sys.argv

    defaultConfigPath = os.path.abspath(
        os.path.join(os.environ['RAM_SVN_DIR'], 'tools', 'oci', 'data', 
                     'test.yml'))
    
    parser = optparse.OptionParser()
    parser.add_option("-c", "--config", dest = "configPath", 
                      default = defaultConfigPath,
                      help = "The path to the config file")
    (options, args) = parser.parse_args(argv)

    # Create our C++ app
    app = ext.core.Application(options.configPath)      

    vehicle = app.getSubsystem('Vehicle')

    for i in xrange(1,100):
        vehicle.update(0)

    batt1 = vehicle.getDevice('Batt 1')
    print "Battery 1: %f (A) %f (V) %d (Enabled)" % \
        (batt1.getVoltage(), batt1.getCurrent(), batt1.enabled())

    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)
