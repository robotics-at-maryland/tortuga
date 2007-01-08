#!/usr/bin/env python
#
# This is the distutils setup script for python-ogre.
# Full instructions are in "install.txt" or "install.html"
#
# To configure, compile, install, just run this script.

DESCRIPTION = """Python-Ogre is a Python wrapper module for the
OGRE 3D Graphics library. It contains python functions and classes
that will allow you to fully utilitize the Ogre library.  It is 
Unique in that the python wrappers are created automatically by
Py++, hence making maintainence etc very simple."""

METADATA = {
    "name":             "Python-Ogre",
    "version":          "0.5 Release",
    "license":          "LGPL",
    "url":              "http://python-ogre.python-hosting.com/",
    "author":           "Lakin Wecker, Roman Yakovenko, Andy Miller",
    "author_email":     "python-ogre-developers@googlegroups.com",
    "description":      "Py++ Ogre Python Binding",
    "long_description": DESCRIPTION,
}
from distutils.core import setup
import os, sys

def GetFileList ( basepath ):
    filesout = []
    for root, dirs, files in os.walk(basepath):
        for name in files:
            filesout.append(os.path.join(root, name))
    return filesout

base = os.path.join(os.getcwd(), os.path.dirname(sys.argv[0]) )
demofiles = GetFileList ( os.path.join (base, "demos") )

PACKAGEDATA = {
       "packages":    ['Ogre'],
       "package_dir": {'': 'packages'},
       "package_data": {'': ['*.pyd', '*.dll']},
#       "data_files": [(base+'Demos', demofiles),]

}

PACKAGEDATA.update(METADATA)
setup(**PACKAGEDATA)
 

 