import unittest
import properties

class tester_t( unittest.TestCase ):
    def __init__(self, *args ):
        unittest.TestCase.__init__( self, *args )
    
    def test(self):
        point = properties.point_t()
        self.failUnless( point.x == 0 )
        self.failUnless( point.y == 0 )
        point.x = 24
        point.y = 18
        self.failUnless( properties.extract_x( point ) == 24 )
        self.failUnless( properties.extract_y( point ) == 18 )

def create_suite():
    suite = unittest.TestSuite()        
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()        