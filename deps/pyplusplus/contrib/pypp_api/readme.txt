pypp_api
========

pypp_api is an optional high-level API built on top of Py++. It can be
used to control the binding creation process without that you have to
know every detail about the internals of pygccxml and Py++.

Usage
-----

In order to use the package set PYTHONPATH to contrib/pypp_api so
that you can import pypp_api. 
In your script you will usually import the API as follows:

from pypp_api import *


Documentation
-------------

A quick start tutorial and a user guide are available on the Py++ wiki at:

  https://realityforge.vrsource.org/view/PyppApi/

(see the pages PyppTutorial and PyppUserGuide)

You can also invoke the script generate_docs.py to generate a reference
manual using epydoc (http://epydoc.sourceforge.net). If you are new to 
pypp_api I recommend to have a look at the wiki first.

