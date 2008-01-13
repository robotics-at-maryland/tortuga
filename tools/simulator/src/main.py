# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulation/src/main.py

# STD Imports
import os
import sys
from optparse import OptionParser

# Project Imports
import ext.core
# NOTE: sim.subsystems delayed till after setting of sys.path for Python-Ogre

def stop(event):
    global APPLICATION
    APPLICATION.stopMainLoop()

def main(args = None):
    if args is None:
        args = sys.argv

    # Determine path for Python-Ogre and set environment variable, and
    # PYTHONPATH properly
    parser = OptionParser()
    parser.add_option('-p','--python-ogre', dest='pythonOgreHome',
                      help = "Location of Python-Ogre install",
                      default = '/opt/ram/local/python-ogre-snapshot')
    options, args = parser.parse_args()

    os.environ['PYTHON_OGRE_HOME'] = options.pythonOgreHome
    sys.path.insert(0, os.path.join(options.pythonOgreHome, 'packages_2.5'))

    # Now Import the packages which depend on Python Ogre
    import sim.subsystems


    # Create Application class (creates all Subsystems)
    path = os.path.abspath(os.path.join(os.environ['RAM_SVN_DIR'], 'tools',
        'simulator', 'data', 'config','sim.yml'))
    app = ext.core.Application(path)

    # Hook into stop event to stop main loop
    global APPLICATION
    APPLICATION = app
    simulation = app.getSubsystem("Simulation")
    conn = simulation.subscribe(sim.subsystems.Simulation.SHUTDOWN, stop)
    
    # Go Into main loop
    app.mainLoop()
    
    # Remove extra references
    conn.disconnect()
    
    APPLICATION = None
    del app

    print 'DONE'

if __name__ == '__main__':
    sys.exit(main())
