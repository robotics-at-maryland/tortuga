import os
import sys
import getpass

<<<<<<< .working
root_dir = None # python-ogre root directory 
gccxml_bin = None #path to gccxml binary
python_include_dir = None #path to python include files directory
pyplusplus_install_dir = None #path to pygccxml & Py++ packages
=======
>>>>>>> .merge-right.r272

##
##  Jan 2007 modification
##
##  This will now look for external config files.
## 
## first it will look for PythonOgreConfig_<username>.py
##
## if that fails it will look for PythonOgreConfig_<platform>.py
##
## otherwise it will use the values you enter below...
##

_LOGGING_ON = False

def log ( instring ):
    if _LOGGING_ON:
        print __file__, "LOG::", instring

PythonOgreMajorVersion = "0"
PythonOgreMinorVersion = "7" # change to 0.7 due to lowercase properties
PythonOgrePatchVersion = "2"

##
## these should be fine with auto create - however override them as necessary
##
PATH_Python = os.path.dirname( sys.executable )
python_include_dirs = os.path.join ( PATH_Python, 'include')
python_lib_dirs = os.path.join ( PATH_Python, 'libs' )
root_dir = os.path.abspath(os.path.dirname(__file__) )## The root directory is where this module is located

sys.path.append( os.path.join( root_dir, 'common_utils' ) )
shared_ptr_dir = os.path.join( root_dir, 'shared_ptr' )
generated_dir_name = 'generated'
package_dir_name = 'packages'
generated_dir = os.path.join( root_dir, generated_dir_name )
declarations_cache_dir = os.path.join( root_dir, 'code_generators', 'cache' )

_UserName = getpass.getuser()
_ConfigSet = False
_SystemType = os.name ## nt or posix or mac
_PlatformType = sys.platform ## win32, ??

##
## Now we load the user specific stuff
##

# log ( "PATH_Python: %s, UserName: %s, SystemType: %s, Root_dir: %s" % (PATH_Python, _UserName, _SystemType, root_dir) )
try:
    s = 'PythonOgreConfig_' + _UserName + '.py'
    execfile( os.path.join(root_dir, s ) )
    _ConfigSet = True
    log ( "Loaded Config (based on username) from %s" % (s))
except:
    try:
        s= 'PythonOgreConfig_' + _SystemType + '.py'
        execfile( os.path.join(root_dir, s ) )
        _ConfigSet = True
        log ( "Loaded Config (based on systemtype) from %s" % (s))
    except:
        pass   

if not _ConfigSet:
    log ( "  Going to use internal config setting")
    ## These need to be adjusted based upon your system
    ##
    ## PLEASE use an external PythonOgreConfig_<username>.py to hold these value !!!!
    ## 
    print "\n\n You DO need to create a PythonOgreConfig_%s.py file with config details", _SystemType
    sys.exit(-1)
    
        
######################

##  Hopefully we can make the rest of this 'automagic'  ##

######################
                    
class ogre:
<<<<<<< .working
    root_dir = None
    include_dirs = None
    libs_dir = None
    dlls_dir = None
    generated_dir = None
    version = "CVS"
    #version = "1.2.3"
    cache_file = None
    libs_files = ["ogremain", "ogreguirenderer", "ceguibase", "newton"]
