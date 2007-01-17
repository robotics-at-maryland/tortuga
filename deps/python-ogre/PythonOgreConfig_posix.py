import sys

print "######################\n\n"
print "You need to edit this file to match your system\n",__file__,"\n\n"
print "######################\n\n"
sys.exit(-1)

## Boost stuff 
## path to the root of your boost dir, should have 'libs', 'boost' subdirectories
PATH_Boost = r'/usr/boost'    
## Path to your boost_pythonxxxx lib file
PATH_LIB_Boost = r'/usr/boost/bin.v2/libs/python/build/???????????'
## and the name of the boost python library
LIB_Boost = 'boost_python??????????'

## Ogre Home, should 'Samples', 'OgreMain' subdirectories
PATH_Ogre = r'/usr/development/Ogrenew'

## ogrenewt should be a subdirectory under here
PATH_OgreAddons = r'/usr/development/OgreAddons'

PATH_CEGUI = r'/usr/development/cegui-0.5.0'
PATH_OIS = r'/usr/development/ois'
PATH_Newton = r'/usr/development/newtonsdk/sdk'

#
# Don't forget we need to call gccxml......
gccxml_bin = r'/usr/gccxml/bin/release/gccxml'
