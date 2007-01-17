import unittest
import auto_conversion as auto_conv
import from_conversion as from_conv

class auto_conversion_tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )

    def test_auto( self ):
        self.failUnless( (0,0,0) == auto_conv.record_ret_val_000() )
        self.failUnless( (1,0,1) == auto_conv.record_ret_val_101() )
        self.failUnless( (0,1,0) == auto_conv.record_ret_ref_010() )
        self.failUnlessRaises( TypeError, auto_conv.record_ret_ptr_110 )
        self.failUnless( auto_conv.test_record_val_000( (0,0,0) ) )
        self.failUnless( auto_conv.test_record_cref_010( (0,1,0) ) )
        self.failUnlessRaises( TypeError, auto_conv.test_record_ref_110, (1,1,0) )
        self.failUnlessRaises( TypeError, auto_conv.test_record_ptr_101, (1,0,1) )
    
    def test_from_sequence( self ):
        self.failUnless( from_conv.test_val_010( (0,1,0) ) )
        self.failUnless( from_conv.test_cref_000( [0,0,0] ) )
        self.failUnless( from_conv.test_ref_111( [1,1,1] ) )
        self.failUnlessRaises( Exception, from_conv.test_ptr_101, (1,0,1) )
        self.failUnlessRaises( Exception, from_conv.test_cptr_110, (1,1,0) )

    def test_from_class( self ):
        color = from_conv.colour_t
        self.failUnless( from_conv.test_val_010( color(0,1,0) ) )
        self.failUnless( from_conv.test_cref_000( color(0,0,0) ) )
        self.failUnless( from_conv.test_ref_111( color(1,1,1) ) )
        self.failUnless( from_conv.test_ptr_101( color(1,0,1) ) )
        self.failUnless( from_conv.test_cptr_110( color(1,1,0) ) )
    
    def cmp_colours( self, c1, c2 ):
        return c1.red == c2.red and c1.green == c2.green and c1.blue == c2.blue
        
    def test_from_class_property( self ):
        colour = from_conv.colour_t
        image = from_conv.image_t()
        self.failUnless( self.cmp_colours( image.background, colour() ) )
        image.background = (1,0,1)
        self.failUnless( self.cmp_colours( image.background, colour(1,0,1) ) )
        self.failUnless( image.test_background( [1,0,1] ) )
        self.failUnless( image.test_background( colour(1,0,1 ) ) )

def create_suite():
    suite = unittest.TestSuite()
    suite.addTest( unittest.makeSuite(auto_conversion_tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()
