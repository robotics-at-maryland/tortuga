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
PYTHON_VERSION = sys.version_info[0:2]
assert (2,5) == PYTHON_VERSION # Check Python Version
PYTHON_VERSION_STR = 'python%d.%d' % PYTHON_VERSION
ROOT_DIR = os.path.abspath(sys.path[0])

DEFULT_TASKS = ['setup_directories', 
                'install_python_modules', 
                'gen_setenv']

DEFAULT_PREFIX = '/opt/local/ram'

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

    # Buildit imports
    util.ensure_buildit_installed(ROOT_DIR, PYTHON_VERSION_STR, options.prefix)
    from buildit.context import Context
    from buildit.context import Software

    # Task information
    import buildfiles.tasks as build_tasks

    # Create and check buildit context
    context = Context(os.path.join(ROOT_DIR, 'buildfiles', 'root.ini'))
    context.globals['ram_prefix'] = options.prefix
    context.globals['python_version_str'] = PYTHON_VERSION_STR
    context.globals['py_site_packages'] = \
        os.path.join(options.prefix, 'lib', PYTHON_VERSION_STR, 'site-packages')
        
    print options.prefix

    # Determing and run build tasks
    if len(args) > 0:
        tasks = args
    else:
        tasks = DEFULT_TASKS

    for task_name in tasks:
        task = getattr(build_tasks, task_name)
        software = Software(task, context)
        software.install()

    return 0

if __name__ == "__main__":
    sys.exit(main())
