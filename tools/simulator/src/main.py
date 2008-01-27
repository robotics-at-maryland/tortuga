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

def commandLineStart(path):
    import sim.subsystems
    
    # Create Application class (creates all Subsystems)
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
    
def guiStart(path):
    # Get the OCI on our path
    ociPath = os.path.abspath(os.path.join(os.environ['RAM_SVN_DIR'], 
        'tools', 'oci', 'src'))
    if not sys.path.count(ociPath):
        sys.path.insert(0, ociPath)
    
    # Now we can import OCI
    import oci.app
    
    # Our Specific Panels
    import sim.view
    
    # Now we can start up the vehicle
    application = oci.app.Application(path)
    application.MainLoop()
    
def main(args = None):
    if args is None:
        args = sys.argv


    parser = OptionParser()
    parser.add_option('-c', '--config', dest = 'configPath', 
                      help = 'The path to the config file')
    parser.add_option('-p', '--python-ogre', dest='pythonOgreHome',
                      help = 'Location of Python-Ogre install')
    parser.add_option('-g', '--gui', dest = 'useGUI', action='store_true',
                      help = 'Turn on graphical interface')
    
    # Set defaults for configuration options
    defaultConfigPath = os.path.abspath(os.path.join(os.environ['RAM_SVN_DIR'], 
        'tools', 'simulator', 'data', 'config','sim.yml'))
    
    defaultPythonOgreHome = '/opt/ram/local/python-ogre-snapshot'
    if os.environ.has_key('PYTHON_OGRE_HOME'):
        defaultPythonOgreHome = os.environ['PYTHON_OGRE_HOME']
    
    parser.set_defaults(configPath = defaultConfigPath,
                        pythonOgreHome = defaultPythonOgreHome,
                        useGUI = False)

    # Parse arguments, (help requests quit program here)
    options, args = parser.parse_args()

    # Determine path for Python-Ogre and set environment variable, and
    # PYTHONPATH properly
    os.environ['PYTHON_OGRE_HOME'] = options.pythonOgreHome
    # The import of ram.sim will put PYTHON_OGRE_HOME onto the sys.path
    sys.path.insert(0, os.path.join(options.pythonOgreHome, 'packages_2.5'))

    # Now Import the packages which depend on Python Ogre
    import sim.subsystems

    # Start up the application (with or without the GUI)
    if (options.useGUI):
        guiStart(options.configPath)
    else:
        commandLineStart(options.configPath)
    
    print 'DONE'

if __name__ == '__main__':
    sys.exit(main())
