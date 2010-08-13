# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  buildfiles/tasks.py

# Python Imports
import os
import os.path
import sys
import platform

# Library Imports
from buildit.task import Task

# Project Imports
from buildfiles.common.commands import *
import buildfiles.variants as variants

pythonExecutable = '"%s"' % sys.executable

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
                Mkdir('${ram_prefix}/man/'),
                Mkdir('${ram_prefix}/pkgs/')],
    )

# Install Python Modules
install_pygccxml = Task(
    'Install Pygccxml',
    namespaces = 'pygccxml',
    targets = '${py_site_packages}/pygccxml',
    workdir = '${deps_dir}/pygccxml',
    commands = [pythonExecutable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_pyplusplus = Task(
    'Install Py++',
    namespaces = 'pyplusplus',
    targets = '${py_site_packages}/pyplusplus',
    workdir = '${deps_dir}/pyplusplus',
    commands = [pythonExecutable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

install_pyyaml = Task(
    'Install PyYaml',
    namespaces = 'pyyaml',
    targets = '${py_site_packages}/yaml',
    workdir = '${deps_dir}/pyyaml',
    commands = [pythonExecutable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )


install_scons = Task(
    'Install Scons',
    namespaces = 'scons',
    targets = '${py_site_packages}/SCons',
    workdir = '${deps_dir}/scons',
    commands = [pythonExecutable + ' setup.py install'
                '  --prefix=${ram_prefix}'
                '  --standard-lib'],
    dependencies = (setup_directories,)
    )

install_zope_interface = Task(
    'Install Zope.Interface',
    namespaces = 'zope_interface',
    targets = '${py_site_packages}/zope',
    workdir = '${deps_dir}/zope_interface',
    commands = [pythonExecutable + ' setup.py install'
                '  --prefix=${ram_prefix}'],
    dependencies = (setup_directories,)
    )

symlink_python = Task(
    'Make Symlink to Proper Python Version',
    namespaces = 'python_symlink',
    targets = '${buildoutdir}/scripts/link_done',
    workdir = '${buildoutdir}',
    commands = ['ln -sf ' + pythonExecutable + 
		' ${buildoutdir}/scripts/python',
		'ln -sf /usr/bin/pydoc2.5 ${buildoutdir}/scripts/pydoc',
		'touch ${buildoutdir}/scripts/link_done'],
    dependencies = (setup_directories,)
    )

symlink_gcc = Task(
    'Make Symlink to Proper GCC Version',
    namespaces = 'gcc_symlink',
    targets = '${buildoutdir}/scripts/gcc',
    workdir = '${buildoutdir}',
    commands = ['ln -sf /usr/bin/gcc-4.2 ${buildoutdir}/scripts/gcc',
                'ln -sf /usr/bin/g++-4.2 ${buildoutdir}/scripts/g++'],
    dependencies = (setup_directories,)
    )

install_python_modules = Task(
    'Install Python Modules',
     namespaces = 'bootstrap',
     workdir = '${buildoutdir}',
     dependencies = (install_pygccxml, install_pyplusplus,
                     install_pyyaml, install_scons, install_zope_interface,
		     symlink_python, symlink_gcc)
    )

# Package Installation

# Small hack to find arch
#arch = 'x86'
#if 'big' == sys.byteorder:
#    arch = 'ppc'

#get_package_list = Task(
#    'Get Package List',
#    namespaces = 'bootstrap',
#    workdir = '${buildoutdir}',
#    commands = [Download('${ram_prefix}/pkgs/packages.txt',
#                         'https://ram.umd.edu/software/' + arch + '/'
#                         variants.get_platform() + '/packages.txt')]
#    dependencies = (setup_directories,)
#    )

#download_packages = Task(
#    'Download Packages',
#    namespaces = 'bootstrap',
#    workdir = '${buildoutdir}',
#    commands = [DownloadPackages('${ram_prefix}/pkgs/packages.txt',
#                                 '${ram_prefix}')]
#    dependencies = (get_package_list,)
#   )

#pkgs = Task(
#    'Install Packages',
#     namespaces = 'bootstrap',
#     workdir = '${buildoutdir}',
#     dependencies = (unpack_packages)
#    )

# Generate Environment File

class CommandReturnedNonZeroError(Exception):
	def __init__(self, errno, cmd):
		self.errno = errno
		self.cmd = cmd
	def __str__(self):
		return '"' + self.cmd + '" returned non-zero: ' + str(self.errno)

def execute_like_backticks(command):
	"""
	Execute a command as if on a command line and return the value 
	that gets written to stdout, much like the backticks operator 
	from certain UNIX shells
	"""
	p = os.popen(command, "r")
	result = p.read()
	exitstatus = p.close()
	if exitstatus is not None:
		raise CommandReturnedNonZeroError(exitstatus, command)
	else:
		return result

def get_wx_prefix():
    if platform.system() == 'Darwin':
    	cmd = "python -c 'import wx;print(wx.__file__)'"
    	try:
            	wx_imported_path = execute_like_backticks(cmd)
    	except CommandReturnedNonZeroError:
			print 'Please install wxPython 2.8.x from http://www.wxpython.org'
			raise
        return os.path.dirname(wx_imported_path)
    else:
        return ''
  
WX_PREFIX = get_wx_prefix()


from distutils import sysconfig
gen_setenv = Task(
      'Generate setenv',
      namespaces = 'bootstrap',
      workdir = '${buildoutdir}',
      targets = '${buildoutdir}/scripts/setenv',
      commands = [Template('${buildoutdir}/scripts/setenv.template',
                           '${buildoutdir}/scripts/setenv',
                           ram_environ_root = '${ram_prefix}',
                           local_svn_dir = '${buildoutdir}',
                           python_prefix = sysconfig.PREFIX,
                           python_version = str(sys.subversion),
                           py_site_packages_suffix = '${py_site_packages_suffix}',
                           wx_bin_dir = WX_PREFIX + os.sep + 'bin',
                           wx_lib_dir = WX_PREFIX + os.sep + 'lib')]
   )

   
def set_windows_environment(listvars, standalone):
    for key,val in listvars.iteritems():
        EditEnvironment(key, val, mode = EditEnvironment.PREPEND).execute()
    
    for key, val in standalone.iteritems():
        EditEnvironment(key, val, mode = EditEnvironment.OVERWRITE).execute()

    # Sections to add to the windows path
    #path = [r'%RAM_SVN_DIR%\build\lib', r'%RAM_ROOT_DIR%\Lib', 
    #        r'%RAM_ROOT_DIR%\Scripts']
    # Sections to add to the python path
    #pythonpath = [r'%RAM_SVN_DIR%\build\lib', r'%RAM_SVN_DIR%\packages\python', 
    #              r'%RAM_ROOT_DIR%\Lib\site-packages']

def rollbak_windows_envrionment(listvars, standalone):
    # Remove all placed variables
    for key, val in listvars.iteritems():
        EditEnvironment(key, val, None, mode = EditEnvironment.REPLACE).execute()
    
    for key, val in standalone.iteritems():
        EditEnvironment(key, None, mode = EditEnvironment.OVERWRITE).execute()
