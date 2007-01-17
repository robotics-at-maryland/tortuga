import os
import sys
import getpass

root_dir = None # python-ogre root directory 
gccxml_bin = None #path to gccxml binary
python_include_dir = None #path to python include files directory
pyplusplus_install_dir = None #path to pygccxml & Py++ packages

generated = None #path to generated files dir
shared_ptr_dir = None #path to shared_ptr directory
PythonOgreMajorVersion = "0"
PythonOgreMinorVersion = "7" # change to 0.7 due to lowercase properties
PythonOgrePatchVersion = "0"


class ogre:
    root_dir = None
    include_dirs = None
    libs_dir = None
    dlls_dir = None
    generated_dir = None
    version = "CVS"
    #version = "1.2.3"
    cache_file = None
    libs_files = ["ogremain", "ogreguirenderer", "ceguibase", "newton"]

class ois:
    version= "1.0"
    
#class ogreode:
#    version= "1.0"
    

class CEGUI:
    version = "0.5.0" 
#class ode:
#    version= "0.7"
    
class newton:
    version= "1.0"
    
#class FMOD:
#    version= "1.0"
    
#class ogrerefapp:
#    version = "1.2.3"

class ogrenewt:
    version = "1.0"
        

# We have removed all unneeded projects
projects = {
    'ois' : ois
    , 'ogre' : ogre
    , 'CEGUI' : CEGUI
#    , 'ode' : ode
    , 'newton' : newton
#    , 'ogrerefapp' : ogrerefapp
    , 'ogrenewt' : ogrenewt
#    , 'FMOD' : FMOD
#    , 'ogreode' : ogreode
}

def shell_backquote(_command):
    command = _command
    if type(_command) is not list:
        command = _command.split(' ')
    # Run the command and check the return code
    process = Popen(command, stdout=PIPE)
    result = process.communicate()[0]
    
    if process.returncode != 0:
        raise 'Shell command "%s" failed' % _command
    return result
    

if os.name == 'posix':
    _env_root = os.path.abspath(os.path.join(os.environ["MRBC_SVN_ROOT"],
                                             'deps', 'local'))

    # Path to python-ogre root directory found automatically
    _relative_path = os.path.dirname(sys.modules[__name__].__file__)
    root_dir = os.path.abspath(_relative_path) 
    python_include_dir = '/usr/include/python2.4'

    # Attempt automatic detection of gccxml
    # try:
    #    gccxml_bin = shell_backquote('which gccxml');
    #except:
    #    raise "Could not find gccxml on the path"
    gccxml_bin = os.path.join(_env_root, 'bin', 'gccxml')

    # Base directories
    _base_include_dir = os.path.join(_env_root, 'include')
    _ogre_include_dir = os.path.join(_base_include_dir, 'OGRE')
    _boost_include_dir = os.path.join(_base_include_dir, 'boost-1_35', 'boost')
    boost_include_dir = os.path.join(_base_include_dir, 'boost-1_35')
    # Include Directories
    ogre.include_dirs = [_ogre_include_dir]

    CEGUI.include_dirs = \
        [os.path.join(_base_include_dir, 'CEGUI'),
         _ogre_include_dir,
         os.path.abspath(os.path.join(root_dir,
              '../ogre/Samples/Common/CEGUIRenderer/include')),
         _boost_include_dir,
         _base_include_dir]

    ogrenewt.include_dirs = \
        [os.path.join(_env_root, 'include', 'OgreNewt'),
         _ogre_include_dir,
         os.path.join(_env_root, 'include'), # Newton SDK include
         _boost_include_dir,
         python_include_dir]

    ois.include_dirs = [os.path.join(_env_root, 'include', 'OIS')]
    
else:
    raise "OS Not Supported"

sys.path.append( os.path.join( root_dir, 'common_utils' ) )
# This is not needed we have these packages installed globally
#sys.path.append( pyplusplus_install_dir + r'/pygccxml_dev' )
#sys.path.append( pyplusplus_install_dir + r'/pyplusplus_dev' )

shared_ptr_dir = os.path.join( root_dir, 'shared_ptr' )
generated_dir = os.path.join( root_dir, 'generated' )
#generated_dir = 'c:/development/boost/libs/pythonogre'

declarations_cache_dir = os.path.join( root_dir, 'code_generators', 'cache' )

# Generate the per project root, generated and cache dirs
for name, cls in projects.items():
    cls.root_dir = os.path.join( root_dir, 'code_generators', name )
    cls.generated_dir = os.path.join( generated_dir, name + '_' + str(cls.version) )
    cls.cache_file = os.path.join( declarations_cache_dir, name + '_' + str(cls.version) + '_cache.xml' )
    cls.package_dir = os.path.join(root_dir, 'packages', name )

