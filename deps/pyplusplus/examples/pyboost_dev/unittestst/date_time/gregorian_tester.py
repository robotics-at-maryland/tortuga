#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

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
    
    def test_current_day(self):
        d1 = gregorian.day_clock.local_day()
        print to_iso_string( d1 )
        
        ymd = gregorian.day_clock.local_day_ymd()
        
        print '%i-%s-%i' % ( ymd.year.as_number()
                             , ymd.month.as_long_string()
                             , ymd.day.as_number() )

        d2 = gregorian.day_clock.universal_day()
        print to_iso_string( d2 )

        ymd2 = gregorian.day_clock.universal_day_ymd()
        print '%i-%s-%i' % ( ymd2.year.as_number()
                             , ymd2.month.as_long_string()
                             , ymd2.day.as_number() )

    def test_date(self):
        self.failUnless( gregorian.date() == gregorian.date( date_time.not_a_date_time ) )
        d1 = gregorian.date( 1900, 1, 1 )
        d2 = gregorian.date( 2000, 1, 1 )
        d3 = gregorian.date( 1900, 1, 1 )
        d4 = gregorian.date( 2000, 12, 31 ) 
        d4a = gregorian.date( 2000, date_time.Dec, 31 ) 
        self.failUnless( d4 == d4a )
        self.failUnless( d1.day().as_number() == 1 )
        self.failUnless( d1.month().as_number() == 1 )
        self.failUnless( d1.year().as_number() == 1900 )
        self.failUnless( d4a.month().as_number() == 12 )
        self.failUnless( d1 < d2 )
        self.failUnless( not ( d1 < d1 ) )
        self.failUnless( d1 <= d1 )
        self.failUnless( d2 > d1 )
        self.failUnless( not ( d1 > d1 ) )
        self.failUnless( d1 >= d1 )
        self.failUnless( d2 != d1 )
        self.failUnless( d1 == d3 )
        ymd = d1.year_month_day()
        self.failUnless( ymd.year.as_number() == 1900 )
        self.failUnless( ymd.month.as_number() == 1 )
        self.failUnless( ymd.day.as_number() == 1 )
  
        answers = [ gregorian.date( 1900, date_time.Jan, 1 )
                    , gregorian.date( 1900, date_time.Jan, 4 )
                    , gregorian.date( 1900, date_time.Jan, 7 )
                    , gregorian.date( 1900, date_time.Jan, 10 )
                    , gregorian.date( 1900, date_time.Jan, 13 )
        ]
        off = gregorian.date_duration( 3 )
        d5 = gregorian.date( 1900, 1, 1 )
        for i in range(5):
            self.failUnless( d5 == answers[i] )
            d5 = d5 + off
  
        answers1 = [ gregorian.date(2000, 2 ,26)
                     , gregorian.date(2000, 2 ,28)
                     , gregorian.date(2000, date_time.Mar ,1 )]
        d8 = gregorian.date(2000, date_time.Feb, 26)
        for i in range(3):
            self.failUnless( d8 == answers1[i] )
            d8 += gregorian.days(2)

        d6 = gregorian.date(2000,2,28)
        d7 = gregorian.date(2000,3,1)
        twoDays = gregorian.date_duration(2)
        negtwoDays = gregorian.date_duration(-2)
        zeroDays = gregorian.date_duration(0)
        self.failUnless( twoDays    == d7 - d6 )
        self.failUnless( negtwoDays == d6 - d7 )
        self.failUnless( zeroDays   == d6 - d6 )
        self.failUnless( d6 + twoDays == d7 )
        self.failUnless( d7 - twoDays == d6 )
        self.failUnless( gregorian.date(1999,1,1) + gregorian.date_duration(365) 
                         == gregorian.date(2000,1,1) )
        self.failUnless( gregorian.date(1999,1,1) + zeroDays == gregorian.date(1999,1,1) )        

        d = gregorian.date(2003, date_time.Oct, 31)
        dd = gregorian.date_duration(55)
        d += dd
        self.failUnless( d == gregorian.date(2003, date_time.Dec,25) )
        d -= dd;
        self.failUnless( d == gregorian.date(2003, date_time.Oct,31) )

        d = gregorian.date(2003,date_time.Oct, 31)
        dd1 = gregorian.date_duration( date_time.pos_infin)
        dd2 = gregorian.date_duration( date_time.neg_infin)
        dd3 = gregorian.date_duration( date_time.not_a_date_time)

        self.failUnless( d + dd1 == gregorian.date(date_time.pos_infin))
        self.failUnless( d + dd2 == gregorian.date(date_time.neg_infin))
        self.failUnless( d + dd3 == gregorian.date(date_time.not_a_date_time))
        self.failUnless( d - dd1 == gregorian.date(date_time.neg_infin))
        self.failUnless( d - dd2 == gregorian.date(date_time.pos_infin))
        self.failUnless( d - dd3 == gregorian.date(date_time.not_a_date_time))
        self.failUnless( gregorian.date(date_time.pos_infin) + dd1 
                         == gregorian.date(date_time.pos_infin))
        self.failUnless( gregorian.date(date_time.pos_infin) - dd1 
                         == gregorian.date(date_time.not_a_date_time))
        self.failUnless( gregorian.date(date_time.neg_infin) + dd1 
                         == gregorian.date(date_time.not_a_date_time))
        self.failUnless( gregorian.date(date_time.neg_infin) - dd1 
                         == gregorian.date(date_time.neg_infin))
              
        self.failUnlessRaises( Exception, lambda: gregorian.date( 2000, 1, 32 ) ) 
        self.failUnlessRaises( Exception, lambda: gregorian.date( 2000, 1, 0 ) ) 
        self.failUnlessRaises( Exception, lambda: gregorian.date( 2000, 2, 31 ) ) 
     
        d15 = gregorian.date(1400,1,1)
        self.failUnless( d15.julian_day() == 2232400)
        self.failUnless( d15.modjulian_day() == -167601)

        d16 = gregorian.date(2004,2,29)
        self.failUnless( d16.julian_day() == 2453065)
        self.failUnless( d16.modjulian_day() == 53064)

        d31 = gregorian.date(2000, date_time.Jun, 1)
        self.failUnless( d31.week_number() == 22)
        
        d38 = gregorian.date(2001, 1, 1)
        self.failUnless( d38.day_of_year().as_number() == 1)

    def test_date_duration(self):
        threeDays = gregorian.date_duration(3)
        twoDays = gregorian.date_duration(2)
        self.failUnless( threeDays == threeDays )
        self.failUnless( threeDays == gregorian.date_duration(3) )
        self.failUnless( threeDays != twoDays )
        self.failUnless( twoDays < threeDays )
        self.failUnless( not ( threeDays < twoDays ) )
        self.failUnless( not ( threeDays < threeDays ) )
        self.failUnless( not ( threeDays > threeDays ) )
        self.failUnless( threeDays >= threeDays )
        self.failUnless( not ( twoDays >= threeDays ) )
        self.failUnless( twoDays + threeDays == gregorian.date_duration(5) )
        fiveDays = gregorian.date_duration(3)
        fiveDays += twoDays
        self.failUnless( fiveDays == gregorian.date_duration(5) )
        derivedOneDay = threeDays - twoDays
        self.failUnless( twoDays - threeDays == gregorian.date_duration(-1) )
        oneDay = gregorian.date_duration(1)
        self.failUnless( oneDay == derivedOneDay )
        fiveDaysDerived = gregorian.date_duration( 10 )
        fiveDaysDerived -= fiveDays
        self.failUnless( fiveDaysDerived == fiveDays)

        oneDay = twoDays / 2
        self.failUnless( oneDay.days() == 1 )
        hundred = gregorian.date_duration(100)
        hundred /= -10
        self.failUnless( hundred.days() == -10 and hundred.is_negative())
        self.failUnless( gregorian.date_duration( -gregorian.date_duration( 123 ).days() ).days() == -123 )
        pos_dur = gregorian.date_duration(123)
        neg_dur = -pos_dur
        self.failUnless( neg_dur.days() == -123)
        
        pi_dur = gregorian.date_duration(date_time.pos_infin)
        ni_dur = gregorian.date_duration(date_time.neg_infin)
        nd_dur = gregorian.date_duration(date_time.not_a_date_time)
        self.failUnless( pi_dur + ni_dur == nd_dur)
        self.failUnless( ni_dur / 3 == ni_dur)
        self.failUnless( pi_dur + hundred == pi_dur)
        self.failUnless( -pi_dur == ni_dur)
       

    def test_generators(self):
        pd1 =gregorian.partial_date(1, date_time.Jan)
        d = pd1.get_date(2000)
        self.failUnless( pd1.to_string() == "0" )
        self.failUnless( gregorian.date(2000,1,1) == d)
  
        pd2 = gregorian.partial_date(1, date_time.Feb)

        self.failUnless( pd1 == pd1)
        self.failUnless( not(pd1 == pd2) )
        self.failUnless( not(pd2 == pd1) )
        self.failUnless( not(pd1 < pd1) )
        self.failUnless( pd1 < pd2)
        self.failUnless( not(pd2 < pd1) )
        
        lastkday = gregorian.last_day_of_the_week_in_month
        lsif = lastkday( date_time.Sunday, date_time.Feb)
        self.failUnless( gregorian.date(2002,  date_time.Feb,24) 
                         == lsif.get_date(2002))
        self.failUnless( lsif.to_string() == "M2.5.0" )
  
        firstkday = gregorian.first_day_of_the_week_in_month
        fsif = firstkday(date_time.Sunday, date_time.Feb)
        self.failUnless( gregorian.date(2002, date_time.Feb,3) 
                         == fsif.get_date(2002))
        self.failUnless( fsif.to_string() == "M2.1.0" )
        
        firstkdayafter = gregorian.first_day_of_the_week_after
        fkaf = firstkdayafter(date_time.Monday)
        self.failUnless( gregorian.date(2002, date_time.Feb,4) 
                         == fkaf.get_date( gregorian.date(2002,date_time.Feb,1)))
        self.failUnless( '2002-Feb-04' == to_simple_string( fkaf.get_date( gregorian.date(2002,date_time.Feb,1))))

        firstkdaybefore = gregorian.first_day_of_the_week_before
        fkbf = firstkdaybefore(date_time.Monday)
        self.failUnless( '2002-Feb-11' == to_simple_string(fkaf.get_date(gregorian.date(2002,date_time.Feb,10))))
        self.failUnless( gregorian.date(2002,date_time.Feb,4) 
                         == fkbf.get_date(gregorian.date(2002,date_time.Feb,10)))
        
        nthkdayofmonth = gregorian.nth_day_of_the_week_in_month;
        nkd1 = nthkdayofmonth(nthkdayofmonth.third, date_time.Sunday, date_time.Jul)

        self.failUnless( gregorian.date(1969, date_time.Jul, 20) == nkd1.get_date(1969))
        self.failUnless( nkd1.to_string() == "M7.3.0" )

        saturday = gregorian.date(2003,date_time.Feb,8)
        sund = gregorian.greg_weekday(date_time.Sunday)
        self.failUnless( gregorian.days_until_weekday(saturday, sund) == gregorian.days(1))
        self.failUnless( gregorian.days_before_weekday(saturday, sund) == gregorian.days(6))
        self.failUnless( gregorian.next_weekday(saturday, sund)== gregorian.date(2003,date_time.Feb,9))
        self.failUnless( gregorian.previous_weekday(saturday, sund)== gregorian.date(2003,date_time.Feb,2))

    def test_greg_day(self):
        self.failUnless( gregorian.greg_day(1).as_number() == 1 )
        self.failUnlessRaises( Exception, lambda: gregorian.greg_day(0) )
        self.failUnless( gregorian.greg_day.min() == 1 )
        self.failUnless( gregorian.greg_day.max() == 31 )
        sunday = gregorian.greg_weekday(0)
        monday = gregorian.greg_weekday(1)
        self.failUnless( sunday.as_short_string() == "Sun" )
        self.failUnlessRaises( Exception, lambda: gregorian.greg_day_of_year_rep(367) )

    def test_greg_year(self):
        self.failUnless( gregorian.greg_year(1400).as_number() == 1400 )
        self.failUnlessRaises( Exception, lambda: gregorian.greg_year(0) )
        self.failUnless( gregorian.greg_year.min() == 1400 )
        self.failUnless( gregorian.greg_year.max() == 10000 )
    
    def test_greg_month(self):
        m1 = gregorian.greg_month(date_time.Jan)
        self.failUnless( m1.as_number() == 1)
        self.failUnless( m1.as_short_string() == "Jan")
        self.failUnless( m1.as_long_string() == "January")
        sm1 = gregorian.greg_month(1)
        self.failUnless( sm1.as_number() == 1)
        self.failUnless( gregorian.greg_month.min() == 1 )
        self.failUnless( gregorian.greg_month.max() == 12 )

    def test_greg_cal(self):
        self.failUnless( gregorian.calendar.day_of_week(gregorian.ymd_type(2000,9,24))==0 )
        self.failUnless( gregorian.calendar.is_leap_year(1404) )
        jday1 = gregorian.calendar.day_number( gregorian.ymd_type(2000,1,1))
        jday2 = gregorian.calendar.day_number( gregorian.ymd_type(2001,1,1))
        self.failUnless( jday1 == 2451545 )
        self.failUnless( jday2 == 2451911 )
        ymd = gregorian.calendar.from_day_number(jday1)
        self.failUnless( ymd.year.as_number() == 2000 
                         and ymd.month.as_number() == 1 
                         and ymd.day.as_number() == 1 )
        self.failUnless( gregorian.calendar.end_of_month_day(2000,1) == 31)

    def test_greg_durations(self):
        m1 = gregorian.months(5)
        m2 = gregorian.months(3)
        m3 = gregorian.months(1)
        self.failUnless( gregorian.months(8) == m1 + m2)
        m1 += m2
        self.failUnless( gregorian.months(8) == m1)
        self.failUnless( gregorian.months(-5) == m2 - m1)
        m2 -= m1;
        self.failUnless( gregorian.months(-5) == m2)
      
        d1 = gregorian.date(2005, date_time.Jan, 1)

        self.failUnless( d1 + gregorian.months(-1) 
                         == gregorian.date(2004, date_time.Dec, 1) )
        d3 = gregorian.date(2005, date_time.Dec, 1);
        self.failUnless( d3 - gregorian.months(-1) 
                         == gregorian.date(2006,date_time.Jan,1))
        m1 = gregorian.months(5)
        m2 = gregorian.months(3)
        m3 = gregorian.months(10)
        self.failUnless( gregorian.months(15) == m1 * 3)
        m1 *= 3
        self.failUnless( gregorian.months(15) == m1)
        self.failUnless( gregorian.months(12) == m2 * 4)
        self.failUnless( gregorian.months(3) == m3 / 3)
        m3 /= 3
        self.failUnless( gregorian.months(3) == m3)

        m = gregorian.months(-5)
        m_pos = gregorian.months(date_time.pos_infin)
        m_neg = gregorian.months(date_time.neg_infin)
        m_nadt = gregorian.months(date_time.not_a_date_time)
        self.failUnless( m + m_pos == m_pos)
        self.failUnless( m + m_neg == m_neg)
        self.failUnless( m_pos + m_neg == m_nadt)
        self.failUnless( m_neg + m_neg == m_neg)
        self.failUnless( m - m_pos == m_neg)
        self.failUnless( m - m_neg == m_pos)
        self.failUnless( m_pos - m_neg == m_pos)
        self.failUnless( m_pos * -1 == m_neg)
        self.failUnless( m_pos * 0 == m_nadt)
        self.failUnless( m_neg / 3 == m_neg)
       
        y1 = gregorian.years(2)
        y2 = gregorian.years(4)
        self.failUnless( gregorian.months(27) == m3 + y1)
        m3 += y1
        self.failUnless( gregorian.months(27) == m3)
        self.failUnless( gregorian.months(-21) == m3 - y2)
        m3 -= y2
        self.failUnless( gregorian.months(-21) == m3)

        d = gregorian.date(2001, date_time.Oct, 31);
        self.failUnless( gregorian.date(2002, date_time.Feb, 28) 
                         == d + gregorian.months(4))
        d += gregorian.months(4)
        self.failUnless( gregorian.date(2002, date_time.Feb, 28) == d);
        d = gregorian.date(2001, date_time.Oct, 31);
        self.failUnless( gregorian.date(2001, date_time.Apr, 30) 
                         == d - gregorian.months(6));
        d -= gregorian.months(6);
        self.failUnless( gregorian.date(2001, date_time.Apr, 30) == d);

        y1 = gregorian.years(2)
        y2 = gregorian.years(4)
        y3 = gregorian.years(1)
        self.failUnless( gregorian.years(3) == y3 + y1)
        y3 += y1
        self.failUnless( gregorian.years(3) == y3)
        self.failUnless( gregorian.years(-1) == y3 - y2)
        y3 -= y2
        self.failUnless( gregorian.years(-1) == y3)
       
        y1 = gregorian.years(5)
        y2 = gregorian.years(3)
        y3 = gregorian.years(10)
        
        self.failUnless( gregorian.years(15) == y1 * 3)
        y1 *= 3
        self.failUnless( gregorian.years(15) == y1)

        self.failUnless( gregorian.years(3) == y3 / 3)
        y3 /= 3
        self.failUnless( gregorian.years(3) == y3)

        m = gregorian.years(15)
        y_pos = gregorian.years(date_time.pos_infin)
        y_neg = gregorian.years(date_time.neg_infin)
        y_nadt = gregorian.years(date_time.not_a_date_time)
        self.failUnless( m + y_pos == y_pos)
        self.failUnless( m + y_neg == y_neg)
        self.failUnless( y_pos + y_neg == y_nadt)
        self.failUnless( y_neg + y_neg == y_neg)
        self.failUnless( m - y_pos == y_neg)
        self.failUnless( m - y_neg == y_pos)
        self.failUnless( y_pos - y_neg == y_pos)
        self.failUnless( y_pos * -1 == y_neg)
        self.failUnless( y_pos * 0 == y_nadt)
        self.failUnless( y_neg / 3 == y_neg)

        m1 = gregorian.months(5)
        m2 = gregorian.months(3)
        self.failUnless( gregorian.months(39) == y2 + m2)
        self.failUnless( gregorian.months(31) == y2 - m1)
   
        d = gregorian.date(2001, date_time.Feb, 28)
        self.failUnless( gregorian.date(2004, date_time.Feb, 29) == d + gregorian.years(3))
        d += gregorian.years(3)
        self.failUnless( gregorian.date(2004, date_time.Feb, 29) == d)
         
        d = gregorian.date(2000, date_time.Feb, 29)
        self.failUnless( gregorian.date(1994, date_time.Feb, 28) == d - gregorian.years(6))
        d -= gregorian.years(6)
        self.failUnless( gregorian.date(1994, date_time.Feb, 28) == d)

        w1 = gregorian.weeks(2)
        w2 = gregorian.weeks(4)
        w3 = gregorian.weeks(1)
        pi = gregorian.weeks(date_time.pos_infin)
        self.failUnless( gregorian.weeks(date_time.pos_infin) == w1 + pi)
        self.failUnless( gregorian.weeks(3) == w3 + w1)
        w3 += w1
        self.failUnless( gregorian.weeks(3) == w3)
        self.failUnless( gregorian.weeks(-1) == w3 - w2)
        w3 -= w2
        self.failUnless( gregorian.weeks(-1) == w3)

        d = gregorian.days(10)
        self.failUnless( gregorian.days(31) == d + gregorian.weeks(3))
        d += gregorian.weeks(3);
        self.failUnless( gregorian.days(31) == d)

        d = gregorian.days(10)
        self.failUnless( gregorian.days(-32) == d - gregorian.weeks(6))
        d -= gregorian.weeks(6)
        self.failUnless( gregorian.days(-32) == d)
        
        d = gregorian.date(2001, date_time.Feb, 28)
        self.failUnless( gregorian.date(2001, date_time.Mar, 21) == d + gregorian.weeks(3))
        d += gregorian.weeks(3)
        self.failUnless( gregorian.date(2001, date_time.Mar, 21) == d)
        
        d = gregorian.date(2001, date_time.Feb, 28);
        self.failUnless( gregorian.date(2001, date_time.Jan, 17) == d - gregorian.weeks(6))
        d -= gregorian.weeks(6)
        self.failUnless( gregorian.date(2001, date_time.Jan, 17) == d)

        d = gregorian.date(2000, date_time.Oct, 31)
        d2 = d + gregorian.months(4) + gregorian.years(2)
        d3 = d + gregorian.years(2) + gregorian.months(4)
        self.failUnless( gregorian.date(2003, date_time.Feb,28) == d2)
        self.failUnless( gregorian.date(2003,date_time.Feb,28) == d3)
        m = gregorian.years(2) + gregorian.months(4) - gregorian.months(4) - gregorian.years(2)
        self.failUnless( m.number_of_months() == 0)

    def test_period(self):
        d1 = gregorian.date(2000,date_time.Jan,1)
        d2 = gregorian.date(2000,date_time.Jan,4)
        p1 = gregorian.date_period(d1,d2)
        p2 = gregorian.date_period(d1,gregorian.date_duration(3) )
        self.failUnless( p1 == p2)
        self.failUnless( p1.begin() == d1)
        self.failUnless( p1.last() == d2-gregorian.date_duration(1) )
        self.failUnless( p1.end() == d2)
        self.failUnless( p2.length() == gregorian.date_duration(3) )
        self.failUnless( p1.contains(d1) )
        self.failUnless( not p1.contains(d2) )
        p3 = gregorian.date_period( gregorian.date(2000,date_time.Jan,4)
                                    , gregorian.date(2000,date_time.Feb,1) )
        self.failUnless( not (p1 == p3) )
        self.failUnless( p1 < p3)
        self.failUnless( p3 > p1)
        self.failUnless( p3 != p1)
        self.failUnless( p1.intersects(p1) )
        self.failUnless( not p1.intersects(p3) )
        self.failUnless( not p3.intersects(p1) )
        p4 = gregorian.date_period(gregorian.date(1999,date_time.Dec,1), d2)
        self.failUnless( p1.intersects(p4) )
        self.failUnless( p4.intersects(p1) )
        p5 = gregorian.date_period(gregorian.date(1999,date_time.Dec,1)
                                   , gregorian.date(2000,date_time.Dec,31) )
        self.failUnless( p1.intersects(p5) )
        self.failUnless( p5.intersects(p1) )
        p6 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1)
                                   ,gregorian.date(2000,date_time.Dec,31) )
        self.failUnless( p5.contains(p6) )
        self.failUnless( p6.contains(p6) )
        self.failUnless( not p6.contains(p5) )
        
        fourDays = gregorian.date_duration(4)
        p1.shift(fourDays) 
        shifted = gregorian.date_period(gregorian.date(2000,date_time.Jan,5)
                                        , gregorian.date(2000,date_time.Jan,8) )
        self.failUnless( p1 == shifted)
        
        i1 = gregorian.date_period(gregorian.date(2000,date_time.Jan,5)
                                   , gregorian.date(2000,date_time.Jan,10) )
        i2 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1)
                                   , gregorian.date(2000,date_time.Jan,7) )
        r1 = gregorian.date_period(gregorian.date(2000,date_time.Jan,5)
                                   , gregorian.date(2000,date_time.Jan,7) )
        self.failUnless( i1.intersection(i2) == r1)
        self.failUnless( i2.intersection(i1) == r1)
        
        i3 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1)
                                   , gregorian.date(2000,date_time.Jan,15) )
        self.failUnless( i1.intersection(i3) == i1)
        self.failUnless( i3.intersection(i1) == i1)
        
        i4 = gregorian.date_period(gregorian.date(2000,date_time.Jan,7), gregorian.date(2000,date_time.Jan,10) )
        i5 = gregorian.date_period(gregorian.date(2000,date_time.Jan,6)
                                   , gregorian.date(2000,date_time.Jan,9) )
        self.failUnless( i1.intersection(i5) == i5)
        self.failUnless( i5.intersection(i1) == i5)
        self.failUnless( i2.intersection(i4).is_null() )
        
        r3 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1), gregorian.date(2000,date_time.Jan,10) )
        
        self.failUnless( i1.merge(i2) == r3)
        self.failUnless( i2.merge(i4).is_null() )
        r4 = gregorian.date_period(gregorian.date(2000,date_time.Jan,5), gregorian.date(2000,date_time.Jan,10) )
        self.failUnless( i1.merge(i5) == r4)
        
        self.failUnless( i1.span(i2) == r3)
        self.failUnless( i2.span(i4) == r3)
        self.failUnless( i4.span(i2) == r3)
        self.failUnless( i3.span(i2) == i3)
        
        i6 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1), gregorian.date(2000,date_time.Jan,2) )
        self.failUnless( i6.span(i4) == r3)
        self.failUnless( i4.span(i6) == r3)
        
        bf_start = gregorian.date(2000,date_time.Jan,5)
        bf_end = gregorian.date(2000,date_time.Jan,10)
        bf_before = gregorian.date(2000,date_time.Jan,4) 
        bf_after = gregorian.date(2000,date_time.Jan,11) 
        bf_during = gregorian.date(2000, date_time.Jan, 7)
        bfp1 = gregorian.date_period(bf_start, bf_end) 
        
        self.failUnless( not bfp1.is_before(bf_start) )
        self.failUnless( bfp1.is_before(bf_end) )
        self.failUnless( not bfp1.is_before(bfp1.last() ))
        self.failUnless( not bfp1.is_before(bf_before) )
        self.failUnless( not bfp1.is_before(bf_during) )
        self.failUnless( bfp1.is_before(bf_after) )
        
        self.failUnless( not bfp1.is_after(bf_start) )
        self.failUnless( not bfp1.is_after(bf_end) )
        self.failUnless( not bfp1.is_after(bfp1.last() ))
        self.failUnless( bfp1.is_after(bf_before) )
        self.failUnless( not bfp1.is_after(bf_during) )
        self.failUnless( not bfp1.is_after(bf_after) )
        
        adj1 = gregorian.date_period( gregorian.date(2000,date_time.Jan,5)
                                      , gregorian.date(2000,date_time.Jan,10) )
        adj2 = gregorian.date_period (gregorian.date(2000,date_time.Jan,1), gregorian.date(2000,date_time.Jan,5) )
        adj3 = gregorian.date_period(gregorian.date(2000,date_time.Jan,7), gregorian.date(2000,date_time.Jan,12) )
        adj4 = gregorian.date_period(gregorian.date(2000,date_time.Jan,12), gregorian.date(2000,date_time.Jan,15) )
        adj5 = gregorian.date_period(gregorian.date(2000,date_time.Jan,1), gregorian.date(2000,date_time.Jan,3) )
        adj6 = gregorian.date_period(gregorian.date(2000,date_time.Jan,7), gregorian.date(2000,date_time.Jan,9) )
        
        self.failUnless( adj1.is_adjacent(adj2) )
        self.failUnless( adj2.is_adjacent(adj1) )
        self.failUnless( not adj1.is_adjacent(adj3) )
        self.failUnless( not adj3.is_adjacent(adj1) )
        self.failUnless( not adj1.is_adjacent(adj4) )
        self.failUnless( not adj4.is_adjacent(adj1) )
        self.failUnless( not adj1.is_adjacent(adj5) )
        self.failUnless( not adj5.is_adjacent(adj1) )
        self.failUnless( not adj1.is_adjacent(adj6) )
        self.failUnless( not adj6.is_adjacent(adj1) )

def create_suite():
    suite = unittest.TestSuite()    
    suite.addTest( unittest.makeSuite(tester_t))
    return suite

def run_suite():
    unittest.TextTestRunner(verbosity=2).run( create_suite() )

if __name__ == "__main__":
    run_suite()