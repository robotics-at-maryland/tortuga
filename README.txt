bootstrap.py
by Joseph Lisee

This is a script which gets from a blank environment to one capable of compiling
our code and running of programs.  Currently it setups os the /opt/ram directory
structure, installs needed python modules, and generates a 'setenv' file which
is used to make sure you pull in the proper libraries and executables.

Please see 'python bootstrap.py --help' for more information on the exact 
options the program takes.

Future work:
 * Integrate with server to download the needed third party libraries for the 
   proper platform.
 * Integrate with package management systems to install the needed packages