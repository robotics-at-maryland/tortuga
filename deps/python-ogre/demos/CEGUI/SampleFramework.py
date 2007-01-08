# This code is in the Public Domain
# it has been changed to a "wrapper" to handle both the 1.2.3 version of Ogre (current SDK) and the CVS version
# which is different in that it uses OIS for all input handling.
#

import Ogre as ogre

try:
    ogre.OgreVersion = ogre.GetOgreVersion()
    ogre.OgreVersionString = ogre.OgreVersion[0] + ogre.OgreVersion[1] + ogre.OgreVersion[2]
    ogre.PythonOgreVersion = ogre.GetPythonOgreVersion()
    from Ogre.sf_OIS import * 
#     if (ogre.OgreVersion[0]+ ogre.OgreVersion[1]) == "12":   # ogre 1.2.3 is latest SDK
#         from Ogre.sf import *
#     else:  # assume it's ogre version 1.3, the CVS version that needs OIS
#         from Ogre.sf_OIS import * 
           
except:
    print "Your version of Python-Ogre doesn't define it's own version and so is rather old!"
    print "It is suggested you visit http://python-ogre.python-hosting.com and upgrade"
    

