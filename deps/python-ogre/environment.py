import os
import sys
import getpass

root_dir = None #python-ogre root directory
gccxml_bin = None #path to gccxml binary
python_include_dirs = None #path to python include files directory
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
    libs_files = ["ogremain", "ogreguirenderer", "ceguibase", "ode", "newton"]

class ois:
    version= "1.0"
    
class ogreode:
    version= "1.0"
    

class CEGUI:
    version = "0.5.0b" 
class ode:
    version= "0.7"
    
class newton:
    version= "1.0"
    
class FMOD:
    version= "1.0"
    
class ogrerefapp:
    version = "1.2.3"

class ogrenewt:
    version = "1.0"
        
       
projects = {
    'ois' : ois
    , 'ogre' : ogre
    , 'CEGUI' : CEGUI
    , 'ode' : ode
    , 'newton' : newton
    , 'ogrerefapp' : ogrerefapp
    , 'ogrenewt' : ogrenewt
    , 'FMOD' : FMOD
    , 'ogreode' : ogreode
}

if 'roman' in getpass.getuser():
    root_dir = r'D:\dev\python-ogre\bindings-dev'
    gccxml_bin = r'D:\dev\gccxml/gccxml.exe'
    python_include_dirs = 'e:/python24/include'
    pyplusplus_install_dir = r'D:\dev\language-binding\sources'

    ogre.include_dirs = [r'D:\dev\python-ogre\sources\ogre_cvs\ogrenew\OgreMain\include']
    ois.include_dirs = r'D:\OisSDK\includes'
else:
    root_dir = r'C:\development\pywork'
    gccxml_bin = r'c:/development/gccxml/bin/release/gccxml.exe'
#    python_include_dirs = 'c:/python25/include'
#    python_include_dirs = 'c:/python24/include'
    pyplusplus_install_dir = r'c:\development\pyplus'

    if ogre.version == "CVS":
        ogre.include_dirs  = [r'c:\development\ocvs\ogrenew\ogremain\include']
        ogreode.include_dirs = [r'c:/development/ocvs/ogreaddons/ogreode/include',
                            r'c:/development/ocvs/ogreaddons/ogreode/prefab/include',
                            r'c:\development\ocvs\ogrenew\ogremain\include',
                            r'c:/development/ode/include']
        ogrerefapp.include_dirs = [r'c:\development\ocvs\ogrenew\ogremain\include',
                           r'c:\development\ocvs\ogrenew\referenceapplication\referenceapplayer\include',
                            r'c:\development\ocvs\ogrenew\dependencies\include']
        ogrerefapp.version="CVS"
        
#         ogre.libs_dirs = [r'c:/development/ocvs/ogrenew/ogremain/lib/release', 
#                         r'c:/development/ocvs/ogrenew/samples/common/ceguirenderer/lib',
#                         r'c:/development/ocvs/ogrenew/dependencies/lib/release',
#                         r'c:/development/boost/boost' ]
        CEGUI.include_dirs = [r'c:/development/cegui-0.5.0/include',
                           ## r'c:/development/ocvs/ogrenew/dependencies/include',
                           ## NOTE: ogre headers expect cegui to live in ./CEGUI so I've copied the cegui include
                           ## tree to ./CEGUI in the following directory..
                            r'c:/development/ocvs/ogrenew/dependencies/include/',
                            r'c:/development/ocvs/ogrenew/samples/common/CEGUIRenderer/include',
                            r'c:/development/ocvs/ogrenew/ogremain/include',
                            r'c:/development/boost'
                            ]
                            
        ogrenewt.include_dirs = [r'c:/development/ocvs/ogreaddons/ogrenewt/OgreNewt_Main/inc',
                            r'c:/development/ocvs/ogrenew/ogremain/include',
                            r'c:/development/newtonsdk/sdk',
                            r'c:/development/boost',
                            r'c:/python25/include'
                            ]

    else:
        ogre.include_dirs  = [r'c:\devold\ogreSDK\include',
                                r'c:\devold\ogreSDK\samples\refapp\include'
                                ]
        CEGUI.include_dirs = [r'c:/devold/cegui-0.5.0/include',
                            r'c:/devold/ogresdk/samples/include',
                            r'c:/devold/ogresdk/include',
                            r'c:/development/boost'
                            ]  
                                    
        ogrerefapp.include_dirs = ogre.include_dir
        
#    CEGUI.include_dir = r'c:/ogresdk/include/CEGUI'
    ode.include_dirs = [r'c:/development/ode/include']
    newton.include_dirs = [r'c:/development/newtonsdk/sdk']
    ois.include_dirs = [r'c:\development\ois\includes']
    FMOD.include_dirs=[r'c:\development\fmod\api\inc']
    


sys.path.append( os.path.join( root_dir, 'common_utils' ) )
sys.path.append( pyplusplus_install_dir + r'/pygccxml_dev' )
sys.path.append( pyplusplus_install_dir + r'/pyplusplus_dev' )

shared_ptr_dir = os.path.join( root_dir, 'shared_ptr' )
generated_dir = os.path.join( root_dir, 'generated' )
#generated_dir = 'c:/development/boost/libs/pythonogre'

declarations_cache_dir = os.path.join( root_dir, 'code_generators', 'cache' )

for name, cls in projects.items():
    cls.root_dir = os.path.join( root_dir, 'code_generators', name )
    cls.generated_dir = os.path.join( generated_dir, name + '_' + str(cls.version) )
    cls.cache_file = os.path.join( declarations_cache_dir, name + '_' + str(cls.version) + '_cache.xml' )

