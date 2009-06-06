#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import time
import sys
sys.path.append( '../..' )

from pyboost import date_time
from pyboost.date_time import gregorian
from pyboost.date_time import posix_time
from pyboost.date_time import to_iso_string
from pyboost.date_time import to_simple_string
import unittest

class tester_t( unittest.TestCase ):
    def __init__( self, *args ):
        unittest.TestCase.__init__( self, *args )
        
    def test_clock(self):
        tl = posix_time.second_clock.local_time()
        print to_simple_string(tl)

        tu = posix_time.second_clock.universal_time()
        print to_simple_string(tu)

        tl = posix_time.microsec_clock.local_time()
        print to_simple_string(tl)

        tu = posix_time.microsec_clock.universal_time()
        print to_simple_string(tu)

    def test_time_period(self):
        d1 = gregorian.date(2001,date_time.Jan, 1)
        t1 = posix_time.ptime(d1,posix_time.hours(2))
        t2 = posix_time.ptime(d1,posix_time.hours(3))
        p1 = posix_time.time_period(t1,t2) 
        p2 = posix_time.time_period(t1,t2) 
        p4 = posix_time.time_period(t1, posix_time.hours(1))
        self.failUnless( p4.length() == posix_time.hours(1))
        
        self.failUnless( p1 == p4)
        self.failUnless( p1.begin() == t1)
        self.failUnless( p1.end() == t2)
        self.failUnless( p1.last() == t2-posix_time.time_duration.unit())
        
        self.failUnless( p1.contains(t1))
        self.failUnless( not p1.contains(t2))
        self.failUnless( p1.contains(t2-posix_time.seconds(1)))
        t3 = posix_time.ptime(gregorian.date(2001,date_time.Jan,1),posix_time.hours(4))
        p5 = posix_time.time_period(t2,t3)
        self.failUnless( not p1.contains(p5.begin()))
        self.failUnless( not p5.contains(p1.begin()))
        self.failUnless( not (p1 == p5))
        self.failUnless( p1 < p5)
        self.failUnless( p5 > p1)
        self.failUnless( p5 != p1)
        self.failUnless( p1.intersects(p1))
        self.failUnless( not p1.intersects(p5))
        self.failUnless( not p5.intersects(p1))
        
        p6 = posix_time.time_period(t2,t3)
        p6.shift(posix_time.minutes(30))        
        
        self.failUnless( p5.intersects(p6))
        self.failUnless( p6.intersects(p5))
        self.failUnless( p5.contains(p6.begin()))
        p6.shift(posix_time.minutes(30))
        
        
        self.failUnless( not p5.intersects(p6))
        self.failUnless( not p6.intersects(p5))
        p6.shift(posix_time.minutes(-30))
        
        p7 = p5.intersection(p6)
        
        self.failUnless(  
        p7 == posix_time.time_period( posix_time.ptime(d1,posix_time.time_duration(3,30,0))
                                     , posix_time.ptime(d1,posix_time.time_duration(4,0,0))))
    
    def test_duration( self ):
        td0 = posix_time.time_duration()
        self.failUnless( posix_time.time_duration().ticks() == 0)
        td1 = posix_time.time_duration(1,25,0)
        td3 = posix_time.time_duration(td1.hours(),td1.minutes(),td1.seconds())
        self.failUnless( td1 == td3)
        td1 = -td1 
        td3 = posix_time.time_duration(td1.hours(),td1.minutes(),td1.seconds())
        self.failUnless( td1 == td3)
        t_1 = posix_time.time_duration(0,1,40)
        t_2 = posix_time.time_duration(0,1,41)
        self.failUnless( not ( t_2 < t_2 ) )
        self.failUnless( t_1 == t_1)
        self.failUnless( t_1 >= t_1)
        self.failUnless( t_2 >= t_1)
        self.failUnless( t_2 <= t_2)
        self.failUnless( t_2 > t_1)
        self.failUnless( not (t_1 > t_2))
        t_3 = posix_time.time_duration(0,1,41)
        self.failUnless( t_2 == t_3)
        t_4 = posix_time.time_duration(0,1,41)
        self.failUnless( t_2 == t_4)
        
        t_5 = posix_time.time_duration(1,30,20,10) 
        t_5 /= 2
        self.failUnless( t_5.hours() == 0 
                         and t_5.minutes() == 45 
                         and t_5.seconds() == 10 
                         and  t_5.fractional_seconds() == 5)
        t_5 = posix_time.time_duration(3,15,8,0) / 2        
        self.failUnless( t_5 == posix_time.time_duration(1,37,34,0) )
        td = posix_time.hours( 5 )
        td *= 5
        self.failUnless( td == posix_time.time_duration( 25, 0,0,0 ) )
        t_5 = t_2 + t_1
        self.failUnless( t_5 == posix_time.time_duration(0,3,21) )

        td_a = posix_time.time_duration(5,5,5,5)
        td_b = posix_time.time_duration(4,4,4,4)
        td_c = posix_time.time_duration(2,2,2,2)
        td_a += td_b
        self.failUnless( td_a == posix_time.time_duration(9,9,9,9) )
        td_d = td_b - td_c 
        self.failUnless( td_d == posix_time.time_duration(2,2,2,2) )
        td_d -= td_b 
        self.failUnless( td_d == td_c - td_b)
        
        utd = posix_time.time_duration(1,2,3,4)
        utd2 = -utd
        
        self.failUnless( utd2.hours() == -1
                         and  utd2.minutes() == -2
                         and  utd2.seconds() == -3
                         and  utd2.fractional_seconds() == -4 )
        utd2 = -posix_time.hours(5)
        self.failUnless( utd2.hours() == -5)
        utd2 = -utd2
        self.failUnless( utd2.hours() == 5)

        t_6 = posix_time.time_duration(5,4,3)  
        self.failUnless( t_6.hours() == 5)
        self.failUnless( t_6.minutes() == 4)
        self.failUnless( t_6.seconds() == 3)
        
        self.failUnless( t_6.total_seconds() == 18243)
        
        tenhours = posix_time.hours(10)
        fivemin = posix_time.minutes(5)
        t7 = posix_time.time_duration(1,2,3) + tenhours + fivemin
        self.failUnless( t7 == posix_time.time_duration(11,7,3))
        t8 = tenhours + posix_time.time_duration(1,2,3) +  fivemin
        self.failUnless(t8 == posix_time.time_duration(11,7,3))
        if posix_time.time_duration.resolution() >= date_time.micro:
            t_9 = posix_time.time_duration(5,4,3,9876) 
            self.failUnless(  t_9.hours() == 5)
            self.failUnless(  t_9.minutes() == 4)
            self.failUnless(  t_9.seconds() == 3)
            self.failUnless(  t_9.fractional_seconds() == 9876)
            self.failUnless( t_9.total_seconds() == 18243)
        
        if posix_time.time_duration.resolution() >= date_time.tenth:
            t_10 = posix_time.time_duration(5,4,3,9) 
            self.failUnless(  t_10.hours() == 5)
            self.failUnless(  t_10.minutes() == 4)
            self.failUnless(  t_10.seconds() == 3)
            self.failUnless(  t_10.fractional_seconds() == 9)
            self.failUnless( t_10.total_seconds() == 18243)

        if posix_time.time_duration.resolution() >= date_time.milli:
            ms = posix_time.millisec(9)        
            if posix_time.time_duration.resolution() == date_time.nano:
                self.failUnless(  ms.fractional_seconds() == 9000000)
                self.failUnless( ms.total_seconds() == 0)
                self.failUnless( ms.total_milliseconds() == 9)
                self.failUnless( posix_time.time_duration.ticks_per_second() == 1000000000)
            else:
                self.failUnless(  ms.fractional_seconds() == 9000)
                self.failUnless( ms.total_seconds() == 0)
                self.failUnless( ms.total_milliseconds() == 9)

        if hasattr( posix_time, 'nanoseconds' ):
            if posix_time.time_duration.resolution() >= date_time.nano:
                ns = posix_time.nanosec(9)
                
                self.failUnless(  ns.fractional_seconds() == 9)
                self.failUnless(  ns.total_nanoseconds() == 9)
                self.failUnless(  ns.total_microseconds() == 0)
                
                ns18 = ns + ns
                self.failUnless(  ns18.fractional_seconds() == 18)
                
                ns2 = posix_time.nanosec(1000000000) 
                self.failUnless(  ns2 == seconds(1))
                self.failUnless(  ns2.total_seconds() == 1)
                
                self.failUnless( (nanosec(3)/2) == nanosec(1))
                self.failUnless( nanosec(3)*1000 == microsec(3))
        
        t_11 = posix_time.time_duration(3600,0,0) 
        self.failUnless(  t_11.hours() == 3600)
        self.failUnless(  t_11.total_seconds() == 12960000)
        
        td_12 = posix_time.time_duration(1,2,3,10) 
        
        self.failUnless(  td_12.total_seconds() == 3723)
        
        self.failUnless( posix_time.hours(2)/2 == posix_time.hours(1))
        self.failUnless( posix_time.hours(3)/2 == posix_time.minutes(90))
        
        self.failUnless( posix_time.time_duration(3,0,0)*2 == posix_time.hours(6))
        self.failUnless( posix_time.hours(3600)*1000 == posix_time.hours(3600000))
        
        pi_dur = posix_time.time_duration(date_time.pos_infin)
        ni_dur = posix_time.time_duration(date_time.neg_infin)
        ndt_dur = posix_time.time_duration(date_time.not_a_date_time)
        self.failUnless( pi_dur + ni_dur == ndt_dur)
        self.failUnless( pi_dur / 3 == pi_dur)
        self.failUnless( pi_dur + td_12 == pi_dur)
        self.failUnless( pi_dur - td_12 == pi_dur)
        self.failUnless( -pi_dur == ni_dur)
        self.failUnless( ni_dur < pi_dur)
        self.failUnless( ni_dur < td_12)
        self.failUnless( pi_dur > td_12)
         
        self.failUnless( to_simple_string(pi_dur)=="+infinity")
        self.failUnless( to_simple_string(ni_dur)=="-infinity")
        self.failUnless( to_simple_string(ndt_dur)=="not-a-date-time")
        
        t1 = posix_time.ptime( gregorian.date(2001,7,14))
        t2 = posix_time.ptime( gregorian.date(2002,7,14))
        self.failUnless(  365*24 == (t2-t1).hours())
        self.failUnless(  365*24*3600 == (t2-t1).total_seconds())
                
        self.failUnless( posix_time.seconds(1).total_milliseconds() == 1000)
        self.failUnless( posix_time.seconds(1).total_microseconds() == 1000000)
        self.failUnless( posix_time.seconds(1).total_nanoseconds() == 1000000000)
        self.failUnless( posix_time.hours(1).total_milliseconds() == 3600*1000)
        
        tms = 3600*1000000*1001
        self.failUnless( posix_time.hours(1001).total_microseconds() == tms)
        tms = 3600*365*24*1000
        self.failUnless( (t2-t1).total_milliseconds() == tms)
        tms = 3600*365*24*1000000000
        self.failUnless( (t2-t1).total_nanoseconds() == tms)
        print posix_time.microseconds(25).ticks()/posix_time.time_duration.ticks_per_second()        

    def test_greg_duration_operators(self):
        p = posix_time.ptime( gregorian.date(2001, date_time.Oct, 31), posix_time.hours(5))
        self.failUnless( posix_time.ptime( gregorian.date(2002, date_time.Feb, 28)
                                           , posix_time.hours(5)) 
                         == p + gregorian.months(4) )
        p += gregorian.months(4)
        self.failUnless( posix_time.ptime(gregorian.date(2002, date_time.Feb, 28), posix_time.hours(5)) == p)

        p = posix_time.ptime( gregorian.date(2001, date_time.Oct, 31), posix_time.hours(5))
        self.failUnless( posix_time.ptime(gregorian.date(2001, date_time.Apr, 30), posix_time.hours(5)) 
                         == p - gregorian.months(6))
        p -= gregorian.months(6)
        self.failUnless( posix_time.ptime(gregorian.date(2001, date_time.Apr, 30)
                                          , posix_time.hours(5)) == p)
        
        p = posix_time.ptime(gregorian.date(2001, date_time.Feb, 28), posix_time.hours(5))
        self.failUnless( posix_time.ptime( gregorian.date(2004, date_time.Feb, 29)
                                           , posix_time.hours(5)) 
                         == p + gregorian.years(3))
        p += gregorian.years(3)
        self.failUnless( posix_time.ptime( gregorian.date(2004, date_time.Feb, 29)
                                           , posix_time.hours(5)) 
                         == p )
        
        p = posix_time.ptime(gregorian.date(2000, date_time.Feb, 29), posix_time.hours(5))
        self.failUnless( posix_time.ptime(gregorian.date(1998, date_time.Feb, 28), posix_time.hours(5)) 
                         == p - gregorian.years(2))
        p -= gregorian.years(2)
        self.failUnless( posix_time.ptime(gregorian.date(1998, date_time.Feb, 28), posix_time.hours(5)) 
                         == p)

    def test_time(self):
        td_pi = posix_time.time_duration(date_time.pos_infin)
        td_ni = posix_time.time_duration(date_time.neg_infin)
        td_ndt = posix_time.time_duration(date_time.not_a_date_time)
        dd_pi = gregorian.date_duration(date_time.pos_infin)
        dd_ni = gregorian.date_duration(date_time.neg_infin)
        dd_ndt = gregorian.date_duration(date_time.not_a_date_time)
        d_pi = gregorian.date(date_time.pos_infin)
        d_ni = gregorian.date(date_time.neg_infin)
        d_ndt = gregorian.date(date_time.not_a_date_time)
        td = posix_time.time_duration(1,2,3,4)
        dd = gregorian.date_duration(1234);
        d = gregorian.date(2003, date_time.Oct,31)

        self.failUnless(  posix_time.ptime() == posix_time.ptime(date_time.not_a_date_time) )
        
        s = "+infinity"
        p_sv1 = posix_time.ptime(date_time.pos_infin);
        self.failUnless(  to_simple_string(p_sv1) == s )
        result = p_sv1 + dd
        self.failUnless( to_iso_string(result) == s )
        self.failUnless( result.is_special() )
        result = p_sv1 - dd
        self.failUnless( to_iso_string(result) == s)
        result = p_sv1 - dd_ni
        self.failUnless( to_iso_string(result) == s)
        p_sv2 = posix_time.ptime(date_time.neg_infin)

        s = "-infinity"
        self.failUnless( to_iso_string(p_sv2) == s)
        result = p_sv2 - td_pi
        self.failUnless( to_iso_string(result) == s )
        p_sv3 = posix_time.ptime(date_time.not_a_date_time)
        self.failUnless(  p_sv3.is_special() )
        s = "not-a-date-time";
        self.failUnless(  to_iso_string(p_sv3) == s)
        result = p_sv3 + td
        self.failUnless(  to_iso_string(result) == s)
        result = p_sv3 - td
        self.failUnless(  to_iso_string(result) == s)
        result = p_sv2 + td_pi
        self.failUnless(  to_iso_string(result) == s)
        result = p_sv1 + dd_ni
        self.failUnless(  to_iso_string(result) == s)
        result = p_sv1 + dd_ndt
        self.failUnless(  to_iso_string(result) == s)

        p_sv1 = posix_time.ptime(d_pi, td)
        s = "+infinity"
        self.failUnless( to_simple_string(p_sv1) == s)
        p_sv2 = posix_time.ptime(d_ni, td)
        s = "-infinity";
        self.failUnless( to_iso_string(p_sv2) == s)
        p_sv3 = posix_time.ptime(d_ndt, td)
        s = "not-a-date-time";
        self.failUnless( to_iso_string(p_sv3) == s)

        p_sv1 = posix_time.ptime(d_ndt, td)
        s = "not-a-date-time"
        self.failUnless( to_simple_string(p_sv1) == s)
        p_sv2 = posix_time.ptime(d, td_ndt)
        self.failUnless( to_iso_string(p_sv2) == s)
        p_sv3 = posix_time.ptime(d_pi, td_ni)
        self.failUnless( to_iso_string(p_sv3) == s)

        p_sv1 = posix_time.ptime(d, td_pi)
        pt = posix_time.ptime(d,td)
        s = "+infinity"
        self.failUnless( to_simple_string(p_sv1) == s)
        self.failUnless( to_simple_string(p_sv1.date()) == s)
        p_sv2 = posix_time.ptime(d, td_ni)
        
        s = "-infinity"
        self.failUnless( to_iso_string(p_sv2) == s)
        self.failUnless( to_simple_string(p_sv2.time_of_day()) == s)
        p_sv3 = posix_time.ptime(d, td_ndt)
        s = "not-a-date-time"
        self.failUnless( to_iso_string(p_sv3) == s)
        self.failUnless( to_simple_string(p_sv3.date()) == s)
        self.failUnless( to_simple_string(p_sv3.time_of_day()) == s)
        self.failUnless( p_sv2 < p_sv1)
        self.failUnless( p_sv2 < pt)
        self.failUnless( pt < p_sv1)
        self.failUnless( p_sv2 <= p_sv2)
        self.failUnless( p_sv2 <= pt)
        self.failUnless( p_sv1 >= pt)
        self.failUnless( p_sv1 != p_sv2)
        self.failUnless( p_sv3 != p_sv2)
        self.failUnless( pt != p_sv1)
        
        self.failUnless( p_sv1.is_infinity() and p_sv1.is_pos_infinity())
        self.failUnless( p_sv2.is_infinity() and p_sv2.is_neg_infinity())
        self.failUnless( not p_sv3.is_infinity() and p_sv3.is_not_a_date_time())
        
        self.failUnless( p_sv1 + dd == p_sv1)
        self.failUnless( pt - dd_pi == p_sv2)
        self.failUnless( pt - dd_ndt == p_sv3)
        
        self.failUnless( p_sv2 + td == p_sv2)
        self.failUnless( pt - td_ni == p_sv1)
        self.failUnless( pt + td_ndt == p_sv3)
        self.failUnless( pt - p_sv1 == td_ni)
        self.failUnless( pt - p_sv2 == td_pi)
        self.failUnless( pt - p_sv3 == td_ndt)
        self.failUnless( p_sv2 - p_sv2 == td_ndt)

        d = gregorian.date(2001, date_time.Dec,1)
        td = posix_time.time_duration(5,4,3)
        t1 = posix_time.ptime(d, td)
        self.failUnless( t1.date() == d)
        self.failUnless( t1.time_of_day() == td)
        self.failUnless( posix_time.ptime( gregorian.date(2005,10,30), posix_time.hours(25)) 
                         == posix_time.ptime(gregorian.date(2005,10,31), posix_time.hours(1)))
        t2 = posix_time.ptime(d, td)
        t3 = posix_time.ptime(d, td)
        self.failUnless( t3.date() == d)
        self.failUnless( t3.time_of_day() == td)
        self.failUnless( t1 == t3)
        d2 = gregorian.date(2001,date_time.Jan,1)
        t4 = posix_time.ptime(d2, td)
        self.failUnless( not (t1 == t4))
        td1 = posix_time.time_duration(5,4,0)
        t5 = posix_time.ptime(d, td1)
        self.failUnless( not (t1 == t5))
        self.failUnless( t1 != t5);
        
        self.failUnless( not (t1 < t1) )
        self.failUnless( t4 < t1)
        self.failUnless( t5 < t1)
        self.failUnless( t1 <= t1)
        self.failUnless( t1 >= t1)
        
        twodays = gregorian.date_duration(2)
        t6 = t1 + twodays
        d3 = gregorian.date(2001,date_time.Dec,3)
        self.failUnless( t6 == posix_time.ptime(d3,td) )
        t7 = t1 - twodays
        self.failUnless( t7 == posix_time.ptime(gregorian.date(2001,date_time.Nov,29),td) )
        
        t6b = posix_time.ptime( gregorian.date(2003, date_time.Oct,31)
                                , posix_time.time_duration(10,0,0,0))
        t6b += gregorian.date_duration(55)
        self.failUnless( t6b == posix_time.ptime(gregorian.date(2003,date_time.Dec,25)
                                                 , posix_time.time_duration(10,0,0,0)))
        t6b += posix_time.hours(6)
        self.failUnless( t6b == posix_time.ptime(gregorian.date(2003,date_time.Dec,25)
                                                 , posix_time.time_duration(16,0,0,0)))
        t6b -= gregorian.date_duration(55)
        self.failUnless( t6b == posix_time.ptime(gregorian.date(2003,date_time.Oct,31)
                                                 , posix_time.time_duration(16,0,0,0)))
        t6b -= posix_time.hours(6)
        self.failUnless( t6b == posix_time.ptime(gregorian.date(2003,date_time.Oct,31)
                                                 , posix_time.time_duration(10,0,0,0)));
        t6b += posix_time.hours(25);
        self.failUnless( t6b ==
              posix_time.ptime(gregorian.date(2003,date_time.Nov,1), posix_time.time_duration(11,0,0,0)));
        t6b -= posix_time.hours(49);
        self.failUnless( t6b == posix_time.ptime(gregorian.date(2003,date_time.Oct,30)
                                                 , posix_time.time_duration(10,0,0,0)));

        td2 = posix_time.time_duration(1,2,3)
        t8 = posix_time.ptime(gregorian.date(2001,date_time.Dec,1))
        t9 = t8 + td2 
        t10 = posix_time.ptime(gregorian.date(2001,date_time.Dec,1)
                               ,posix_time.time_duration(1,2,3))
        
        self.failUnless( t9 == t10)
        t9 = posix_time.ptime(gregorian.date(2001,date_time.Dec,1)
                              , posix_time.time_duration(12,0,0)); 
        td3 = posix_time.time_duration(-4,0,0)
        self.failUnless( t9 + td3 
                         == posix_time.ptime(gregorian.date(2001,date_time.Dec,1)
                                             , posix_time.time_duration(8,0,0)) )
        td3 = posix_time.time_duration(24,0,0)
        self.failUnless( t8+td3 == posix_time.ptime(gregorian.date(2001,date_time.Dec,2) ) )
        td4 = posix_time.time_duration(24,0,1)
        self.failUnless( t8+td4 == posix_time.ptime(gregorian.date(2001,date_time.Dec,2)
                                                    , posix_time.time_duration(0,0,1)));

        td5 = posix_time.time_duration(168,0,1)
        self.failUnless( t8+td5 == posix_time.ptime(gregorian.date(2001,date_time.Dec,8)
                                                    , posix_time.time_duration(0,0,1)))
        
        t11 = posix_time.ptime(gregorian.date(2001,date_time.Dec,1)
                               , posix_time.time_duration(12,0,0))
        td6 = posix_time.time_duration(12,0,1)
        t12 = t11-td6
        self.failUnless( t12 == posix_time.ptime(gregorian.date(2001,date_time.Nov,30)
                                                 , posix_time.time_duration(23,59,59)))
        self.failUnless( t11-posix_time.time_duration(13,0,0)
                         == posix_time.ptime(gregorian.date(2001,date_time.Nov,30)
                                             , posix_time.time_duration(23,0,0)))
        self.failUnless( t11-posix_time.time_duration(-13,0,0)
                         == posix_time.ptime(gregorian.date(2001,date_time.Dec,2)
                                             , posix_time.time_duration(1,0,0)))
        
        t13 = posix_time.ptime(d, posix_time.hours(3))
        t14 = posix_time.ptime(d, posix_time.hours(4))
        t14a = posix_time.ptime(d+gregorian.date_duration(1), posix_time.hours(4))
        
        self.failUnless( t14-t13 == posix_time.hours(1))
        
        self.failUnless( t13 - t14 == posix_time.hours(-1))
        self.failUnless( t14a-t14 == posix_time.hours(24))
        
        t15 = posix_time.ptime(d, posix_time.time_duration(0,0,0,1))
        t16 = posix_time.ptime(d, posix_time.time_duration(0,0,0,2))
        self.failUnless( t15 + posix_time.time_duration.unit() == t16)
        self.failUnless( t16 - posix_time.time_duration.unit() == t15)
                
        self.failUnless( posix_time.ptime(date_time.min_date_time)
                         == posix_time.ptime(gregorian.date(1400,1,1)
                                             , posix_time.time_duration(0,0,0,0)) )
        
        self.failUnless( posix_time.ptime(date_time.max_date_time)
                         == posix_time.ptime(gregorian.date(9999,12,31)
                                             , posix_time.hours(24)- posix_time.time_duration.unit()))

    def test_microsec_time_clock(self):
        last = posix_time.microsec_clock.local_time()
        time.sleep(0.025)
        t1 = posix_time.second_clock.local_time()
        t2 = posix_time.microsec_clock.local_time()
        self.failUnless( t1.time_of_day().hours() == t2.time_of_day().hours() )
        self.failUnless( t1.time_of_day().minutes() == t2.time_of_day().minutes())
        self.failUnless( t1.time_of_day().minutes() == t2.time_of_day().minutes())
        self.failUnless( t1.date() == t2.date() )
        self.failUnless( last < t2 )

        last = posix_time.microsec_clock.universal_time()
        time.sleep(0.025)
        t1 = posix_time.second_clock.universal_time()
        t2 = posix_time.microsec_clock.universal_time()
        self.failUnless( t1.time_of_day().hours() == t2.time_of_day().hours() )
        self.failUnless( t1.time_of_day().minutes() == t2.time_of_day().minutes())
        self.failUnless( t1.time_of_day().minutes() == t2.time_of_day().minutes())
        self.failUnless( t1.date() == t2.date() )
        self.failUnless( last < t2 )


def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()