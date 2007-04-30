PYPLUSPLUS_ROOT=https://pygccxml.svn.sourceforge.net/svnroot/pygccxml
MRBC_VENDOR_SVN=https://ram.umd.edu/svn/mrbc/vendor
PYTHON_OGRE_SVN=https://svn.python-ogre.org/trunk/python-ogre 

# Grab Revision
PYPP_VER_DIR=/tmp/pyverchk
PYTHON_OGRE_VER_DIR=/tmp/python-ogre-verchk
svn co $PYPLUSPLUS_ROOT/developer_scripts $PYPP_VER_DIR
PYPPREVISION=`svnversion $PYPP_VER_DIR`
svn co $PYTHON_OGRE_SVN/shared_ptr $PYTHON_OGRE_VER_DIR
PYTHON_OGRE_REV=`svnversion $PYTHON_OGRE_VER_DIR`

rm -rf $PYPP_VER_DIR $PYTHON_OGRE_VER_DIR

PYGCCXML_DIR=/tmp/pygccxml-r$PYPPREVISION
PYPLUSPLUS_DIR=/tmp/pyplusplus-r$PYPPREVISION
PYTHON_OGRE_DIR=/tmp/pythonogre-r$PYTHON_OGRE_REV

# Export the needed libraries
svn export $PYPLUSPLUS_ROOT/pygccxml_dev $PYGCCXML_DIR
svn export $PYPLUSPLUS_ROOT/pyplusplus_dev $PYPLUSPLUS_DIR
svn export $PYTHON_OGRE_SVN $PYTHON_OGRE_DIR 

# Remove extra Python-Ogre stuff
rm -rf $PYTHON_OGRE_DIR/SourceSnapShots/* $PYTHON_OGRE_DIR/code_generators/cache/*

# Call svn_load_dirs.pl
svn_load_dirs -t $PYPPREVISION $MRBC_VENDOR_SVN/pygccxml current $PYGCCXML_DIR
svn_load_dirs -t $PYPPREVISION $MRBC_VENDOR_SVN/pyplusplus current $PYPLUSPLUS_DIR
svn_load_dirs -t $PYTHON_OGRE_REV $MRBC_VENDOR_SVN/python-ogre current $PYTHON_OGRE_DIR

# Merge changes into local folders
#svn merge $MRBC_VENDOR_SVN/pygccxml/current