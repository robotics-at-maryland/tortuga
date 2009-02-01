Py++ - Boost.Python code generator
========================================

Py++ is a code generator for Boost.Python that simplifies writing
Python bindings of a C/C++ library. The tool is implemented as a Python
module which is controlled by a user script.

Homepage: http://www.language-binding.net/pyplusplus/pyplusplus.html


Requirements
------------

In order to use Py++ you need the following additional components:

- Python v2.4 (or higher)
- pygccxml (http://www.language-binding.net/pygccxml/pygccxml.html)
- GCC-XML (http://www.gccxml.org)


Install
-------

The package uses the Python distutils so you can do the usual procedure:

  python setup.py install

For more information about using the distutils see the Python manual
"Installing Python Modules".


Documentation
-------------

For examples and tutorials see the Py++ web site. An API reference
is available in the directory docs/apidocs in the source archive.

If you obtained the source code from the subversion repository you
have to build the API reference yourself. This can be done using the
setup script:

  python setup.py doc

In order for this to work you need epydoc (http://epydoc.sourceforge.net)
and pygccxml.


--------------------------------------------------------------------------
Copyright 2004-2008 Roman Yakovenko.
Distributed under the Boost Software License, Version 1.0. (See
accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
