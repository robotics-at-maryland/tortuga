import os
from pyboost import date_time
from pyboost.date_time import gregorian 
from pyboost.date_time import posix_time
from pyboost.date_time import local_time
from pyboost.date_time import to_simple_string

#Date programming
weekstart = gregorian.date(2002, date_time.Feb,1)
print 'weekstart: ', to_simple_string( weekstart )

weekend  = weekstart + gregorian.weeks(1)
print 'weekend: ', to_simple_string( weekend )

today = gregorian.day_clock.local_day()
d2 = weekstart + gregorian.days(5)
if d2 >= today: #comparison operator
    pass

thisWeek = gregorian.date_period(today,d2)
if thisWeek.contains(today):
    pass

#date generator functions 
d5 = gregorian.next_weekday(today, date_time.Sunday); #calculate Sunday following d4
print 'd5: ', to_simple_string( d5 )

#US labor day is first Monday in Sept 
first = gregorian.nth_day_of_the_week_in_month.first
labor_day = gregorian.nth_day_of_the_week_in_month(first, date_time.Monday, date_time.Sep)
#calculate a specific date for 2004 from functor 
print 'labor day 2004: ', to_simple_string( labor_day.get_date(2004) )

#Time programming:
d = gregorian.date(2002,date_time.Feb,1)#an arbitrary date 
t1 = posix_time.ptime(d, posix_time.hours(5) + posix_time.millisec(100)); #date + time of day offset 
print 't1: ', to_simple_string( t1 )

t2 = t1 - posix_time.minutes(4) + posix_time.seconds(2)
print 't2: ', to_simple_string( t2 )

now = posix_time.second_clock.local_time(); #use the clock 
print 'now: ', to_simple_string( now )
today = now.date() #Get the date part out of the time 
print 'today: ', to_simple_string( today )
tomorrow = today + gregorian.date_duration(1)
print 'tomorrow: ', to_simple_string( tomorrow )

#Local time programming:
#setup some timezones for creating and adjusting times
#first time zone uses the time zone file for regional timezone definitions
tz_db = local_time.time_zone_database()
tz_db.load_from_file( os.path.join( date_time.__path__[0], "date_time_zonespec.csv") )
nyc_tz = tz_db.time_zone_from_region("America/New_York")
#This timezone uses a posix time zone string definition to create a time zone
phx_tz = local_time.posix_time_zone("MST-07:00:00")

#local departure time in phoenix is 11 pm on April 2 2005 
#Note that New York changes to daylight savings on Apr 3 at 2 am)
phx_departure = local_time.local_date_time( 
    gregorian.date(2005, date_time.Apr, 2)
    , posix_time.hours(23)
    , phx_tz
    , local_time.local_date_time.NOT_DATE_TIME_ON_ERROR)

flight_length = posix_time.hours(4) + posix_time.minutes(30)
phx_arrival = phx_departure + flight_length
#//convert the phx time to a nyz time
nyc_arrival = phx_arrival.local_time_in(nyc_tz, posix_time.time_duration() )
print "New York arrival: ", nyc_arrival.to_string() #//2005-Apr-03 06:30:00 EDT
