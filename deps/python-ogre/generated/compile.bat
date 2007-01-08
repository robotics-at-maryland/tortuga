REM Batch file to compile python-ogre
REM NOTE: setting BOOST_PYTHON_MAX_ARITY=19 below so read the BUILDING.TXT file
rem call vsvars32
set PYTHON_VERSION=2.5
bjam -sBUILD=release -sBOOST_PYTHON_MAX_ARITY=19