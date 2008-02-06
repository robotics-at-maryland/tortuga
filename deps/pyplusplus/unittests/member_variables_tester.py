# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import unittest
import fundamental_tester_base

class tester_t(fundamental_tester_base.fundamental_tester_base_t):
    EXTENSION_NAME = 'member_variables'
    
    def __init__( self, *args ):
        fundamental_tester_base.fundamental_tester_base_t.__init__( 
            self
            , tester_t.EXTENSION_NAME
            , *args )
                                                                    
    def customize(self, mb ):        
        mb.variable( 'prefered_color' ).alias = 'PreferedColor'
        mb.classes().always_expose_using_scope = True

    def change_default_color( self, module ):
        module.point.default_color = module.point.color.blue
    
    def change_prefered_color( self, module ):
        xypoint = module.point()        
        xypoint.PreferedColor = module.point.color.blue
    
    def set_b( self, bf, value ):
        bf.b = value
        
    def run_tests(self, module):  
        self.failIfRaisesAny( module.point )
        xypoint = module.point()        
        self.failUnless( module.point.instance_count == 1)
        self.failUnless( xypoint.instance_count == 1)
        self.failUnless( module.point.default_color == module.point.color.red)
        self.failUnless( xypoint.default_color == module.point.color.red)
        self.failUnless( xypoint.x == -1)
        self.failUnless( xypoint.y == 2 )
        self.failUnless( xypoint.PreferedColor == xypoint.color.blue )
        self.failUnlessRaises( Exception, self.change_default_color )
        self.failUnlessRaises( Exception, self.change_prefered_color )

        bf = module.bit_fields_t()
        module.set_a( bf, 1 )
        self.failUnless( 1 == bf.a )
        self.failUnless( bf.b == module.get_b( bf ) )
        self.failIfNotRaisesAny( lambda: self.set_b( bf, 23 ) )
        
        array = module.array_t()
        self.failUnless( len( array.vars ) == 3 )
        for i in range( len( array.vars ) ):
            self.failUnless( array.vars[i].value == -9 )
        self.failUnless( len( array.ivars ) == 10 )
        
        ivars = array.ivars
        del array #testing call policies
        for i in range(20):
            for index in range(10):
                self.failUnless( ivars[index] == -index )
        
        array = module.array_t()
        for index in range( len(array.ivars) ):
            array.ivars[index] = index * index
            self.failUnless( array.get_ivars_item( index ) == index * index )
        
        tree = module.create_tree()
        self.failUnless( tree.parent is None )
        self.failUnless( tree.data.value == 0 )
        self.failUnless( tree.right is None )
        self.failUnless( tree.left )
        self.failUnless( tree.left.data.value == 1 )
        
        try:
            tree.right = module.create_tree()
            self.failUnless( 'Attribute error exception should be raised!' )
        except AttributeError:
            pass
        #self.failUnless( tree.right.parent is None )
        #self.failUnless( tree.right.data.value == 0 )
        #self.failUnless( tree.right.right is None )
        #self.failUnless( tree.right.left )
        #self.failUnless( tree.right.left.data.value == 1 )
        
        mem_var_str = module.mem_var_str_t()
        mem_var_str.identity( module.mem_var_str_t.class_name )
        
def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()