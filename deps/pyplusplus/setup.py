# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys, os, os.path
from distutils import sysconfig
from distutils.core import setup
from distutils.cmd import Command

try:
    sys.path.append("../pygccxml_dev")
    import pygccxml
    pygccxml_available = True
except ImportError:
    pygccxml_available = False


def add_pygccxml_to_PYTHONPATH():
    """Update PYTHONPATH so that is refers to pygccxml_dev.

    The updated path is required for generating the documentation when
    pygccxml is not 'officially' installed.
    """
    if not os.environ.has_key( 'PYTHONPATH' ):
        os.environ['PYTHONPATH'] = ''
    if sys.platform == 'win32':
        environment_var_delimiter = ';'
    else:
        environment_var_delimiter = ':'

    pygccxml_path = os.path.normpath(os.path.join(os.getcwd(), "..", "pygccxml_dev"))
    os.environ[ 'PYTHONPATH' ] = os.environ[ 'PYTHONPATH' ] \
                                 + environment_var_delimiter \
                                 + pygccxml_path
    print "Setting PYTHONPATH to", os.environ["PYTHONPATH"]


def generate_doc():
    """Generate the epydoc reference manual.
    """
    if not pygccxml_available:
        print "Please install pygccxml before generating the docs."
        sys.exit()

    add_pygccxml_to_PYTHONPATH()

    import epydoc
    from epydoc.docbuilder import build_doc_index
    from epydoc.docwriter.html import HTMLWriter

    print "Generating epydoc files..."

    # Register a logger object so that warnings/errors are shown
    epydoc.log.register_logger(epydoc.log.SimpleLogger())

    docindex = build_doc_index(['pyplusplus', 'pygccxml'])
    html_writer = HTMLWriter( docindex
                              , prj_name='Py++'
                              , prj_url='http://www.language-binding.net'
                              , show_private=False
                              , show_frames=False  )

    html_writer.write( os.path.join('docs', 'documentation', 'apidocs') )


class doc_cmd(Command):
    """This is a new distutils command 'doc' to build the epydoc manual.
    """

    description = 'build the API reference using epydoc'
    user_options = [('no-doc', None, "don't run epydoc")]
    boolean_options = ['no-doc']

    def initialize_options (self):
        self.no_doc = 0

    def finalize_options (self):
        pass

    def run(self):
        if self.no_doc:
            return
        generate_doc()


# Generate the doc when a source distribution is created
if sys.argv[-1]=="sdist":
    generate_doc()


setup( name = "Py++",
       version = "0.8.5",
       description="Py++ is a framework of components for creating C++ code generator for Boost.Python library",
       author="Roman Yakovenko",
       author_email="roman.yakovenko@gmail.com",
       url='http://www.language-binding.net/pyplusplus/pyplusplus.html',
       scripts = ["scripts/pyplusplus_gui",
                  "scripts/pyplusplus_gui.pyw"],
       packages=[ 'pyplusplus',
                  'pyplusplus.gui',
                  'pyplusplus.file_writers',
                  'pyplusplus.code_creators',
                  'pyplusplus.module_creator',
                  'pyplusplus.code_repository',
                  'pyplusplus.decl_wrappers',
                  'pyplusplus.module_builder',
                  'pyplusplus.utils',
                  'pyplusplus.function_transformers',
                  'pyplusplus._logging_',
                  'pyplusplus.messages'],
       cmdclass = {"doc" : doc_cmd}
)
