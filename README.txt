Introduction:

Welcome to the Robotics @ Maryland source control system.  It a multilanguage, 
multiplaform code base for control robotics system.  It has a build system
designed to place minimum work load on the developer.  This is important
because the code base uses several open source libraries and has many
inter-connected subcomponents.

Getting Started:

In order to get started you need to run the 'bootstrap.py' script which gets
you from a blank environment to one capable of compiling our code and running 
its programs (*).  It will installs all dependencies to the directory
you specify (/opt/ram/local is default) it also generates a 'setenv' file which
is used to make sure you pull in the proper libraries and executables.

Please see 'python bootstrap.py --help' for more information on the exact 
options the program takes.

 (*) Current it will not downloaded the extra software from the
   https://ram.umd.edu/software/<arch>/<platform> dirctory, this needs to be
   done manually.

Compiling the Code:

After having run the bootstrap script, and having it complete with no errors
you need to source the generated 'setenv' script, you do this by running
"srouce scripts/setenv".  This needs to be done for every new terminal you
open Now you can build the code by running 'scons' in the root directory.  
This will build everything.  SCons also contains autoconf like functionality 
and by default our build system will ensure all dependencies are properly 
installed before attempting to compile anything.

Auther:  Joseph Lisee
