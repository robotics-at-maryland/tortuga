import os
import sys
import getpass

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

_LOGGING_ON = True

def log ( instring ):
    if _LOGGING_ON:
        #print __file__, "LOG::", instring
        print "LOG::", instring

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
user_config = 'PythonOgreConfig_' + _UserName ## + '.py'
plat_config = 'PythonOgreConfig_' + _SystemType ## + '.py'

if os.path.exists(user_config + '.py'):
    Config = __import__(  s  )
    _ConfigSet = True
    log ( "Loaded Config (based on username) from %s" % (user_config))
elif os.path.exists(plat_config + '.py'):
    execfile(user_config)
    _ConfigSet = True
    log ( "Loaded Config (based on systemtype) from %s" % (plat_config))

if not _ConfigSet:
    log ( "  Going to use internal config setting")
    ## These need to be adjusted based upon your system
    ##
    ## PLEASE use an external PythonOgreConfig_<username>.py to hold these value !!!!
    ## 
    print "\n\n You DO need to create a PythonOgreConfig_%s.py file with config details" % ( _SystemType)
    sys.exit(-1)
    
        
######################

##  Hopefully we can make the rest of this 'automagic'  ##

######################
                    
class ogre:
    active = True
    version = "1.4"   # "1.2"
    libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'OgreGUIRenderer', 'CEGUIBase']
    lib_dirs = [ Config.PATH_LIB_Boost
                ,  Config.PATH_LIB_Ogre_CEGUIRenderer
                , Config.PATH_LIB_Ogre_OgreMain
                , Config.PATH_LIB_Ogre_Dependencies 
                ]
    include_dirs = [ Config.PATH_Boost 
                , Config.PATH_INCLUDE_Ogre 
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    LINKFLAGS = ''
    CheckIncludes=['boost/python.hpp', 'Ogre.h']

     
class ois:
    active = True
    version= "1.0"
    libs=['OIS']
    include_dirs = [ Config.PATH_Boost 
            , Config.PATH_INCLUDE_OIS
            ]
    lib_dirs = [ Config.PATH_LIB_Boost 
            , Config.PATH_LIB_OIS
            ]
    ModuleName = 'OIS'
    CheckIncludes=['boost/python.hpp', 'OIS.h']
        
class ogrerefapp:
    active = True
    version = "1.4"
    libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'ReferenceAppLayer']
    lib_dirs = [ Config.PATH_LIB_Boost
                , Config.PATH_LIB_Ogre_OgreMain
                , Config.PATH_LIB_ODE
                , Config.PATH_LIB_OgreRefApp
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,Config.PATH_INCLUDE_Ogre
                    ,Config.PATH_INCLUDE_OgreRefApp
                    ,Config.PATH_INCLUDE_ODE
                    ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'OgreRefApp'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'OgreReferenceAppLayer.h', 'ode/ode.h']
    
class ogrenewt:
    active=True
    version = "1.0"
    libs = ['newton', Config.LIB_Boost, 'OgreNewt_Main', 'OgreMain']
    include_dirs = [Config.PATH_Boost
                    , Config.PATH_Newton   # only one path for Newton
                    , Config.PATH_INCLUDE_Ogre 
                    , Config.PATH_INCLUDE_OgreNewt
                    ]
    lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Newton
                ,Config.PATH_LIB_OgreNewt
                , Config.PATH_LIB_Ogre_OgreMain
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'OgreNewt'
    CheckIncludes=['boost/python.hpp', 'Ogre.h', 'OgreNewt.h', 'Newton.h']

class CEGUI:
    active = True
    version = "0.5.0b" 
    libs=[Config.LIB_Boost, 'CEGUIBase', 'OgreMain', 'OgreGUIRenderer' ]
    include_dirs = [Config.PATH_Boost
                    ,Config.PATH_INCLUDE_CEGUI
                    ,Config.PATH_CEGUI
                    , Config.PATH_INCLUDE_Ogre_CEGUIRenderer
                    , Config.PATH_INCLUDE_Ogre
                    , Config.PATH_INCLUDE_Ogre_Dependencies
                    ]
                  
    lib_dirs = [ Config.PATH_LIB_Boost
                ,  Config.PATH_LIB_Ogre_CEGUIRenderer
                , Config.PATH_LIB_Ogre_OgreMain
                ,  Config.PATH_LIB_Ogre_Dependencies
                ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    ModuleName = 'CEGUI'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'CEGUI.h', 'OgreCEGUIRenderer.h'] 

                    
#############  these are under construction and DO NOT WORK (Yet) #####################  
  
