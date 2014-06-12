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

if not os.environ.has_key('RAM_SVN_DIR'):
   raise Exception('R@M Environment Not Setup. Run scripts/setenv')

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
    parser.add_option("-r", "--redirect", dest = "redirect",
                      action="store_true",
                      help="Turn on stdout redirecting to the pycrust shell")
    parser.add_option("-n", "--no-redirect", dest = "redirect",
                      action="store_false",
                      help="Turn off stdout redirecting to the pycrust shell")
    (options, args) = parser.parse_args()

    application = app.Application(options.configPath, options.redirect)
    application.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)
