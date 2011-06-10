# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  bootstrap.py

import os
import sys
import subprocess, re, getpass, platform
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

archives = ('boost-1.45.7z', 'fann-2.1.0.7z', 'gccxml-0.9.0.7z',
            'log4cpp-1.0.0.7z', 'python-ogre-1.6.4.7z', 'segment-1.0.7z',
            'unittestpp-1.3.7z')

class dependency(object):
    def __init__(self, name, packages):
        self._name = name
        if isinstance(packages, str):
            packages = packages.split(' ')
        self.depends = packages

    @property
    def name(self):
        return self._name

    @property
    def install_targets(self):
        return self.depends[:]

def setup_dependencies():
    # run dpkg and only continue if there's no error
    re_install = re.compile(r'([^\s]+)\s+([^\s]+)')
    dpkg = subprocess.Popen(['dpkg', '--get-selections'],
                            stdout = subprocess.PIPE)
    if dpkg.wait():
        return

    # setup dependency information
    deps = []
    deps.append(dependency('essential', 'build-essential p7zip'))
    deps.append(dependency('cmake', 'cmake'))
    deps.append(dependency('python', 'python-dev python-numpy'))
    deps.append(dependency('opencv', 'libcv-dev libhighgui-dev libcvaux-dev'))
    deps.append(dependency('dc1394', 'libdc1394-22-dev'))
    deps.append(dependency('opengl', 'mesa-common-dev libglu1-mesa-dev'))
    deps.append(dependency('libusb', 'libusb-dev'))
    deps.append(dependency('wx', 'python-wxgtk2.8 libwxgtk2.8-dev'))
    # uncomment when fann is fixed in ubuntu
    #deps.append(dependency('fann', 'libfann-dev'))
    deps.append(dependency('fftw', 'libfftw3-dev'))

    # find all packages we're interested in
    interesting_packages = set()
    for d in deps:
        interesting_packages.update(d.install_targets)

    depends = {}
    for name, status in (x.groups()
                         for x in (re_install.search(y)
                                   for y in dpkg.communicate()[0].split('\n'))
                         if x):
        if name in interesting_packages:
            depends[name] = status

    # find what packages we actually need to install
    to_install = [x for x in interesting_packages
                  if not depends.has_key(x) or depends[x] != 'install']

    # execute command to install needed packages (requires root)
    if to_install:
        cmd = ['apt-get', 'install']
        cmd.extend(to_install)
        if os.geteuid() != 0: # insert sudo to get root privelages
            cmd.insert(0, 'sudo')
        subprocess.call(cmd)

def download_precompiled():
    # find the correct url
    arch = 'x86' if re.search(r'i\d86', platform.machine()) else 'x86_64'
    if arch == 'x86_64':
        print 'warning: x86_64 architecture is not supported'

    distro = platform.linux_distribution()
    if distro[0] != 'Ubuntu':
        print 'error: download feature is not supported for anything other than Ubuntu'
        return
    distro = distro[0].lower() + '_' + distro[1].replace('.', '')

    url = 'https://ram.umd.edu/software/%s/%s' % (arch, distro)
    print 'authentication for %s' % url
    username = raw_input('username: ')
    password = getpass.getpass('password: ')

    file_exists, join_path = os.path.exists, os.path.join
    for dep in (x for x in archives):
        dest = join_path('/opt/ram/local', dep)
        if not file_exists(dest):
            subprocess.call(['wget', '--user', username,
                             '--password', password, '--no-check-certificate',
                             '%s/%s' % (url, dep)], cwd = '/opt/ram/local')
            subprocess.call(['7zr', 'x', dest], cwd = '/opt/ram/local')

    print 'success: all archives downloaded'
    
def main(argv=None):
    # Parse Arguments
    parser = OptionParser()
    parser.set_defaults(task = str(DEFULT_TASKS), prefix = DEFAULT_PREFIX)
    parser.add_option('-t','--task', nargs = 1,
                      help = 'Bootstrap tasks to run', default = None)
    parser.add_option('-p','--prefix', nargs = 1,    
                      help = 'The prefix to install all packages into'
                             ' [default: %default]')
    parser.add_option('--download', action='store_true', dest='download',
                      help = 'download precompiled dependencies')
    (options, args) = parser.parse_args()

    site_package_dir = os.path.join(options.prefix, PYTHON_SITE_PACKAGE_SUFFIX)

    setup_dependencies()
    if options.download:
        download_precompiled()

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
    context.globals['python_executable'] = sys.executable
        

    # Determing and run build tasks
    if options.task is not None:
        tasks = [options.task]
    elif len(args) > 0:
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
