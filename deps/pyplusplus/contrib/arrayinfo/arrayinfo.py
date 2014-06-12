######################################################################
# Array info
#
# Author: Matthias Baas (baas@ira.uka.de)
######################################################################

import os.path

# ArrayInfo
class ArrayInfo:
    """Reads the information that was created by createarrayinfo.py.

    Usage:

    Create an instance of this class and either provide the file
    name in the constructor or call read() manually. After the info
    file has been read you can call arraySize() to retrieve the
    size of a particular argument.
    """

    def __init__(self, datfile=None):
        """Constructor.

        datfile is the name of the file that contains the array size
        information (as created by the createarrayinfo.py utility).
        If a name is provided is already read in (so there's no need
        to manually call read() anymore).
        """

        # Key: (filename, firstline, funcname)
        self.funcargs = {}

        if datfile!=None:
            self.read(datfile)

    # read
    def read(self, datfile):
        """Read an info file.
        """

        n = 0
        for s in file(datfile, "rt"):
            a = s.split(";")
            filename, firstline, lastline, classname, funcname, args = a
            firstline = int(firstline)
            lastline = int(lastline)
            args = eval(args)
            key = (filename, lastline, funcname)
            self.funcargs[key] = args
            n += 1
        print n,"entries read from",datfile

    # arraySize
    def arraySize(self, decl, idx):
        """Return the array size of an argument.

        decl is the pygccxml calldef declaration whose argument list
        should be inspected. idx is the index of the argument (0-based).
        The return value is None if the argument is no array or the
        array size is not known, otherwise the size of the array is
        returned.
        """
        filename = os.path.normpath(decl.location.file_name)
        line = decl.location.line
        funcname = decl.name
        args = self.funcargs.get((filename, line, funcname))
        if args==None:
            return None
        if idx>=len(args):
            return None
        else:
            return args[idx]

