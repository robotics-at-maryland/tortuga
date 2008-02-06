Array info utility
==================

Author: Matthias Baas (baas@ira.uka.de)

This directory contains a small utility that can be used to determine
the array size of a function argument. For example, suppose you have
a C function that is defined as:

  void foo(double v[3]);

When this function is parsed by gccxml the type of "v" will just be
reported as "double *" and the array size has got lost.
However, for creating Python wrappers the array size is a valuable
information that can be exploited for creating appropriate wrapper
functions automatically.

The command line utility "createarrayinfo.py" can be used to parse
the original header files and extract just the above array sizes
from function definitions. This information is stored in a file that
can then be read by your own Python scripts that create Python bindings
using pyplusplus. In your own scripts you can use the "arrayinfo" module
to read the info file and query the array size of a particular argument
from a particular function (given as a pygccxml declaration object).

See the comments and doc strings in the Python files to get more 
usage information.

NOTE: While the utility did work for me, it is not as general to deal
with all eventualities (it only uses a very simple C/C++ parser).
So it might well be that you have to adjust the code in 
createarrayinfo.py to your own needs.

