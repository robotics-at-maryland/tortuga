# Original code: Copyright (c) 2005, 2006 Kevin Dangoor and contributors. 
#                TurboGears is a trademark of Kevin Dangoor.
#
#    Please see the license/TurboGearsLicense.txt file for more information.
# All Changes:
#  Copyright (C) 2007 Maryland Robotics Club
#  Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
#
# Author: Originally from Turbogears
#         Changes - Joseph Lisee <jlisee@umd.edu>
# File:  packages/python/ram/test/decorator.py


import unittest

import ram.decorator as decorator

def d1(func):
    def call(func, *args, **kw):
        return func(*args, **kw)
    return call
d1 = decorator.decorator(d1)

def d2(func):
    def call(func, *args, **kw):
        return func(*args, **kw)
    return call
d2 = decorator.decorator(d2)

def d3(func):
    func.baz2 = 2
    def call(func, *args, **kw):
        return func(*args, **kw)
    return call
d3 = decorator.decorator(d3)

def foo(a, b):
    return a+b
foo.baz1 = 1

bar = foo

foo = d1(foo)
foo = d2(foo)
foo = d3(foo)

def addn(n):
    def entangle(func):
        def call(func, *args, **kw):
            return func(*args, **kw) + n
        return call
    return decorator.decorator(entangle)

@addn(11)
@addn(11)
def py23(a):
    return a

@decorator.compose(addn(1), addn(2))
def composed(a):
    return a

def weakener():
    def call(func, *args, **kw):
        return len(args)
    return decorator.weak_signature_decorator(call)

@weakener()
def weakling():
    pass

def sig_changer():
    def call(func, a, b):
        return func(b)
    return decorator.decorator(call, (["a", "b"], None, None, None))

@sig_changer()
def new_sig(a):
    return a

def simple(func, *args, **kw):
    return func(*args, **kw) + 1
simple = decorator.decorator(simple)

def simple_weakener(func, *args, **kw):
     return len(args)
simple_weakener = decorator.weak_signature_decorator(simple_weakener)

def simple_weakling():
    pass
simple_weakling = simple_weakener(simple_weakling)

def simple_adder1(a):
    return a
simple_adder1 = simple(simple_adder1)

class TestDecorator(unittest.TestCase):

    def test_preservation(self):
        for key, value in bar.__dict__.iteritems():
            self.failUnless(key in foo.__dict__)
            self.failUnless(value == foo.__dict__[key])
        self.failUnless(bar.__name__ == foo.__name__)
        self.failUnless(bar.__module__ == foo.__module__)
        self.failUnless(bar.__doc__ == foo.__doc__)

    def test_eq(self):
        self.failUnless(decorator.func_id(foo) == decorator.func_id(bar))
        self.failUnless(decorator.func_eq(foo, bar))
        self.failUnless(bar == decorator.func_original(foo))
        self.failUnless(bar is decorator.func_original(foo))

    def test_history(self):
        self.failUnless(len(decorator.func_composition(foo)) == 4)
        self.failUnless(decorator.func_composition(foo)[:-1] ==
                        decorator.func_composition(decorator.func_composition(foo)[-2]))

    #def test_23compatibility(self):
    #    self.failUnless(py23(2)==24)

    def test_attributes(self):
        self.failUnless(foo.baz1==1)
        #self.failUnless(foo.baz2==2)

    def test_composition(self):
        #self.failUnless(addn(3)(py23)(2)==27)
        #self.failUnless(composed(1)==4, composed(1))
        self.failUnless(composed.__name__ == "composed")

    def test_signature(self):
        self.failUnless(decorator.make_weak_signature(bar)[1:3]==(
                        "_decorator__varargs", "_decorator__kwargs"))
        self.failUnless(weakling(1,2)==2)
        self.failUnless(new_sig(1,2)==2)

    def test_simple_decorators(self):
        self.failUnless(simple_adder1(1)==2)
        self.failUnless(simple_weakling(1,2)==2)

def get_suite():
    """
    Returns all TestSuites in this module
    """
    suites = []
    test_loader = unittest.TestLoader()
    
    suites.append(test_loader.loadTestsFromTestCase(TestDecorator))
    
    return unittest.TestSuite(suites)
    
if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(get_suite())
