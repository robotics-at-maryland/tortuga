PYPLUSPLUS_ROOT=https://pygccxml.svn.sourceforge.net/svnroot/pygccxml
MRBC_VENDOR_SVN=https://nearspace.net/svn/mrbc/vendor

# Grab Revision
svn co $PYPLUSPLUS_ROOT/developer_scripts /tmp/pyverchk
REVISION=`svnversion /tmp/pyverchk`

PYGCCXML_DIR=/tmp/pygccxml-r$REVISION
PYPLUSPLUS_DIR=/tmp/pyplusplus-r$REVISION

# Export the needed libraries
svn export  $PYPLUSPLUS_ROOT/pygccxml_dev $PYGCCXML_DIR
svn export  $PYPLUSPLUS_ROOT/pyplusplus_dev $PYPLUSPLUS_DIR

# Call svn_load_dirs.pl
svn_load_dirs -t $REVISION $MRBC_VENDOR_SVN/pygccxml current $PYGCCXML_DIR
svn_load_dirs -t $REVISION $MRBC_VENDOR_SVN/pyplusplus current $PYPLUSPLUS_DIR

# Merge changes into local folders
#svn merge $MRBC_VENDOR_SVN/pygccxml/current