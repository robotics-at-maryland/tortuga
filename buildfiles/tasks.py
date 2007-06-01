# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tasks.py

import sys

from common.commands import *
from buildit.task import Task

# Setup basic directory structure
setup_directories = Task(
    'Setup Directory Structure',
    namespaces = 'bootstrap',
    targets = '${ram_prefix}/man',
    workdir = '${buildoutdir}',
    commands = [Mkdir('${py_site_packages}/'),
                Mkdir('${ram_prefix}/lib/pkgconfig/'),
                Mkdir('${ram_prefix}/include/'),
                Mkdir('${ram_prefix}/bin/'),
                Mkdir('${ram_prefix}/man/')],
    )

# Install Python Modules
install_pygccxml = Task(
    'Install Pygccxml',
    namespaces = 'pygccxml',
    targets = '${py_site_packages}/pygccxml',
    workdir = '${deps_dir}/pygccxml',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_pyplusplus = Task(
    'Install Py++',
    namespaces = 'pyplusplus',
    targets = '${py_site_packages}/pyplusplus',
    workdir = '${deps_dir}/pyplusplus',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_pyserial = Task(
    'Install PySerial',
    namespaces = 'pyserial',
    targets = '${py_site_packages}/serial',
    workdir = '${deps_dir}/pyserial',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_pyyaml = Task(
    'Install PyYaml',
    namespaces = 'pyyaml',
    targets = '${py_site_packages}/yaml',
    workdir = '${deps_dir}/pyyaml',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )


install_scons = Task(
    'Install Scons',
    namespaces = 'scons',
    targets = '${py_site_packages}/SCons',
    workdir = '${deps_dir}/scons',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'
                '  --standard-lib'],
    dependencies = (setup_directories,)
    )

install_zope_interface = Task(
    'Install Zope.Interface',
    namespaces = 'zope_interface',
    targets = '${py_site_packages}/zope',
    workdir = '${deps_dir}/zope_interface',
    commands = [sys.executable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_python_modules = Task(
    'Install Python Modules',
     namespaces = 'bootstrap',
     workdir = '${buildoutdir}',
     commands = [],
     dependencies = (install_pygccxml, install_pyplusplus, install_pyserial,
                     install_pyyaml, install_scons, install_zope_interface)
    )

# Generate Environment File
gen_setenv = Task(
      'Generate setenv',
      namespaces = 'bootstrap',
      workdir = '${buildoutdir}',
      targets = '${buildoutdir}/scripts/setenv',
      commands = [Template('${buildoutdir}/scripts/setenv.template',
                           '${buildoutdir}/scripts/setenv',
                           ram_environ_root = '${ram_prefix}',
                           local_svn_dir = '${buildoutdir}',
                           py_version_str = '${python_version_str}')],
   )