class ode:
    version= "0.7"
    include_dirs = [r'c:/development/ode/include']
    libs=[Config.LIB_Boost,  'ode']
    lib_dirs = [ Config.PATH_LIB_Boost
                ,  Config.PATH_LIB_ODE
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,  Config.PATH_INCLUDE_ODE
                    , Config.PATH_INCLUDE_ODESOURCE ## some header files are in the source tree ??
                    ]

    ModuleName = 'ODE'
    CheckIncludes = ['boost/python.hpp',  'ode/ode.h'] 
    active=False
class newton:
    version= "1.0"
    active=False
    include_dirs = [r'c:/development/newtonsdk/sdk']
    ModuleName = 'NEWTON'
class ogreode:
    version= "1.0"
    lib_dirs = [ Config.PATH_LIB_Boost
                , Config.PATH_LIB_OgreOde
                , Config.PATH_LIB_OgreOdePrefab
                , Config.PATH_LIB_OgreOdeLoader
                , Config.PATH_LIB_Ogre_OgreMain
                ,  Config.PATH_LIB_ODE
                ]
    include_dirs = [ Config.PATH_Boost 
                , Config.PATH_INCLUDE_ODE
                , Config.PATH_INCLUDE_OgreOde
                , Config.PATH_INCLUDE_OgreOdePrefab
                , Config.PATH_INCLUDE_OgreOdeLoader
                , Config.PATH_INCLUDE_Ogre
                ]

    libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'OgreOde_Core', 'OgreOde_Prefab', 'OgreOde_Loader' ]
    CCFLAGS =  ' -D"BOOST_PYTHON_MAX_ARITY=19"'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'ode/ode.h', 'ode/odecpp.h', 'OgreOde_Core.h', 'OgreOde_Loader.h', 
                    'Ogreode_Prefab.h'] 
                     
    ModuleName='OgreOde'
    active=False
    
class fmod:
    version= "4.06"
    include_dirs=[Config.PATH_Boost
                   ,Config.PATH_INCLUDE_FMOD
                   ]
    lib_dirs = [ Config.PATH_LIB_Boost
                  ,Config.PATH_LIB_FMOD
                  ] 
                 
    CCFLAGS = ' /D "NDEBUG" /D "WIN32" /D "_MBCS" '
    ModuleName = 'FMOD' 
    CheckIncludes = ['fmod.h']
    active=True
    
class ogreal:
    version="0.3"
    include_dirs = [ Config.PATH_Boost
                , Config.PATH_INCLUDE_Ogre
                , Config.PATH_INCLUDE_OgreAL
                , Config.PATH_INCLUDE_OGG
                , Config.PATH_INCLUDE_VORBIS
                , Config.PATH_INCLUDE_OPENAL
                , Config.PATH_INCLUDE_ALUT
                ]    
    lib_dirs = [ Config.PATH_LIB_Boost
                  ,Config.PATH_LIB_Ogre_OgreMain
                  , Config.PATH_LIB_OgreAL
                  ] 
    libs=[Config.LIB_Boost, 'OgreMain', 'OgreAL']
    ModuleName = 'OgreAL'
    CheckIncludes = ['OgreAL.h']
    active=False
                
############################################################################################

## Here is the master list....
    
projects = {
    'ois' : ois
    , 'ogre' : ogre
    , 'CEGUI' : CEGUI
#    , 'ode' : ode
#    , 'newton' : newton
#    , 'ogrerefapp' : ogrerefapp
    , 'ogrenewt' : ogrenewt
#    , 'fmod' : fmod
#    , 'ogreode' : ogreode
#    , 'ogreal' : ogreal
}        

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
                print "WARNING: Unable to find %s include file (%s class) in include_dirs in path:" % (incfile, name)
                for p in cls.include_dirs:
                    print "\t",p
        if os.name =='nt': 
            for libfile in cls.libs :
                libfile += '.lib'
                found = False
                log ( "Checking for %s library (%s class) in lib_dirs" % (libfile, name) )
                for lookdir in cls.lib_dirs:
                    if os.path.isfile ( os.path.join ( lookdir, libfile ) ):
                        found = True
                        break
                if not found:
                    print "WARNING: Unable to find %s library (%s class) in lib_dirs" % (libfile, name)
    
#
# a couple of specials that should be done differently
gccxml_bin = Config.gccxml_bin
pyplusplus_install_dir = Config.pyplusplus_install_dir                    
                            
for name, cls in projects.items():
    # little hack to allow overriding of settings from the config file
    if hasattr(Config, name):   # look to see if the class exists in the config file
        print "Using Override for class ", name
        _class = Config.__dict__[name]  # get the class
        for key, value in _class.__dict__.items():
            if not key.startswith('_'):
                cls.__dict__[key] = value   
                print "Set %s.%s to %s" % (name, key, value) 
                
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

