# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/python/control.py

# STD Imports
import os
import sys
import StringIO
from distutils import sysconfig

# Ensure we are using the proper version of python
import ram_version_check


# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    # Only import the core module if it exists, its a conditional dependency
    import os as __os
    __coreModulePath = __os.path.join(__os.environ['RAM_SVN_DIR'], 'build_ext',
                                      'ext', 'core.py')
    if __os.path.exists(__coreModulePath):
        # Core more first because of Boost.Python wrapping dependencies
        import ext.core

    from ext._math import *

finally:
    sys.stderr = stderr

