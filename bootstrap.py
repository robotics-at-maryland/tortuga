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

DEFAULT_TASKS = ['setup_directories', 
                 'install_python_modules', 
                 'gen_setenv']

DEFAULT_PREFIX = util.ram_prefix()
PYTHON_SITE_PACKAGE_SUFFIX = util.site_packages_suffix()

archives = ('boost-1.45.7z', 'fann-2.1.0.7z', 'gccxml-0.9.0.7z',
            'log4cpp-1.0.0.7z', 'python-ogre-1.6.4.7z', 'segment-1.0.7z',
            'unittestpp-1.3.7z')
platforms = {
  ('Ubuntu', '8.04', 'hardy') : 'ubuntu_804',
  ('Ubuntu', '8.10', 'intrepid') : 'ubuntu_810',
  ('Ubuntu', '9.04', 'jaunty') : 'ubuntu_904',
  ('Ubuntu', '9.10', 'karmic') : 'ubuntu_910',
  ('Ubuntu', '10.04', 'lucid') : 'ubuntu_1004',
  ('Ubuntu', '10.10', 'maverick') : 'ubuntu_1010',
  ('Ubuntu', '11.04', 'natty') : 'ubuntu_1104',
  ('LinuxMint', '11', 'katya') : 'ubuntu_1104',
}

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
    try:
        dpkg = subprocess.Popen(['dpkg', '--get-selections'],
                                stdout = subprocess.PIPE)
    except OSError:
        # dpkg doesn't exist
        return

    # setup dependency information
    deps = []
    deps.append(dependency('essential', 'build-essential p7zip'))
    deps.append(dependency('cmake', 'cmake'))
    deps.append(dependency('python', 'python-dev python-numpy'))
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

def download_precompiled(quiet=False):
    # find the correct url
    arch = 'x86' if re.search(r'i\d86', platform.machine()) else 'x86_64'
    if arch == 'x86_64':
        print 'warning: x86_64 architecture is not fully supported'

    distro = platform.linux_distribution()
    if not platforms.has_key(distro):
        print 'error: download feature is not supported for this platform %r' % distro
        return
    distro = platforms[distro]

    file_exists, join_path = os.path.exists, os.path.join
    needed = [x for x in archives
              if not file_exists(join_path('/opt/ram/local', x))]
    if needed:
        url = 'https://ram.umd.edu/software/%s/%s' % (arch, distro)
        print 'authentication for %s' % url
        username = raw_input('username: ')
        password = getpass.getpass('password: ')

        call = subprocess.call
        if quiet:
            def call(*args, **kwargs):
                return subprocess.call(*args, stdout=open(os.devnull), stderr=open(os.devnull), **kwargs)

        for dep in needed:
            dest = join_path('/opt/ram/local', dep)
            subprocess.call(['wget', '--user', username,
                             '--password', password, '--no-check-certificate',
                             '%s/%s' % (url, dep)], cwd = '/opt/ram/local')
            subprocess.call(['7zr', 'x', dest], cwd = '/opt/ram/local')

    print 'success: all archives installed'
    
def main(argv=None):
    # Parse Arguments
    parser = OptionParser()
    parser.set_defaults(task = str(DEFAULT_TASKS), prefix = DEFAULT_PREFIX)
    parser.add_option('-t','--task', nargs = 1,
                      help = 'Bootstrap tasks to run', default = None)
    parser.add_option('-p','--prefix', nargs = 1,    
                      help = 'The prefix to install all packages into'
                             ' [default: %default]')
    parser.add_option('--download', action='store_true', dest='download',
                      help = 'Turn on automatic downloading of dependencies')
    parser.add_option('-q', '--quiet', action='store_true')
    (options, args) = parser.parse_args()

    site_package_dir = os.path.join(options.prefix, PYTHON_SITE_PACKAGE_SUFFIX)

    if options.download:
        setup_dependencies()
        download_precompiled(options.quiet)

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
        tasks = DEFAULT_TASKS

    for task_name in tasks:
        print 'Attempting Task',task_name
        task = getattr(build_tasks, task_name)
        software = Software(task, context)
        software.install()
    
    downloadCV = 'placeholder'
    while downloadCV == 'placeholder' or not(downloadCV == '' or 
                                             downloadCV == 'yes' or 
                                             downloadCV == 'no' or 
                                             downloadCV == 'y' or 
                                             downloadCV == 'n'):
        try:
            downloadCV = str(raw_input(
                    'Download and install OpenCV? (y/n) [n]: ')).strip()
        except Exception as e:
            downloadCV = 'no'
        downloadCV = downloadCV.lower()
        if not (downloadCV == '' or downloadCV == 'yes' or downloadCV == 'no' 
                or downloadCV == 'y' or downloadCV == 'n'):
                print('please input yes, no, y, or n')

    if downloadCV == 'yes' or downloadCV == 'y':
        file_exists, join_path = os.path.exists, os.path.join
        openCVTar = join_path('/opt/ram/local', 'opencv-2.4.6.1.tar.gz')
        openCVExtract = join_path('/opt/ram/local', 'opencv-2.4.6.1')
        openCVBuild = join_path(openCVExtract, 'build')
        if not file_exists(openCVTar):
            subprocess.call(['wget', 'http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.6.1/opencv-2.4.6.1.tar.gz'], cwd = '/opt/ram/local')
        if not file_exists(openCVExtract):
            subprocess.call(['tar', '-xvzf', openCVTar], cwd = '/opt/ram/local')
        if not file_exists(openCVBuild):
            subprocess.call(['mkdir', 'build'], cwd = openCVExtract)
            subprocess.call(['cmake', '..'], cwd = openCVBuild)
            subprocess.call(['make'], cwd = openCVBuild)
            subprocess.call(['sudo', 'make', 'install'], cwd = openCVBuild)
        if not file_exists(openCVTar):
            subprocess.call(['rm', 'opencv-2.4.6.1.tar.gz'], cwd = '/opt/ram/local')
        print ('opencv downloaded and installed in /opt/ram/local')
    return 0

if __name__ == "__main__":
    sys.exit(main())
