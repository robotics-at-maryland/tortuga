# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   sandbox/gui_example/src/main.py

# STD Imports
import sys
import os
from optparse import OptionParser

# Library Imports
import yaml

# Project Imports
import oci.app as app
import ram

# Needed to register makers
import ext.vision

try:
    import ctypes
    import platform
    end = 'so'
    if 'Darwin' == platform.system():
        end = 'dylib'
    ctypes.CDLL("libram_network." + end, mode = ctypes.RTLD_GLOBAL)
except OSError, e:
    print e

def main():            
    # Parse command line options
    defaultConfigPath = os.path.abspath(
        os.path.join(os.environ['RAM_SVN_DIR'], 'tools', 'oci', 'data', 
                     'test.yml'))
    
    parser = OptionParser()
    parser.add_option("-c", "--config", dest = "configPath", 
                      default = defaultConfigPath,
                      help = "The path to the config file")
    (options, args) = parser.parse_args()

    application = app.Application(options.configPath)
    application.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)
