# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/__init__.py

# To allow the registration of SubsystemMakers
import ram.motion.basic as _basic
import ram.timer as _timer
import ram.ai.state as _state

import os as __os
import sys as __sys
from distutils import sysconfig as __sysconfig

# Ensure we are using the proper version of python
if __os.environ['RAM_PYTHON_EXE'] !=  __os.path.realpath(__sys.executable):
    print 'Error running with python exe:', __os.path.realpath(__sys.executable)
    print 'Bootstrap run with:', __os.environ['RAM_PYTHON_EXE']
    __sys.exit(1)

if __os.environ['RAM_PYTHON_PREFIX'] != __sysconfig.PREFIX:
    print 'Error running with python prefix:',__sysconfig.PREFIX
    print 'Bootstrap run with:', __os.environ['RAM_PYTHON_PREFIX']
    __sys.exit(1)

if __os.environ['RAM_PYTHON_VERSION'] !=  __sys.version.replace('\n',''):
    print 'Error running with python version:', __sys.version.replace('\n','')
    print 'Bootstrap run with:', __os.environ['RAM_PYTHON_VERSION']
    __sys.exit(1)