=======
    active = True
    version = "1.4"   # "1.2"
    libs=[LIB_Boost, 'OgreMain', 'ode', 'OgreGUIRenderer', 'CEGUIBase']
    lib_dirs = [ PATH_LIB_Boost
                , os.path.join( PATH_Ogre, 'Samples/Common/CEGUIRenderer/lib')
                , os.path.join( PATH_Ogre, 'OgreMain/lib/Release' )
                , os.path.join( PATH_Ogre, 'Dependencies/lib/Release')
                ]
    include_dirs = [ PATH_Boost 
                , os.path.join(PATH_Ogre,'OgreMain/include') 
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    LINKFLAGS = ''
    CheckIncludes=['boost/python.hpp', 'Ogre.h']
>>>>>>> .merge-right.r272

     
class ois:
    active = True
    version= "1.0"
    libs=['OIS']
    include_dirs = [ PATH_Boost 
            , os.path.join(PATH_OIS,'includes')    ## Note the plural include's
            ]
    if os.name =='nt':
        _lpath='dll'
    elif os.name == 'posix':
        _lpath='lib'   
    elif os.name =='mac':
        _lpath='Mac' ## TOFIX
    lib_dirs = [ PATH_LIB_Boost 
            , os.path.join(PATH_OIS, _lpath)
            ]
    ModuleName = 'OIS'
    CheckIncludes=['boost/python.hpp', 'OIS.h']
        
class ogrerefapp:
    active = True
    version = "1.4"
    libs=[LIB_Boost, 'OgreMain', 'ode', 'OgreGUIRenderer', 'CEGUIBase', 'ReferenceAppLayer']
    lib_dirs = [ PATH_LIB_Boost
                , os.path.join( PATH_Ogre, 'Samples/Common/CEGUIRenderer/lib')
                , os.path.join( PATH_Ogre, 'OgreMain/lib/Release' )
                , os.path.join( PATH_Ogre, 'Dependencies/lib/Release')
                , os.path.join( PATH_Ogre, 'ReferenceApplication/ReferenceAppLayer/lib/Release')
                ]
    include_dirs = [ PATH_Boost 
                    , os.path.join(PATH_Ogre,'OgreMain/include') 
                    , os.path.join(PATH_Ogre,'ReferenceApplication/ReferenceAppLayer/include') 
                    , os.path.join( PATH_Ogre, 'Dependencies/include')
                    ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'OgreRefApp'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'OgreReferenceAppLayer.h', 'OIS/OIS.h']
    
<<<<<<< .working
#class ogreode:
#    version= "1.0"
=======
class ogrenewt:
    active=True
    version = "1.0"
    libs = ['newton', LIB_Boost, 'OgreNewt_Main', 'OgreMain']
    if os.name =='nt':
        _lpath='dll'
    elif os.name =='posix':
        _lpath='lib-mt'  
    elif os.name == 'mac':
        print "WARNING: Newton doesn't support MAC's"
    include_dirs = [PATH_Boost
                    , PATH_Newton   # only one path for Newton
                , os.path.join(PATH_Ogre,'OgreMain/include') 
                , os.path.join(PATH_OgreAddons,'ogrenewt/OgreNewt_Main/inc')
                    ]
    lib_dirs = [ PATH_LIB_Boost
                , os.path.join(PATH_Newton ,_lpath)
                ,os.path.join(PATH_OgreAddons, r'ogrenewt/OgreNewt_Main/lib/Release') 
                , os.path.join( PATH_Ogre, 'OgreMain/lib/Release' )
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'OgreNewt'
    CheckIncludes=['boost/python.hpp', 'Ogre.h', 'OgreNewt.h', 'Newton.h']
>>>>>>> .merge-right.r272
    
class CEGUI:
<<<<<<< .working
    version = "0.5.0" 
#class ode:
#    version= "0.7"
=======
    active = True
    version = "0.5.0b" 
    libs=[LIB_Boost, 'CEGUIBase', 'OgreMain', 'OgreGUIRenderer' ]
    include_dirs = [PATH_Boost
                    ,os.path.join(PATH_CEGUI, r'include')
                    ,PATH_CEGUI
                    ,os.path.join ( PATH_Ogre, r'Samples/Common/CEGUIRenderer/include' )
                    , os.path.join(PATH_Ogre,'OgreMain/include')
                    ]
                  
    lib_dirs = [ PATH_LIB_Boost
                , os.path.join ( PATH_Ogre, r'Samples/Common/CEGUIRenderer/lib' )
                , os.path.join ( PATH_Ogre, r'OgreMain/lib/Release' )
                , os.path.join ( PATH_Ogre, r'Dependencies/lib/Release' )
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'CEGUI'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'CEGUI.h', 'OgreCEGUIRenderer.h'] 

                    
#############  these are under construction and DO NOT WORK (Yet) #####################  
  
class ode:
    version= "0.7"
>>>>>>> .merge-right.r272
    include_dirs = [r'c:/development/ode/include']
    ModuleName = 'ODE'
    active=False
class newton:
    version= "1.0"
    active=False
    include_dirs = [r'c:/development/newtonsdk/sdk']
    ModuleName = 'NEWTON'
class ogreode:
    version= "1.0"
    include_dirs = [r'c:/development/ocvs/ogreaddons/ogreode/include',
                    r'c:/development/ocvs/ogreaddons/ogreode/prefab/include',
                    r'c:\development\ocvs\ogrenew\ogremain\include',
                    r'c:/development/ode/include']
    ModuleName='OgreOde'
    active=False
    
<<<<<<< .working
#class FMOD:
#    version= "1.0"
=======
class FMOD:
    version= "4.06"
    include_dirs=[PATH_Boost
                   ,os.path.join(PATH_FMOD, 'api/inc')]
    lib_dirs = [ PATH_LIB_Boost
                  ,os.path.join(PATH_FMOD, 'api/lib')] 
    CCFLAGS = ' /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" '
    ModuleName = 'FMOD'
    if os.name =='nt':
        libs=[LIB_Boost, 'fmodexL_vc']
    elif os.name =='posix':
        libs=[LIB_Boost, 'libfmodex']
    elif os.name == 'mac':
        pass        #TOFIX
    active=False
############################################################################################

## Here is the master list....
>>>>>>> .merge-right.r272
    
<<<<<<< .working
#class ogrerefapp:
#    version = "1.2.3"

class ogrenewt:
    version = "1.0"
        

# We have removed all unneeded projects
=======
>>>>>>> .merge-right.r272
projects = {
    'ois' : ois
    , 'ogre' : ogre
    , 'cegui' : CEGUI
#    , 'ode' : ode
    , 'newton' : newton
#    , 'ogrerefapp' : ogrerefapp
    , 'ogrenewt' : ogrenewt
<<<<<<< .working
#    , 'FMOD' : FMOD
#    , 'ogreode' : ogreode
=======
    , 'fmod' : FMOD
    , 'ogreode' : ogreode
>>>>>>> .merge-right.r272
}        

<<<<<<< .working
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

=======
#
# let's setup some defaults
#    
def CheckPaths ( cls , name):
    """ lets check we can find files listed in the CheckIncludes list somewhere in the include_dirs directories
    also look for libs in the lib_dirs.
    This way we can warn people early if they have a configuration problem"""
    if cls.active:  ## only check active classes
        for incfile in cls.CheckIncludes:
            found = False
            log ( "Checking for %s include file (%s class) in include_dirs" % (incfile, name) )
            for lookdir in cls.include_dirs:
                if os.path.isfile ( os.path.join ( lookdir, incfile ) ):
                    found = True
                    break
            if not found:
                print "WARNING: Unable to find %s include file (%s class) in include_dirs" % (incfile, name)
        if os.name =='nt': 
            for libfile in cls.libs :
                libfile += '.lib'
    #             elif os.name == 'posix':
    #                 libfile += '.a'
    #             elif os.name == 'mac':
    #                 libfie += '' # I don't know what to do here 
                found = False
                log ( "Checking for %s library (%s class) in lib_dirs" % (libfile, name) )
                for lookdir in cls.lib_dirs:
                    if os.path.isfile ( os.path.join ( lookdir, libfile ) ):
                        found = True
                        break
                if not found:
                    print "WARNING: Unable to find %s library (%s class) in lib_dirs" % (libfile, name)
    
        
>>>>>>> .merge-right.r272
# Generate the per project root, generated and cache dirs
for name, cls in projects.items():
    CheckPaths( cls, name )
    cls.root_dir = os.path.join( root_dir, 'code_generators', name )
    cls.dir_name = name + '_' + str(cls.version)
    cls.generated_dir = os.path.join( generated_dir, cls.dir_name )
    cls.cache_file = os.path.join( declarations_cache_dir, cls.dir_name + '_cache.xml' )
    if not hasattr (cls, 'ModuleName'):
        cls.ModuleName = name[0].upper() + name[1:]
    if not hasattr (cls, 'PydName'):
        cls.PydName = '_' + name.lower() + '_'
        if os.name == 'nt':
            cls.PydName = cls.PydName + '.pyd'
        elif os.name == 'posix':
            cls.PydName = cls.PydName + '.so'
        elif os.name =='mac':
            print "WARNING - check the last line of environment.py!!"
    cls.package_dir = os.path.join(root_dir, 'packages', name )

