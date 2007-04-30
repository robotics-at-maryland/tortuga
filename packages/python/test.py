# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  test.py

# Python Imports
import unittest
import re
import os
import os.path
import optparse

# Library Imports
HAVE_FIGLEAF = False
try:
    import figleaf
    import figleaf.htmlizer
    HAVE_FIGLEAF = True
except ImportError:
    print 'Figleaf not found, no coverage data available'

# Note This code is borrowed from Trac:
#  Copyright (C) 2003-2006 Edgewall Software
#  All rights reserved.
# See TracLicense.txt in the license folder.
def Mock(bases=(), *initargs, **kw):
    """
    Simple factory for dummy classes that can be used as replacement for the 
    real implementation in tests.
    
    Base classes for the mock can be specified using the first parameter, which
    must be either a tuple of class objects or a single class object. If the
    bases parameter is omitted, the base class of the mock will be object.

    So to create a mock that is derived from the builtin dict type, you can do:

    >>> mock = Mock(dict)
    >>> mock['foo'] = 'bar'
    >>> mock['foo']
    'bar'

    Attributes of the class are provided by any additional keyword parameters.

    >>> mock = Mock(foo='bar')
    >>> mock.foo
    'bar'

    Objects produces by this function have the special feature of not requiring
    the 'self' parameter on methods, because you should keep data at the scope
    of the test function. So you can just do:

    >>> mock = Mock(add=lambda x,y: x+y)
    >>> mock.add(1, 1)
    2

    To access attributes from the mock object from inside a lambda function,
    just access the mock itself:

    >>> mock = Mock(dict, do=lambda x: 'going to the %s' % mock[x])
    >>> mock['foo'] = 'bar'
    >>> mock.do('foo')
    'going to the bar'

    Because assignments or other types of statements don't work in lambda
    functions, assigning to a local variable from a mock function requires some
    extra work:

    >>> myvar = [None]
    >>> mock = Mock(set=lambda x: myvar.__setitem__(0, x))
    >>> mock.set(1)
    >>> myvar[0]
    1
    """
    if not isinstance(bases, tuple):
        bases = (bases,)
    cls = type('Mock', bases, {})
    mock = cls(*initargs)
    for k,v in kw.items():
        setattr(mock, k, v)
    return mock

def import_module_from_name(name):
    """
    Equivalent to import <name> as <return>.
    
    @type  name: string
    @param name: The module to be import, of the form 'root.submod.module'
    
    @rtype : module
    @return: The module at the end of the dotted module name
    """
    mod = __import__(name)
    components = name.split('.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod

def import_sub_modules(package_name):
    """
    Gets all modules contained within a package
    
    @type  name: string
    @param name: A package name of the form 'root.submod.module'
    
    @rtype : list
    @return: This is a list containing all sub modules of the given package
    """
    print 'Find sub:',package_name
    # Bail out if path is not a directory
    #if not os.path.isdir(package_name):
    #    return
    
    # Get a files in the packages directory
    package = import_module_from_name(package_name)
    package_dir = os.path.split(package.__file__)[0]
    files = os.listdir(package_dir)
    
    # This filtering might be cleaned with a regular expression
    # Accept only *.py files that aren't __init__ and strip the .py
    mod_files = [path[0:-3] for path in files if path.endswith('.py')]
    mod_files.remove('__init__')
 
    # Import all sub modules into a list
    modules = []
    for mod_file in mod_files:
        mod_name = '%s.%s' % (package_name, mod_file)
        print 'Importing module:',mod_name
        modules.append(import_module_from_name(mod_name))
        
    # Now recursively add sub modules excluding hidden directories
    sub_modules = [path for path in files if os.path.isdir(path)]
    sub_modules = [path for path in sub_modules if not path.startswith('.')]
    
    for sub_module in sub_modules:
        mod_name = '%s.%s' % (package_name, sub_module)
        print 'Locking sub modules of %s, in %s' % (package_name, mod_name)
        new_sub_modules = import_sub_modules(mod_name)
        if new_sub_modules is not None:
            modules.extend(new_sub_modules)
        
    return modules
    
def gather_suites(modules):
    """
    This gets the suites from all given modules with 'get_suites' and combines
    them into one large unittest.
    """
    suites = []
    
    for mod in modules:
        suites.append(mod.get_suite())
    
    return unittest.TestSuite(suites)

def gather_suites_from_package(package_name):
    """
    Gets all modules contained within a package
    
    @type  name: string
    @param name: A package name of the form 'root.submod.module'
    
    @rtype : unittest.TestSuite
    @return: A test suite that contains all tests of the package and its sub
             modules.
    """
    modules = import_sub_modules(package_name)
    return gather_suites(modules)

def generate_coverage_files(report_file, coverage_directory):
    """
    Go through all modules in the given package and generate the coverage data
    for them.
    
    @type  name: string
    @param name: A package name of the form 'root.submod.module'
    
    @type  coverage_directory: string
    @param coverage_directory: The directory into which the coverage html files
                               will be placed.
    """
    
    # Ensure coverage directory exists
    if not os.path.exists(coverage_directory):
        os.makedirs(coverage_directory)
    
    # Generate exclude patterns to remove files from display
    patterns = ['site-packages', 'Python25', r'python' + os.sep*2 + 'tests',
                'test.py']
    regexes = []
    for pattern in patterns:
        regexes.append(re.compile(pattern))
    
    coverage = figleaf.read_coverage(report_file)
    figleaf.htmlizer.report_as_html(coverage, coverage_directory, regexes,
                                    relative_paths = True, sort_by_path = True)

def test_with_coverage(test_func):
    # Run test function with coverage
    figleaf.start()
    test_func()
    figleaf.stop()
    
    # Create html coverage data
    figleaf.write_coverage('.figleaf')
    generate_coverage_files('.figleaf', 'coverage')

if __name__ == '__main__':
    option_parser = optparse.OptionParser()

    option_parser.add_option('-c', '--coverage', action="store_true",
                             dest="create_coverage",
                             help="Generage an html code coverage report")

    (options, args) = option_parser.parse_args()
    # Remove old coverage data and start collecting a new set
    
    # Run All Tests
    def run_tests():
        suite = gather_suites_from_package('tests')
        unittest.TextTestRunner(verbosity=2).run(suite)
        
    if options.create_coverage:
        if HAVE_FIGLEAF:
            test_with_coverage(run_tests)
        else:
            print 'Figleaf not found, no coverage data generated'
    else:
        run_tests()