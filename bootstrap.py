import os
import sys
from optparse import OptionParser

DEFAULT_PACKAGES = [
    'pygccxml',
    'pyplusplus',
    'pyserial',
    'pyyaml',
    'scons',
    'zope_interface']

DEFAULT_ENVIRON_ROOT = '/opt/ram'


# Check Python Version, ensure 2.5.x
python_version = sys.version_info[0:2]
# assert (2,5) == python_version

root_dir = os.path.abspath(sys.path[0])
deps_dir = os.path.join(root_dir, 'deps')

def makepath(path):
    """ 
    creates missing directories for the given path and
    returns a normalized absolute version of the path.

    - if the given path already exists in the filesystem
      the filesystem is not modified.

    - otherwise makepath creates directories along the given path
      using the dirname() of the path. You may append
      a '/' to the path if you want it to be a directory path.

    from holger@trillke.net 2002/03/18
    """

    from os import makedirs
    from os.path import normpath,dirname,exists,abspath,expanduser

    path = expanduser(path)
    dpath = normpath(dirname(path))
    if not exists(dpath): makedirs(dpath)
    return normpath(abspath(path))


def ensure_directory_structure(environ_root, test = False):
    """
    Makes sure the proper directories are created for the rest of the instal
    process.
    """
    print 'Setting up dependency directories in:', environ_root
    print 'Warning: you will need root access to do this'

    py_directory = 'python%d.%d' % python_version
    directories = [os.path.join('local', 'lib', py_directory, 'site-packages'),
                   os.path.join('local', 'lib', 'pkgconfig'),
                   os.path.join('local', 'include'),
                   os.path.join('local', 'bin'),
                   os.path.join('local', 'man')]
    for dir in directories:
        dir = os.path.join(environ_root, dir)
        print 'Making directory:',dir

        if not test:
            makepath(dir)
    
def install_packages(packages, environ_root, test = False):
    """
    Installs all the needed python packages to the ram specfic python directory
    """
    print 'Installing local python packages'

    prefix_dir = os.path.abspath(os.path.join(environ_root, 'local'))

    cwd = os.getcwd()
    for package in packages:
        # Determine the directory of the packages and then go there
        package_dir = os.path.join(deps_dir, package)
        print 'Changing into:',package_dir
        os.chdir(package_dir)

        # Run setup.py for that package
        command_str = 'python setup.py install --prefix=%s' % prefix_dir
        if test:
            print 'Install command:',command_str
        else:
            os.system(command_str)
        
    # Change back to original directory
    print 'Returning to',cwd
    os.chdir(cwd)

def create_environ_script(environ_root, test = False):
    """
    This uses the setenv.template file to generate a setenv file which can be
    sourced by users to setup there environment.

    It needs the following variables (default/typical given):
        ram_environ_root = /opt/ram
        local_svn_dir = /home/jlisee/ram-code
        py_version_str = python2.5
    """
    
    result_path = os.path.join(root_dir, 'scripts', 'setenv')
    print 'Creating environment file:',result_path

    template_parameters = {
        'ram_environ_root' : environ_root,
        'local_svn_dir' : root_dir,
        'py_version_str' : 'python%d.%d' % python_version
        }

    # Read in template file as a big string then expand template values into it
    template_path = os.path.join(root_dir, 'scripts', 'setenv.template')
    template_file = file(template_path, 'r')
    template = ''.join(template_file.readlines())
    template_file.close()

    output_text = template % template_parameters
    stat_info = os.stat(root_dir)

    if test:
        print 'Template arguments:'
        for k,v in template_parameters.iteritems():
            print '\t',k,':',v
        print 'Changing file ownership to UID: %d GID: %d' % (stat_info.st_uid,
                                                              stat_info.st_gid)
    else:
        # Write results to the setenv file
        result_file = file(result_path, 'w')
        result_file.write(output_text)
        result_file.close()

        # This script is run as root, so change the owner ship back to the user
        # who owns the directory this script is in
        os.chown(result_path, stat_info.st_uid, stat_info.st_gid)

def main(argv=None):
    # Parse Arguments
    parser = OptionParser()
    parser.add_option('-d', '--directories', dest = 'create_directory',
                      action = 'store_true',
                      help = 'Just create basic directory structures in /opt/ram')
    parser.add_option('-i', '--install', dest = 'install_packages',
                      action = 'store_true',
                      help = 'Just install python packages')
    parser.add_option('-e', '--environ', dest = 'setup_environ',
                      action = 'store_true',
                      help = 'Just create environment file')
    parser.add_option('-r', '--root-dir', nargs=1, dest='root_dir',
                      help = 'The root directory into which things are installed',
                      default = DEFAULT_ENVIRON_ROOT)
    parser.add_option('-t', '--test', action = 'store_true', dest = 'testing',
                      help = 'Does NOT make any changes, just shows what will happen')
    (options, args) = parser.parse_args()


    # If the user just one a sub action run it
    done_something = False
    if options.create_directory:
        ensure_directory_structure(options.root_dir, options.testing)
        done_something = True
    if options.install_packages:
        install_packages(DEFAULT_PACKAGES, options.root_dir, options.testing)
        done_something = True
    if options.setup_environ:
        create_environ_script(options.root_dir, options.testing)
        done_something = True

    # If the user hasn't asked for a specific action do everything
    if not done_something:
        ensure_directory_structure(options.root_dir, options.testing)
        install_packages(DEFAULT_PACKAGES, options.root_dir, options.testing)
        create_environ_script(options.root_dir, options.testing)



    return 0
if __name__ == "__main__":
    sys.exit(main())
