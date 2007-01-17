
# Root of our special build environment
_env_root = os.path.abspath(os.path.join(os.environ["MRBC_SVN_ROOT"],
                                         'deps', 'local'))
# Include Directories 
_base_include_dir = os.path.join(_env_root, 'include')
#_cegui_renderer_include_dir = '../ogre/Samples/Common/CEGUIRenderer/include'

OGRE_INCLUDE_DIRS = [os.path.join(_base_include_dir, 'OGRE')]
BOOST_INCLUDE_DIR = os.path.join(_base_include_dir, 'boost-1_35')
OIS_INCLUDE_DIRS = [os.path.join(_base_include_dir, 'OIS')]
OGRENEWT_INCLUDE_DIRS = OGRE_INCLUDE_DIRS + [_base_include_dir,
                        os.path.join(_base_include_dir,'OgreNewt')]
CEGUI_INCLUDE_DIRS = OGRE_INCLUDE_DIRS + [_base_include_dir,
    os.path.join(_base_include_dir, 'CEGUI')]

# Lib directories
_base_lib_dir = os.path.join(_env_root, 'lib')

OGRE_LIB_DIRS = [_base_lib_dir]
OIS_LIB_DIRS = [_base_lib_dir]
OGRENEWT_LIB_DIRS = [_base_lib_dir]
CEGUI_LIB_DIRS = [_base_lib_dir]

# We have to call GCC-XML
gccxml_bin = os.path.join(_env_root, 'bin', 'gccxml')

# Special Boost variables
LIB_Boost = 'boost_python-gcc'
PATH_Boost = BOOST_INCLUDE_DIR

# Setup ccache as the compiler
cxx_compiler='ccache g++-4.0'
cc_compiler='ccache gcc-4.0'
