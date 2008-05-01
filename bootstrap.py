# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  bootstrap.py

import os
import sys
from buildfiles.common import util
from optparse import OptionParser

# Check Python Version, ensure 2.5.x
PYTHON_VERSION_STR = util.python_version_str()
ROOT_DIR = os.path.abspath(sys.path[0])

DEFULT_TASKS = ['setup_directories', 
                'install_python_modules', 
                'gen_setenv']

DEFAULT_PREFIX = util.ram_prefix()
PYTHON_SITE_PACKAGE_SUFFIX = util.site_packages_suffix()
    
def main(argv=None):
    # Parse Arguments
    parser = OptionParser()
    parser.set_defaults(task = str(DEFULT_TASKS), prefix = DEFAULT_PREFIX)
    parser.add_option('-t','--task', nargs = 1,
                      help = 'Bootstrap tasks to run')
    parser.add_option('-p','--prefix', nargs = 1,    
                      help = 'The prefix to install all packages into'
                             ' [default: %default]')
    (options, args) = parser.parse_args()

    site_package_dir = os.path.join(options.prefix, PYTHON_SITE_PACKAGE_SUFFIX)

    # Buildit imports
    util.ensure_buildit_installed(ROOT_DIR, site_package_dir, options.prefix)
    from buildit.context import Context
    from buildit.context import Software

    # Make sure out site package dir is on the python path
    default_path = os.environ.get('PYTHONPATH', '')
    os.environ['PYTHONPATH'] = site_package_dir + os.pathsep + default_path

    # Task information
    import buildfiles.bootstrap.tasks as build_tasks

    # Create and check buildit context
    context = Context(os.path.join(ROOT_DIR, 'buildfiles', 'bootstrap',
                                   'root.ini'))
    context.globals['ram_prefix'] = options.prefix
    context.globals['python_version_str'] = PYTHON_VERSION_STR
    context.globals['py_site_packages'] = site_package_dir
    context.globals['py_site_packages_suffix'] = PYTHON_SITE_PACKAGE_SUFFIX
    context.globals['python_executable'] = os.path.realpath(sys.executable)
        

    # Determing and run build tasks
    if len(args) > 0:
        tasks = args
    else:
        tasks = DEFULT_TASKS

    for task_name in tasks:
        print 'Attempting Task',task_name
        task = getattr(build_tasks, task_name)
        software = Software(task, context)
        software.install()

    return 0

if __name__ == "__main__":
    sys.exit(main())
