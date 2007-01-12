import unittest
from auto_conversion import  *

class auto_conversion_tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )

    def test( self ):
        self.failUnless( (0,0,0) == point_ret_val_000() )
        self.failUnless( (1,0,1) == point_ret_val_101() )
        self.failUnless( (0,1,0) == point_ret_ref_010() )
        self.failUnlessRaises( TypeError, point_ret_ptr_110 )
        self.failUnless( test_point_val_000( (0,0,0) ) )
        self.failUnless( test_point_cref_010( (0,1,0) ) )
        self.failUnlessRaises( TypeError, test_point_ref_110, (1,1,0) )
        self.failUnlessRaises( TypeError, test_point_ptr_101, (1,0,1) )
        
def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(auto_conversion_tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
