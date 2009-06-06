#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import sys
sys.path.append( '../..' )

from pyboost import crc
import random
import unittest

class consts:
    data = [ 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 ]
    crc_ccitt_result = 0x29B1
    crc_16_result = 0xBB3D
    crc_32_result = 0xCBF43926

class tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )
    
    def fundamental_test( self, cls, data, expected ):
        inst = cls()
        inst.process_bytes( data )
        #self.failUnless( inst.checksum() == expected )
        
    def compute_test( self, fast_crc, slow_crc, expected ):
        fast_crc.process_bytes( consts.data, len(consts.data) )
        slow_crc.process_bytes( consts.data, len(consts.data) )
    
    def test( self ):
        self.fundamental_test( crc.crc_ccitt_type, consts.data, consts.crc_ccitt_result )
        self.fundamental_test( crc.crc_16_type, consts.data, consts.crc_16_result )
        self.fundamental_test( crc.crc_32_type, consts.data, consts.crc_32_result )

    def test_crc_basic_1( self ):
        crc1 = crc.crc_basic_1( 1 )
        crc1.process_bytes( consts.data )
        self.failUnless( crc1.checksum() == 1 )
        
    def test_crc_basic_3( self ):
        #The CRC standard is a SDH/SONET Low Order LCAS control word with CRC-3
        #taken from ITU-T G.707 (12/03) XIII.2.

        #Four samples, each four bytes should all have a CRC of zero
        samples = \
        [
            [ 0x3A, 0xC4, 0x08, 0x06 ],
            [ 0x42, 0xC5, 0x0A, 0x41 ],
            [ 0x4A, 0xC5, 0x08, 0x22 ],
            [ 0x52, 0xC4, 0x08, 0x05 ]
        ]

        # Basic computer
        tester1 = crc.basic[3]( 0x03 ) #same as crc.crc_basic_3
    
        tester1.process_bytes( samples[0] )
        self.failUnless( tester1.checksum() == 0 )
    
        tester1.reset()
        tester1.process_bytes( samples[1] )
        self.failUnless( tester1.checksum() == 0 )
    
        tester1.reset()
        tester1.process_bytes( samples[2] )
        self.failUnless( tester1.checksum() == 0 )
    
        tester1.reset()
        tester1.process_bytes( samples[3] )
        self.failUnless( tester1.checksum() == 0 )
    
        # Optimal computer
        #define PRIVATE_CRC_FUNC   boost::crc<3, 0x03, 0, 0, false, false>
        #define PRIVATE_ACRC_FUNC  boost::augmented_crc<3, 0x03>
    
        #self.failUnless( 0 == PRIVATE_CRC_FUNC(samples[0], 4) )
        #self.failUnless( 0 == PRIVATE_CRC_FUNC(samples[1], 4) )
        #self.failUnless( 0 == PRIVATE_CRC_FUNC(samples[2], 4) )
        #self.failUnless( 0 == PRIVATE_CRC_FUNC(samples[3], 4) )
    
        # maybe the fix to CRC functions needs to be applied to augmented CRCs?
    
        #undef PRIVATE_ACRC_FUNC
        #undef PRIVATE_CRC_FUNC
        
    def test_crc_basic_7( self ):
        #"Doing short-CRC (7-bit augmented) message tests."
    
        # The CRC standard is a SDH/SONET J0/J1/J2/N1/N2/TR TTI (trace message)
        # with CRC-7, o.a. ITU-T G.707 Annex B, G.832 Annex A.
    
        # Two samples, each sixteen bytes
        # Sample 1 is '\x80' + ASCII("123456789ABCDEF")
        # Sample 2 is '\x80' + ASCII("TTI UNAVAILABLE")
        samples = [ [ 0x80, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
                      0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 ]
                  , [ 0x80, 0x54, 0x54, 0x49, 0x20, 0x55, 0x4E, 0x41, 
                      0x56, 0x41, 0x49, 0x4C, 0x41, 0x42, 0x4C, 0x45 ] ]
        results = [ 0x62, 0x23 ]
    
        # Basic computer
        tester1 = crc.crc_basic_7( 0x09 )
    
        tester1.process_bytes( samples[0] )
        self.failUnless( tester1.checksum() == results[0] )
    
        tester1.reset()
        tester1.process_bytes( samples[1] )
        self.failUnless( tester1.checksum() == results[1] )
    
        # Optimal computer
        #define PRIVATE_CRC_FUNC   boost::crc<7, 0x09, 0, 0, false, false>
        #define PRIVATE_ACRC_FUNC  boost::augmented_crc<7, 0x09>
    
        #self.failUnless( results[0] == PRIVATE_CRC_FUNC(samples[0], 16) )
        #self.failUnless( results[1] == PRIVATE_CRC_FUNC(samples[1], 16) )
    
        # maybe the fix to CRC functions needs to be applied to augmented CRCs?
    
        #undef PRIVATE_ACRC_FUNC
        #undef PRIVATE_CRC_FUNC
    
    def compute_test( self, optimal_cls, basic_cls, data, expected ):   
        fast_crc = optimal_cls()
        slow_crc = basic_cls( fast_crc.truncated_polynominal
                              , fast_crc.initial_remainder
                              , fast_crc.final_xor_value
                              , fast_crc.reflect_input
                              , fast_crc.reflect_remainder )

        fast_crc.process_bytes( data )
        slow_crc.process_bytes( data )
        self.failUnless( fast_crc.checksum() == expected )
        self.failUnless( slow_crc.checksum() == expected )
        #self.failUnless( func_result == expected );

    def interrupt_test( self, optimal_cls, basic_cls, data, expected ):   
        fast_crc1 = optimal_cls()
        slow_crc1 = basic_cls( fast_crc1.truncated_polynominal
                               , fast_crc1.initial_remainder
                               , fast_crc1.final_xor_value
                               , fast_crc1.reflect_input
                               , fast_crc1.reflect_remainder )
        self.failUnless( fast_crc1.get_interim_remainder() == slow_crc1.get_initial_remainder() )
        first_half = data[:len( data )/2]
        second_half = data[len( data )/2:]
        
        fast_crc1.process_bytes( first_half );
        slow_crc1.process_bytes( first_half );
        self.failUnless( fast_crc1.checksum() == slow_crc1.checksum() )
        
        #Process the second half of the data (also test accessors)
        fast_crc2 = optimal_cls( fast_crc1.get_interim_remainder() )
        slow_crc2 = basic_cls( slow_crc1.get_truncated_polynominal()
                               , slow_crc1.get_interim_remainder()
                               , slow_crc1.get_final_xor_value()
                               , slow_crc1.get_reflect_input()
                               , slow_crc1.get_reflect_remainder() )        
        
        fast_crc2.process_block( second_half )
        slow_crc2.process_block( second_half )
        
        self.failUnless( fast_crc2.checksum() == slow_crc2.checksum() )
        self.failUnless( fast_crc2.checksum() == expected )
        self.failUnless( slow_crc2.checksum() == expected )

    def error_test( self, optimal_cls, basic_cls ):   
        # Create a random block of data
        ran_data = map( lambda unused: random.randint(0, 255), [0] * 256 )

        # Create computers and compute the checksum of the data
        fast_tester = optimal_cls()
        slow_tester = basic_cls( fast_tester.truncated_polynominal
                                 , fast_tester.initial_remainder
                                 , fast_tester.final_xor_value
                                 , fast_tester.reflect_input
                                 , fast_tester.reflect_remainder )

        fast_tester.process_bytes( ran_data )
        slow_tester.process_bytes( ran_data )
    
        fast_checksum = fast_tester.checksum()
        slow_checksum = slow_tester.checksum()
    
        self.failUnless( fast_checksum == slow_checksum )
    
        # Do the checksum again (and test resetting ability)
        fast_tester.reset();
        slow_tester.reset( fast_tester.initial_remainder );
        fast_tester.process_bytes( ran_data )
        slow_tester.process_bytes( ran_data )
        self.failUnless( fast_tester.checksum() == slow_tester.checksum() )
        self.failUnless( fast_tester.checksum() == fast_checksum )
        self.failUnless( slow_tester.checksum() == slow_checksum )
    
        # Produce a single-bit error
        ran_data[ ran_data[0] % 255 ] = ( ran_data[ ran_data[0] % 255 ] ^ ( 1 << (ran_data[1] % 32) ) ) % 255
    
        # Compute the checksum of the errorenous data
        # (and continue testing resetting ability)
        fast_tester.reset( fast_tester.initial_remainder )
        slow_tester.reset()
        fast_tester.process_bytes( ran_data )
        slow_tester.process_bytes( ran_data )
        self.failUnless( fast_tester.checksum() == slow_tester.checksum() )
        self.failUnless( fast_tester.checksum() != fast_checksum )
        self.failUnless( slow_tester.checksum() != slow_checksum )

    def test_crc_ccitt( self ):
        self.compute_test( crc.crc_ccitt_type, crc.crc_basic_16, consts.data, consts.crc_ccitt_result )
        self.interrupt_test( crc.crc_ccitt_type, crc.crc_basic_16, consts.data, consts.crc_ccitt_result )
        self.error_test( crc.crc_ccitt_type, crc.crc_basic_16 )
        
    def test_crc_16( self ):
        self.compute_test( crc.crc_16_type, crc.crc_basic_16, consts.data, consts.crc_16_result)
        self.interrupt_test( crc.crc_16_type, crc.crc_basic_16, consts.data, consts.crc_16_result)

    def test_crc_32( self ):
        self.compute_test( crc.crc_32_type, crc.crc_basic_32, consts.data, consts.crc_32_result)
        self.interrupt_test( crc.crc_32_type, crc.crc_basic_32, consts.data, consts.crc_32_result)

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t) )
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()