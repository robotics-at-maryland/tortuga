# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/common/util.py

import sys
import os.path
import subprocess

class CommandError(Exception):
    pass     

def safe_system(command):
    retcode = subprocess.call(command, shell=True)
    if retcode != 0:   
        raise CommandError, "Command '%s' with status %d" % \
             (command, retcode)
    
def run_shell_cmd(command, error_msg):
    """
    Run the given shell command return the stdout result
    """
    if type(command) is not list:
        command = command.split()
    try:
        proc = subprocess.Popen(command, stdout=subprocess.PIPE)
        result = proc.communicate()

        if 0 != proc.returncode:
            print error_msg
        
        return result[0], proc.returncode
    except OSError:
        print error_msg
        sys.exit(1)

def ensure_buildit_installed(root_dir, site_package_dir, prefix_dir):
    config_path = os.path.join(root_dir, 'buildfiles', 'root.ini')    
        
    build_install_dir = os.path.join(site_package_dir, 'buildit')
    
    # Check to see if Buildit is installed
    if not os.path.exists(build_install_dir):
        print 'Could not find buildit attempting install...'
        # Install buidit
        package_dir = os.path.join(root_dir, 'deps', 'buildit')   
        install_buildit(package_dir, prefix_dir)
    
    # Check to make the sitepackage directory is on the path
    try:
        sys.path.index(site_package_dir)
    except ValueError:
        print 'Could not find "%s" on the python path, adding it' % site_package_dir
        sys.path.insert(1, site_package_dir)
        
def install_buildit(package_dir, prefix_dir):
    print 'Changing into:',package_dir
    cwd = os.getcwd()
    os.chdir(package_dir)

    # Run setup.py for that package    
    command_str = '"%s" setup.py install --prefix=%s' % \
        (sys.executable, prefix_dir) 
    safe_system(command_str)
    
    # Change back to original directory
    print 'Returning to',cwd
    os.chdir(cwd)

def python_version_str():
    """
    Gets the python version string, ie: python2.5 for Python 2.5.x
    """
    python_version = sys.version_info[0:2]
    assert (2,5) == python_version or (2,6) == python_version # Check Python Version
    return 'python%d.%d' % python_version

def ram_prefix():
    """
    Returns the current prefix RAM dependency libraries are installed in
    """
    prefix = None
    if os.name == 'posix':
        prefix = '/opt/ram/local'
    elif os.name == 'nt':
        prefix = os.environ['RAM_ROOT_DIR']
    else:
        print 'OS "%s" Not Supported' % os.name
    return prefix

def site_packages_suffix():
    """
    Gets the suffix (the part after the prefix) of the python site packages dir
    """
    site_packages_suffix = None
    if os.name == 'posix':
        site_packages_suffix = os.path.join('lib', python_version_str(),
                                            'site-packages')
    elif os.name == 'nt':
        site_packages_suffix = os.path.join('Lib','site-packages')
    else:
        print 'OS "%s" Not Supported' % os.name

    return site_packages_suffix
