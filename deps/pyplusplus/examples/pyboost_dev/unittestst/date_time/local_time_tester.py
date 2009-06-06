#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path.append( '../..' )

from pyboost import date_time
from pyboost.date_time import gregorian
from pyboost.date_time import posix_time
from pyboost.date_time import local_time
from pyboost.date_time import to_iso_string
from pyboost.date_time import to_simple_string
import unittest

class tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )

    def test_clocks( self ):
        az_tz = local_time.posix_time_zone("MST-07")
        ny_tz = local_time.posix_time_zone("EST-05EDT,M4.1.0,M10.5.0")
        print to_simple_string( posix_time.second_clock.local_time() )
        print local_time.local_sec_clock.local_time(az_tz).to_string()
        print local_time.local_sec_clock.local_time(ny_tz).to_string()

        print to_simple_string( posix_time.microsec_clock.local_time() )
        print local_time.local_microsec_clock.local_time(az_tz).to_string()
        print local_time.local_microsec_clock.local_time(ny_tz).to_string()

    def test_dst_transition_day_rule(self):
        rule1 = local_time.partial_date_dst_rule( 
                    gregorian.partial_date(30, date_time.Apr)
                    , gregorian.partial_date(30, date_time.Oct))
        self.failUnless( rule1.start_day(2001) == gregorian.date(2001, date_time.Apr, 30))
        self.failUnless( rule1.end_day(2001) == gregorian.date(2001, date_time.Oct, 30))
    
        rule2 = local_time.first_last_dst_rule(
                    local_time.first_last_dst_rule.start_rule(date_time.Sunday, date_time.Apr)
                    , local_time.first_last_dst_rule.end_rule(date_time.Sunday, date_time.Oct))

        self.failUnless( rule2.start_day(2001) == gregorian.date(2001, date_time.Apr, 1))
        self.failUnless( rule2.end_day(2001) == gregorian.date(2001, date_time.Oct, 28))

        rule3 = local_time.last_last_dst_rule(
                    local_time.last_last_dst_rule.start_rule(date_time.Sunday, date_time.Mar)
                    , local_time.last_last_dst_rule.end_rule(date_time.Sunday, date_time.Oct))

        self.failUnless( rule3.start_day(2001) == gregorian.date(2001, date_time.Mar, 25))
        self.failUnless( rule3.end_day(2001) == gregorian.date(2001, date_time.Oct, 28) )
  
        nkday = gregorian.nth_kday_of_month
        rule4 = local_time.nth_last_dst_rule(
                    local_time.nth_last_dst_rule.start_rule(nkday.first, date_time.Sunday, date_time.Mar)
                    , local_time.nth_last_dst_rule.end_rule(date_time.Sunday, date_time.Oct))
  
        self.failUnless( rule4.start_day(2001) == gregorian.date(2001, date_time.Mar, 4))
        self.failUnless( rule4.end_day(2001) == gregorian.date(2001, date_time.Oct, 28))

        rule5 = local_time.nth_kday_dst_rule(
                    local_time.nth_kday_dst_rule.start_rule(nkday.first, date_time.Sunday, date_time.Mar)
                    , local_time.nth_kday_dst_rule.end_rule(nkday.fourth, date_time.Sunday, date_time.Oct))
  
        self.failUnless( rule5.start_day(2001) == gregorian.date(2001, date_time.Mar, 4))
        self.failUnless( rule5.end_day(2001) == gregorian.date(2001, date_time.Oct, 28))
  
    def test_custom_time_zone( self ):
        rule1 = local_time.partial_date_dst_rule(
                    gregorian.partial_date(30, date_time.Apr)
                    , gregorian.partial_date(30, date_time.Oct))
  
        rule2 = local_time.first_last_dst_rule(
                    local_time.first_last_dst_rule.start_rule( date_time.Sunday, date_time.Apr)
                    , local_time.first_last_dst_rule.end_rule( date_time.Sunday, date_time.Oct))
        
        rule3 = local_time.last_last_dst_rule(
                    local_time.last_last_dst_rule.start_rule(date_time.Sunday, date_time.Mar)
                    , local_time.last_last_dst_rule.end_rule(date_time.Sunday, date_time.Oct))
        
        rule4 = None

        pst = local_time.time_zone_names( "Pacific Standard Time"
                                          , "PST"
                                          , "Pacific Daylight Time" 
                                          , "PDT" )
        
        mst = local_time.time_zone_names( "Mountain Standard Time"
                                          , "MST"
                                          , "" 
                                          , "" )
  
        of = local_time.dst_adjustment_offsets( posix_time.hours(1)
                                                , posix_time.hours(2)
                                                , posix_time.hours(2))
        of2 = local_time.dst_adjustment_offsets( posix_time.hours(0)
                                                 , posix_time.hours(0)
                                                 , posix_time.hours(0))

        tz1 = local_time.custom_time_zone(pst, posix_time.hours(-8), of, rule1)
        tz2 = local_time.custom_time_zone(pst, posix_time.hours(-8), of, rule2)
        tz3 = local_time.custom_time_zone(pst, posix_time.hours(-8), of, rule3)
        tz4 = local_time.custom_time_zone(mst, posix_time.hours(-7), of2, rule4)


        self.failUnless( tz1.dst_zone_abbrev() == "PDT" )
        self.failUnless( tz1.std_zone_abbrev() == "PST" )
        self.failUnless( tz1.std_zone_name() == "Pacific Standard Time" )
        self.failUnless( tz1.dst_zone_name() == "Pacific Daylight Time" )
        
        self.failUnless( tz1.dst_offset() == posix_time.hours(1) )
        self.failUnless( tz1.base_utc_offset() == posix_time.hours(-8) )
        self.failUnless( tz1.has_dst() )
        
        self.failUnless( tz1.dst_local_start_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Apr,30)
                                              , posix_time.hours(2) ) )
        
        self.failUnless( tz1.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Oct,30)
                                              , posix_time.hours(2) ) )
        
        
        self.failUnless( tz1.to_posix_string() == "PST-08PDT+01,120/02:00,303/02:00")
        self.failUnless( tz2.to_posix_string() == "PST-08PDT+01,M4.1.0/02:00,M10.5.0/02:00")
        self.failUnless( tz3.to_posix_string() == "PST-08PDT+01,M3.5.0/02:00,M10.5.0/02:00")
        self.failUnless( tz4.to_posix_string() == "MST-07")
        
        self.failUnless( not tz4.has_dst() )
        
        self.failUnless( tz4.dst_local_start_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time ) )
        
        self.failUnless( tz4.dst_local_end_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time ) )
    
    def test_local_time_period( self ):
        d1 = gregorian.date(2001, date_time.Jan, 1)        
        az_tz = local_time.posix_time_zone("MST-07")
        
        t1 = local_time.local_date_time( d1, posix_time.hours(2), az_tz, False)
        t2  = local_time.local_date_time( d1, posix_time.hours(3), az_tz, False)
        p1 = local_time.local_time_period( t1, t2 ) 
        p2 = local_time.local_time_period( t1, t2 )
        self.failUnless( p1 == p2 )

        p4 = local_time.local_time_period( t1, posix_time.hours(1) )
        self.failUnless( p4.length() == posix_time.hours(1) )        
        self.failUnless( p1 == p4 )
        self.failUnless( p1.begin() == t1 )
        self.failUnless( p1.end() == t2 )
        self.failUnless( p1.last() == t2 - posix_time.time_duration.unit() )
        self.failUnless( p1.contains( t1 ) )
        self.failUnless( not p1.contains( t2 ) )
        self.failUnless( p1.contains( t2 - posix_time.seconds(1) ) )
        
        t3 = local_time.local_date_time( gregorian.date(2001, date_time.Jan,1)
                                         , posix_time.hours(4)
                                         , az_tz
                                         , False )
        p5 = local_time.local_time_period( t2, t3 )
        self.failUnless( not p1.contains( p5.begin() ) )
        self.failUnless( not p5.contains( p1.begin() ) )
        self.failUnless( not( p1 == p5 ) )
        self.failUnless( p1 < p5 )
        self.failUnless( p5 > p1 )
        self.failUnless( p5 != p1 )
        self.failUnless( p1.intersects(p1) )
        self.failUnless( not p1.intersects(p5) )
        self.failUnless( not p5.intersects(p1) )
        
        p6 = local_time.local_time_period( t2, t3 )
        p6.shift( posix_time.minutes(30) )
        self.failUnless( p5.intersects(p6) )
        self.failUnless( p6.intersects(p5) )
        self.failUnless( p5.contains( p6.begin() ) )
        p6.shift( posix_time.minutes(30) )
        self.failUnless( not p5.intersects(p6) )
        self.failUnless( not p6.intersects(p5) )
        p6.shift( posix_time.minutes(-30) )
        
        p7 = p5.intersection(p6)
        
        self.failUnless( p7 == local_time.local_time_period(
                                   local_time.local_date_time( d1
                                                               , posix_time.time_duration(3,30,0)
                                                               , az_tz, False)
                                   , local_time.local_date_time( d1
                                                                 , posix_time.time_duration(4,0,0)
                                                                 , az_tz
                                                                 , False) ) )
     
    def test_time_zone(self):
        rule1 = local_time.partial_date_dst_rule(
                    gregorian.partial_date(30, date_time.Apr)
                    , gregorian.partial_date(30, date_time.Oct) )
        
        rule2 = local_time.first_last_dst_rule(
                    local_time.first_last_dst_rule.start_rule( date_time.Sunday, date_time.Apr)
                    , local_time.first_last_dst_rule.end_rule( date_time.Sunday, date_time.Oct))
        
        rule3 = local_time.last_last_dst_rule( 
                    local_time.last_last_dst_rule.start_rule( date_time.Sunday, date_time.Mar)
                    , local_time.last_last_dst_rule.end_rule( date_time.Sunday, date_time.Oct))
        rule4 = None
        
        pst = local_time.time_zone_names( "Pacific Standard Time"
                                          , "PST"
                                          , "Pacific Daylight Time" 
                                          , "PDT")
        mst = local_time.time_zone_names("Mountain Standard Time", "MST", "" , "")
        
        of = local_time.dst_adjustment_offsets(
                 posix_time.hours(1)
                 , posix_time.hours(2)
                 , posix_time.hours(2) )
        
        of2 = local_time.dst_adjustment_offsets(
                  posix_time.hours(0)
                  , posix_time.hours(0)
                  , posix_time.hours(0) )
        
        tz1 = local_time.time_zone(pst, posix_time.hours(-8), of, rule1)
        tz2 = local_time.time_zone(pst, posix_time.hours(-8), of, rule2)
        tz3 = local_time.time_zone(pst, posix_time.hours(-8), of, rule3)
        tz4 = local_time.time_zone(mst, posix_time.hours(-7), of2, rule4)
        
        self.failUnless( tz1.dst_zone_abbrev() == "PDT")
        self.failUnless( tz1.std_zone_abbrev() == "PST")
        self.failUnless( tz1.std_zone_name() == "Pacific Standard Time")
        self.failUnless( tz1.dst_zone_name() == "Pacific Daylight Time")
        
        self.failUnless( tz1.dst_offset() == posix_time.hours(1) )
        self.failUnless( tz1.base_utc_offset() == posix_time.hours(-8) )
        self.failUnless( tz1.has_dst() )
        
        self.failUnless( tz1.dst_local_start_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Apr,30)
                                              , posix_time.hours(2) ) )
        
        self.failUnless( tz1.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Oct,30)
                                              ,posix_time.hours(2) ) )
        
        self.failUnless( tz1.to_posix_string() == "PST-08PDT+01,120/02:00,303/02:00" )
        self.failUnless( tz2.to_posix_string() == "PST-08PDT+01,M4.1.0/02:00,M10.5.0/02:00")
        self.failUnless( tz3.to_posix_string() == "PST-08PDT+01,M3.5.0/02:00,M10.5.0/02:00")
        self.failUnless( tz4.to_posix_string() == "MST-07")
        
        self.failUnless( not tz4.has_dst() )
        self.failUnless( tz4.dst_local_start_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time ) )
        self.failUnless( tz4.dst_local_end_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time ) )

    def test_tz_database(self):
        tz_db = local_time.time_zone_database()
        tz_db.load_from_file( os.path.join( os.path.split( date_time.__file__ )[0]
                                            , 'date_time_zonespec.csv' ) )
                
        
        self.failUnless( not tz_db.time_zone_from_region("Invalid/name") ) 
        
        nyc_test = tz_db.time_zone_from_region("America/New_York")
        self.failUnless( nyc_test ) 
        self.failUnless( nyc_test.std_zone_abbrev() == "EST" )
        self.failUnless( nyc_test.std_zone_name() == "Eastern Standard Time" )
        self.failUnless( nyc_test.dst_zone_abbrev() == "EDT" ) 
        self.failUnless( nyc_test.dst_zone_name() == "Eastern Daylight Time" )
        self.failUnless( nyc_test.base_utc_offset() == posix_time.hours(-5) )
        self.failUnless( nyc_test.dst_offset() == posix_time.hours(1) )
        self.failUnless( nyc_test.dst_local_start_time(2004) 
                         == posix_time.ptime( gregorian.date(2004, date_time.Apr, 4)
                                              , posix_time.hours(2) ) )
        self.failUnless( nyc_test.dst_local_end_time(2004) 
                         == posix_time.ptime( gregorian.date(2004, date_time.Oct, 31)
                                              , posix_time.hours(2) ) )
        self.failUnless( nyc_test.has_dst() )
        
        phx_test = tz_db.time_zone_from_region("America/Phoenix")
        self.failUnless( phx_test ) 
        self.failUnless( phx_test.std_zone_abbrev() == "MST" )
        self.failUnless( phx_test.std_zone_name() == "Mountain Standard Time" )
        self.failUnless( phx_test.dst_zone_abbrev() == "" )
        self.failUnless( phx_test.dst_zone_name() == "" )  
        self.failUnless( phx_test.base_utc_offset() == posix_time.hours(-7) )
        self.failUnless( phx_test.dst_offset() == posix_time.hours(0) )
        self.failUnless( phx_test.has_dst() == False)
        
        
        eastern = local_time.posix_time_zone("EST-05:00:00EDT+01:00:00,M4.1.0/02:00:00,M10.5.0/02:00:00")
        tz_db.add_record("United States/Eastern", eastern)
        eastern_test = tz_db.time_zone_from_region("United States/Eastern" )
        self.failUnless( eastern_test )
        self.failUnless( eastern_test.std_zone_abbrev() == "EST" )
        self.failUnless( eastern_test.std_zone_name() == "EST" )
        self.failUnless( eastern_test.dst_zone_abbrev() == "EDT" )
        self.failUnless( eastern_test.dst_zone_name() == "EDT" )  
        self.failUnless( eastern_test.base_utc_offset() == posix_time.hours(-5) )
        self.failUnless( eastern_test.dst_local_start_time(2004) 
                         == posix_time.ptime( gregorian.date(2004, date_time.Apr, 4)
                                              , posix_time.hours(2) ) )
        self.failUnless( eastern_test.dst_local_end_time(2004) 
                         == posix_time.ptime( gregorian.date(2004, date_time.Oct, 31)
                                              , posix_time.hours(2) ) )
        self.failUnless( eastern_test.has_dst() )

    def test_posix_time_zone(self):
        specs = [ "MST-07"
                  , "MST-07:00:00"
                  , "EST-05EDT,M4.1.0,M10.5.0"
                  , "EST-05:00:00EDT+01:00:00,M4.1.0/02:00:00,M10.5.0/02:00:00"
                  , "PST-08PDT,J46/1:30,J310"
                  , "PST-08PDT,45,310/0:30:00" ]
        
        nyc1 = local_time.posix_time_zone( specs[2] )
        nyc2 = local_time.posix_time_zone( specs[3] )
        
        td = posix_time.hours(-5)        
        self.failUnless( nyc1.has_dst() and nyc2.has_dst() )
        self.failUnless( nyc1.base_utc_offset() == td )
        self.failUnless( nyc1.base_utc_offset() == nyc2.base_utc_offset() )
        self.failUnless( nyc1.std_zone_abbrev() == "EST" )
        self.failUnless( nyc2.std_zone_abbrev() == "EST" )
        self.failUnless( nyc1.dst_zone_abbrev() == "EDT" )
        self.failUnless( nyc2.dst_zone_abbrev() == "EDT" )
        self.failUnless( nyc1.std_zone_name() == "EST" ) 
        self.failUnless( nyc2.std_zone_name() == "EST" )
        self.failUnless( nyc1.dst_zone_name() == "EDT" )
        self.failUnless( nyc2.dst_zone_name() == "EDT" )
        
        td = posix_time.hours(1)
        self.failUnless( nyc1.dst_offset() == td )
        self.failUnless( nyc1.dst_offset() == nyc2.dst_offset() )
        self.failUnless( nyc1.dst_local_start_time(2003) 
                         ==  posix_time.ptime( gregorian.date(2003, date_time.Apr,6)
                                               , posix_time.time_duration(2,0,0) ) )
        self.failUnless( nyc1.dst_local_start_time(2003) == nyc2.dst_local_start_time(2003) ) 
        self.failUnless( nyc1.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Oct, 26)
                                              , posix_time.time_duration(2,0,0)))
        self.failUnless( nyc1.dst_local_end_time(2003) == nyc2.dst_local_end_time(2003) ) 
        self.failUnless( nyc1.to_posix_string() == "EST-05EDT+01,M4.1.0/02:00,M10.5.0/02:00" )
        self.failUnless( nyc2.to_posix_string() == "EST-05EDT+01,M4.1.0/02:00,M10.5.0/02:00")
        
        az1 = local_time.posix_time_zone( specs[0] )
        az2 = local_time.posix_time_zone( specs[1] )
        td = posix_time.hours(-7)
        
        self.failUnless( not az1.has_dst() and not az2.has_dst() )
        self.failUnless( az1.base_utc_offset() == td )
        self.failUnless( az1.base_utc_offset() == az2.base_utc_offset() )
        self.failUnless( az1.dst_local_start_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time) )
        self.failUnless( az2.dst_local_end_time(2005) 
                         == posix_time.ptime( date_time.not_a_date_time ) )
        self.failUnless( az1.std_zone_abbrev() == "MST" )
        self.failUnless( az2.std_zone_abbrev() == "MST" )
        self.failUnless( az1.dst_zone_abbrev() == "" )
        self.failUnless( az2.dst_zone_abbrev() == "" )
        self.failUnless( az1.std_zone_name() == "MST" )
        self.failUnless( az2.std_zone_name() == "MST" )
        self.failUnless( az1.dst_zone_name() == "" )
        self.failUnless( az2.dst_zone_name() == "" )
        self.failUnless( az1.to_posix_string() == "MST-07")
        self.failUnless( az2.to_posix_string() == "MST-07")
        
        bz = local_time.posix_time_zone("BST+11:21:15BDT-00:28,M2.2.4/03:15:42,M11.5.2/01:08:53")
        self.failUnless( bz.has_dst() )
        self.failUnless( bz.base_utc_offset() == posix_time.time_duration(11,21,15) )
        self.failUnless( bz.std_zone_abbrev() == "BST" )
        self.failUnless( bz.dst_zone_abbrev() == "BDT" )
        self.failUnless( bz.dst_offset() == posix_time.time_duration(0,-28,0) )
        self.failUnless( bz.dst_local_start_time(1962) 
                         == posix_time.ptime( gregorian.date(1962, date_time.Feb,8)
                                              , posix_time.time_duration(3,15,42) ) )
        self.failUnless( bz.dst_local_end_time(1962) 
                         == posix_time.ptime( gregorian.date( 1962, date_time.Nov,27)
                                              , posix_time.time_duration(1,8,53) ) )
        
        la1 = local_time.posix_time_zone( specs[4] )         
        self.failUnless( la1.dst_local_start_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Feb,15)
                                              , posix_time.time_duration(1,30,0) ) )
        self.failUnless( la1.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Nov,6)
                                              , posix_time.time_duration(2,0,0) ) )
        self.failUnless( la1.to_posix_string() == "PST-08PDT+01,45/01:30,310/02:00")
        
        la2 = local_time.posix_time_zone( specs[5] ) 
        self.failUnless( la2.dst_local_start_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Feb,15)
                                              ,posix_time.time_duration(2,0,0) ) )
        self.failUnless( la2.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003, date_time.Nov,6)
                                              , posix_time.time_duration(0,30,0) ) )
        self.failUnless( la2.to_posix_string() == "PST-08PDT+01,45/02:00,310/00:30" )
        
        self.failUnlessRaises( Exception, local_time.posix_time_zone, "EST-13" )
        self.failUnlessRaises( Exception, local_time.posix_time_zone, "EST-5EDT24:00:01,J124/1:30,J310" )
        self.failUnlessRaises( Exception, local_time.posix_time_zone, "EST-5EDT01:00:00,J124/-1:30,J310" )
        self.failUnlessRaises( Exception, local_time.posix_time_zone, "EST-5EDT01:00:00,J124/1:30,J370" )
             
        cet_tz = local_time.posix_time_zone("CET+01:00:00EDT+01:00:00,M4.1.0/02:00:00,M10.5.0/02:00:00")
        self.failUnless( cet_tz.has_dst() )
        self.failUnless( cet_tz.base_utc_offset() == posix_time.hours(1) )
        self.failUnless( cet_tz.std_zone_abbrev() == "CET") 
        
        caus_tz = local_time.posix_time_zone("CAS+08:30:00CDT+01:00:00,M4.1.0/02:00:00,M10.5.0/02:00:00")
        self.failUnless( caus_tz.has_dst() )
        self.failUnless( caus_tz.base_utc_offset() == posix_time.hours(8)+posix_time.minutes(30) )
        self.failUnless( caus_tz.std_zone_abbrev() == "CAS")
        
        spec = "FST+3FDT,J60,J304"
        fl_1 = local_time.posix_time_zone(spec)
        self.failUnless( fl_1.dst_local_start_time(2003) 
                         == posix_time.ptime( gregorian.date(2003,date_time.Mar,1)
                                              ,posix_time.hours(2) ) )
        self.failUnless( fl_1.dst_local_end_time(2003) 
                         == posix_time.ptime( gregorian.date(2003,date_time.Oct,31)
                                              , posix_time.hours(2) ) )
        self.failUnless( fl_1.dst_local_start_time(2004) 
                         == posix_time.ptime( gregorian.date(2004, date_time.Mar, 1)
                                              , posix_time.hours(2) ) )
        self.failUnless( fl_1.dst_local_end_time(2004) 
                         == posix_time.ptime( gregorian.date(2004,date_time.Oct,31)
                                              , posix_time.hours(2) ) )
        
    def test_local_time( self ):
        az_tz = local_time.posix_time_zone("MST-07")
        ny_tz = local_time.posix_time_zone("EST-05EDT,M4.1.0,M10.5.0")
        sydney = local_time.posix_time_zone("EST+10EST,M10.5.0,M3.5.0/03:00")
        null_tz = None
        d = gregorian.date(2003, 12, 20)
        h = posix_time.hours(12)
        t = posix_time.ptime(d,h)
        az_time = local_time.local_date_time(t, az_tz ) 
        
        self.failUnless( az_time.zone().std_zone_abbrev() == "MST") 
        self.failUnless( az_time.zone().base_utc_offset() == posix_time.hours(-7) )
        self.failUnless( az_time.zone().has_dst() == False )
        self.failUnless( az_time.is_dst() == False )
        self.failUnless( az_time.to_string() == "2003-Dec-20 05:00:00 MST" )
        
        az_time2 = local_time.local_date_time(t, az_tz ) 
        
        self.failUnless( az_time2.is_dst() == False )
        self.failUnless( az_time2.to_string() == "2003-Dec-20 05:00:00 MST" )
        self.failUnless( az_time2.zone().has_dst() == False )
        self.failUnless( az_time2.zone().base_utc_offset() == posix_time.hours(-7) )
                
        svpt = posix_time.ptime(date_time.not_a_date_time)
        sv_time = local_time.local_date_time(svpt, ny_tz)
        self.failUnless( sv_time.is_not_a_date_time() )
        self.failUnless( sv_time.to_string() == "not-a-date-time" )
        self.failUnless( sv_time.is_dst() == False )
        sv_time2 = local_time.local_date_time( date_time.pos_infin, None )
        self.failUnless( sv_time2.is_pos_infinity() )
        self.failUnless( sv_time2.to_string() == "+infinity" )
        self.failUnless( sv_time2.is_dst() == False )
        sv_time2 += gregorian.days(12) 
        self.failUnless( sv_time2.is_pos_infinity() )

        sv_time3 = local_time.local_date_time( date_time.max_date_time, ny_tz )
        self.failUnless( sv_time3.to_string() == "9999-Dec-31 18:59:59.999999999 EST" 
                         or sv_time3.to_string() == "9999-Dec-31 18:59:59.999999 EST" )
        
        sv_time4 = local_time.local_date_time(date_time.min_date_time, None)
        self.failUnless( sv_time4.to_string() == "1400-Jan-01 00:00:00 UTC" )
        
        d = gregorian.date(2004, date_time.Apr, 4)
        td = posix_time.time_duration(2,30,0) 
        calcop = local_time.local_date_time(d, td, ny_tz, local_time.local_date_time.NOT_DATE_TIME_ON_ERROR)
        self.failUnless( calcop.is_not_a_date_time() )

        d = gregorian.date(2004, date_time.Apr, 4)
        td = posix_time.time_duration(2,30,0) 
        self.failUnlessRaises( Exception, local_time.local_date_time, d, td, ny_tz, local_time.local_date_time.EXCEPTION_ON_ERROR)
        
        az_time3 = local_time.local_date_time(d, h, az_tz, False)
        self.failUnless( az_time3.zone().std_zone_abbrev() == "MST") 
        self.failUnless( az_time3.zone().base_utc_offset() == posix_time.hours(-7) )
        self.failUnless( az_time3.zone().has_dst() == False )
        self.failUnless( az_time3.is_dst() == False )
        self.failUnless( az_time3.to_string() == "2004-Apr-04 12:00:00 MST" )
        
        null_tz_time = local_time.local_date_time(d, h, null_tz, True)
        
        self.failUnless( null_tz_time.is_dst() == False )
        self.failUnless( null_tz_time.to_string() == "2004-Apr-04 12:00:00 UTC" )
        
        self.failUnlessRaises( Exception
                               , local_time.local_date_time
                               , gregorian.date(2004,date_time.Apr,4)
                               , posix_time.time_duration(2,30,0)
                               , ny_tz
                               , True )
          
        d = gregorian.date(2004, date_time.Apr,4)
        td = posix_time.time_duration(1,15,0) 
        lt1 = local_time.local_date_time(d,td,ny_tz,False)
        lt2 = local_time.local_date_time( posix_time.ptime(d
                                                           , posix_time.time_duration(6,15,0) )
                                          , ny_tz)
        self.failUnless( lt1.utc_time() == lt2.utc_time( ))
        self.failUnless( lt1.is_dst() == False )
        self.failUnless( lt2.is_dst() == False )
        lt1 += posix_time.hours(1)
        lt2 += posix_time.hours(1)
        self.failUnless( lt1.is_dst() == True)
        self.failUnless( lt2.is_dst() == True)
    
        d = gregorian.date(2004,date_time.Oct,31)
        td = posix_time.time_duration(1,15,0) 
        lt1 = local_time.local_date_time(d,td,ny_tz,True)
        lt2 = local_time.local_date_time(posix_time.ptime(d
                                                          , posix_time.time_duration(5,15,0))
                                         , ny_tz)
        self.failUnless( lt1.utc_time() == lt2.utc_time() )
        self.failUnless( lt1.is_dst() == True )
        self.failUnless( lt2.is_dst() == True )
        lt1 += posix_time.hours(1)
        lt2 += posix_time.hours(1)
        self.failUnless( lt1.is_dst() == False )
        self.failUnless( lt2.is_dst() == False )
        
        ny_time = az_time.local_time_in(ny_tz, posix_time.time_duration(0,0,0))
        self.failUnless( ny_time.zone().std_zone_abbrev() == "EST" )
        self.failUnless( ny_time.zone().base_utc_offset() == posix_time.hours(-5) )
        self.failUnless( ny_time.zone().has_dst() == True )
        self.failUnless( ny_time.to_string() == "2003-Dec-20 07:00:00 EST" )
        ny_time += posix_time.hours(3)
        self.failUnless( ny_time.to_string() == "2003-Dec-20 10:00:00 EST" )
        ny_time += gregorian.days(3)
        self.failUnless( ny_time.to_string() == "2003-Dec-23 10:00:00 EST" )

        d = gregorian.date(2003, date_time.Aug, 28)
        sv_pt = posix_time.ptime(date_time.pos_infin)
        sv_lt = local_time.local_date_time(sv_pt, ny_tz)
        utc_pt = posix_time.ptime(d, posix_time.hours(12))
        
        utc_lt = local_time.local_date_time(utc_pt, null_tz)           
        az_lt = local_time.local_date_time(d, posix_time.hours(5), az_tz, False)  
        ny_lt = local_time.local_date_time(d, posix_time.hours(8), ny_tz, True)   
        au_lt = local_time.local_date_time(d, posix_time.hours(22), sydney, False)
        
        self.failUnless( az_lt == utc_lt ) 
        self.failUnless( ny_lt == utc_lt ) 
        self.failUnless( au_lt == utc_lt ) 
        self.failUnless( au_lt <= utc_lt ) 
        self.failUnless( au_lt >= utc_lt ) 
        self.failUnless( az_lt == ny_lt )  
        self.failUnless( az_lt < ny_lt + posix_time.seconds(1) ) 
        self.failUnless( az_lt + posix_time.seconds(1) >  ny_lt ) 
        self.failUnless( az_lt <= ny_lt ) 
        self.failUnless( az_lt >=  ny_lt ) 
        self.failUnless( az_lt + posix_time.seconds(1) !=  ny_lt ) 
        
        au_lt += posix_time.hours(1)
        self.failUnless( au_lt != utc_lt ) 
        self.failUnless( utc_lt <= au_lt )  
        self.failUnless( au_lt >= utc_lt ) 
        self.failUnless( utc_lt < au_lt ) 
        self.failUnless( au_lt > utc_lt )
        au_lt -= posix_time.hours(1)
        self.failUnless( au_lt == utc_lt ) 
        
        self.failUnless( (az_lt + gregorian.days(2)).to_string() == "2003-Aug-30 05:00:00 MST")
        self.failUnless( (az_lt - gregorian.days(2)).to_string() == "2003-Aug-26 05:00:00 MST")
        
        az_lt += gregorian.days(2)
        self.failUnless( az_lt.to_string() == "2003-Aug-30 05:00:00 MST")
        az_lt -= gregorian.days(2)
        self.failUnless( az_lt.to_string() == "2003-Aug-28 05:00:00 MST")
        self.failUnless( (az_lt + posix_time.hours(2)).to_string() == "2003-Aug-28 07:00:00 MST")
        self.failUnless( (az_lt - posix_time.hours(2)).to_string() == "2003-Aug-28 03:00:00 MST")
        
        self.failUnless( sv_lt > au_lt) 
        sv_lt2 = sv_lt + gregorian.days(2)
        self.failUnless( sv_lt2 == sv_lt)

        m = gregorian.months(2)
        y = gregorian.years(2)
        self.failUnless( (az_lt + m).to_string() == "2003-Oct-28 05:00:00 MST")
        az_lt += m
        self.failUnless( az_lt.to_string() == "2003-Oct-28 05:00:00 MST")
        self.failUnless( (az_lt - m).to_string() == "2003-Aug-28 05:00:00 MST")
        az_lt -= m
        self.failUnless( az_lt.to_string() == "2003-Aug-28 05:00:00 MST")
        self.failUnless( (az_lt + y).to_string() == "2005-Aug-28 05:00:00 MST")
        az_lt += y
        self.failUnless( az_lt.to_string() == "2005-Aug-28 05:00:00 MST")
        self.failUnless( (az_lt - y).to_string() == "2003-Aug-28 05:00:00 MST")
        az_lt -= y
        self.failUnless( az_lt.to_string() == "2003-Aug-28 05:00:00 MST")

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()