#! /usr/bin/python
# Copyright 2004-2008 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import _date_time_

#import pprint
#pprint.pprint( dir( _date_time_ ) )

#implementation details
#int_adapter_int = _date_time_.int_adapter_int
#int_adapter_long = _date_time_.int_adapter_long
#int_adapter_ulong = _date_time_.int_adapter_ulong

next_weekday = _date_time_.next_weekday
to_simple_string_type = _date_time_.to_simple_string_type
from_simple_string = _date_time_.from_simple_string
from_undelimited_string = _date_time_.from_undelimited_string
date_period_from_wstring = _date_time_.date_period_from_wstring
#to_sql_string_type_w = _date_time_.to_sql_string_type_w
parse_undelimited_time_duration = _date_time_.parse_undelimited_time_duration
#to_iso_extended_string_type_w = _date_time_.to_iso_extended_string_type_w
previous_weekday = _date_time_.previous_weekday
to_sql_wstring = _date_time_.to_sql_wstring
date_from_tm = _date_time_.date_from_tm
parse_undelimited_date = _date_time_.parse_undelimited_date
nth_as_str = _date_time_.nth_as_str
date_period_from_string = _date_time_.date_period_from_string
to_sql_string_type = _date_time_.to_sql_string_type
to_simple_wstring = _date_time_.to_simple_wstring
split = _date_time_.split
from_uk_string = _date_time_.from_uk_string
duration_from_string = _date_time_.duration_from_string
from_string = _date_time_.from_string
to_iso_extended_wstring = _date_time_.to_iso_extended_wstring
days_until_weekday = _date_time_.days_until_weekday
to_iso_string = _date_time_.to_iso_string
parse_iso_time = _date_time_.parse_iso_time
power = _date_time_.power
special_value_from_string = _date_time_.special_value_from_string
#to_iso_string_type_w = _date_time_.to_iso_string_type_w
time_from_string = _date_time_.time_from_string
date_from_iso_string = _date_time_.date_from_iso_string
parse_delimited_time_duration = _date_time_.parse_delimited_time_duration
to_iso_extended_string = _date_time_.to_iso_extended_string
from_iso_string = _date_time_.from_iso_string
from_simple_string_type = _date_time_.from_simple_string_type
to_tm = _date_time_.to_tm
absolute_value = _date_time_.absolute_value
to_simple_string = _date_time_.to_simple_string
convert_to_lower = _date_time_.convert_to_lower
days_before_weekday = _date_time_.days_before_weekday
to_iso_wstring = _date_time_.to_iso_wstring
#to_simple_string_type_w = _date_time_.to_simple_string_type_w
to_iso_string_type = _date_time_.to_iso_string_type
to_iso_extended_string_type = _date_time_.to_iso_extended_string_type
parse_delimited_time = _date_time_.parse_delimited_time
to_sql_string = _date_time_.to_sql_string
from_us_string = _date_time_.from_us_string












# ##############################################################################
#
# Creating shortcuts for enums. I using this approach because is allows to use
# "code complition" futures on most editors
#
# ##############################################################################

months_of_year = _date_time_.months_of_year
Mar = months_of_year.Mar
Feb = months_of_year.Feb
Aug = months_of_year.Aug
Sep = months_of_year.Sep
Apr = months_of_year.Apr
Jun = months_of_year.Jun
Jul = months_of_year.Jul
Jan = months_of_year.Jan
May = months_of_year.May
Nov = _date_time_.months_of_year.Nov
Dec = _date_time_.months_of_year.Dec
Oct = _date_time_.months_of_year.Oct
NotAMonth = months_of_year.NotAMonth
NumMonths = months_of_year.NumMonths

time_is_dst_result = _date_time_.time_is_dst_result
ambiguous = time_is_dst_result.ambiguous
is_not_in_dst = time_is_dst_result.is_not_in_dst
invalid_time_label = time_is_dst_result.invalid_time_label
is_in_dst = time_is_dst_result.is_in_dst

date_resolutions = _date_time_.date_resolutions
week = date_resolutions.week
century = date_resolutions.century
months = date_resolutions.months
year = date_resolutions.year
day = date_resolutions.day
decade = date_resolutions.decade
NumDateResolutions = date_resolutions.NumDateResolutions

weekdays = _date_time_.weekdays
Monday = weekdays.Monday
Tuesday = weekdays.Tuesday
Friday = weekdays.Friday
Wednesday = weekdays.Wednesday
Thursday = weekdays.Thursday
Sunday = weekdays.Sunday
Saturday = weekdays.Saturday

special_values = _date_time_.special_values 
not_special = special_values.not_special
neg_infin = special_values.neg_infin
pos_infin = special_values.pos_infin
min_date_time = special_values.min_date_time
not_a_date_time = special_values.not_a_date_time
max_date_time = special_values.max_date_time
NumSpecialValues = special_values.NumSpecialValues

dst_flags = _date_time_.dst_flags
not_dst = dst_flags.not_dst
is_dst = dst_flags.is_dst
calculate = dst_flags.calculate

time_resolutions = _date_time_.time_resolutions
ten_thousandth = time_resolutions.ten_thousandth
hundreth = time_resolutions.hundreth
nano = time_resolutions.nano
NumResolutions = time_resolutions.NumResolutions
micro = time_resolutions.micro
sec = time_resolutions.sec
milli = time_resolutions.milli
tenth = time_resolutions.tenth

ymd_order_spec = _date_time_.ymd_order_spec
ymd_order_dmy = ymd_order_spec.ymd_order_dmy
ymd_order_us = ymd_order_spec.ymd_order_us
ymd_order_iso = ymd_order_spec.ymd_order_iso

month_format_spec = _date_time_.month_format_spec
month_as_short_string = month_format_spec.month_as_short_string
month_as_long_string = month_format_spec.month_as_long_string
month_as_integer = month_format_spec.month_as_integer


class gregorian:
    date_period = _date_time_.date_period
    year_based_generator = _date_time_.gregorian_year_based_generator
    partial_date = _date_time_.partial_date
    nth_kday_of_month = _date_time_.nth_kday_of_month    
    nth_day_of_the_week_in_month = _date_time_.nth_kday_of_month
    first_kday_of_month = _date_time_.first_kday_of_month
    first_day_of_the_week_in_month = _date_time_.first_kday_of_month
    last_kday_of_month = _date_time_.last_kday_of_month
    last_day_of_the_week_in_month = _date_time_.last_kday_of_month
    first_kday_after = _date_time_.first_kday_after
    first_day_of_the_week_after = _date_time_.first_kday_after 
    first_kday_before = _date_time_.first_kday_before
    first_day_of_the_week_before = _date_time_.first_kday_before
    day_clock = _date_time_.day_clock
    date = _date_time_.date
    date_duration = _date_time_.date_duration
    days = _date_time_.date_duration
    greg_day = _date_time_.greg_day
    greg_month = _date_time_.greg_month
    greg_weekday = _date_time_.greg_weekday
    greg_year = _date_time_.greg_year
    gregorian_calendar = _date_time_.gregorian_calendar
    calendar = _date_time_.gregorian_calendar
    year_month_day = _date_time_.year_month_day
    ymd_type = _date_time_.year_month_day
    months = _date_time_.months
    years = _date_time_.years
    weeks = _date_time_.weeks
    days_until_weekday = staticmethod( _date_time_.days_until_weekday )
    days_before_weekday = staticmethod( _date_time_.days_before_weekday )
    next_weekday = staticmethod( _date_time_.next_weekday )
    previous_weekday = staticmethod( _date_time_.previous_weekday )
    day_of_year_type = _date_time_.day_of_year_type
    greg_day_of_year_rep = _date_time_.day_of_year_type
    greg_weekday_rep = _date_time_.greg_weekday_rep
    greg_day_rep = _date_time_.greg_day_rep
    greg_month_rep = _date_time_.greg_month_rep
    greg_year_rep = _date_time_.greg_year_rep
    
class posix_time:
    ptime = _date_time_.ptime
    time_duration = _date_time_.time_duration
    time_duration_impl = _date_time_.time_duration_impl
    time_period = _date_time_.time_period
    hours = _date_time_.hours
    minutes = _date_time_.minutes
    seconds = _date_time_.seconds
    millisec = _date_time_.milliseconds
    milliseconds = _date_time_.milliseconds
    microsec = _date_time_.microseconds
    microseconds = _date_time_.microseconds
    if hasattr( _date_time_, 'nanoseconds' ):
        nanosec = _date_time_.nanoseconds
        nanoseconds = _date_time_.nanoseconds
    second_clock = _date_time_.second_clock
    microsec_clock = _date_time_.microsec_clock
    us_dst = _date_time_.us_dst
    no_dst = _date_time_.us_dst

class local_time:
    dst_calc_rule = _date_time_.dst_calc_rule
    #local_date_time = _date_time_.local_date_time
    custom_time_zone = _date_time_.custom_time_zone
    posix_time_zone = _date_time_.posix_time_zone
    time_zone_base = _date_time_.time_zone_base
    time_zone = _date_time_.custom_time_zone
    tz_database = _date_time_.tz_database
    time_zone_names = _date_time_.time_zone_names
    time_zone_database = _date_time_.tz_database
    partial_date_dst_rule = _date_time_.partial_date_dst_rule
    partial_date_dst_rule.date_type = gregorian.date
    partial_date_dst_rule.start_rule = gregorian.partial_date
    partial_date_dst_rule.end_rule = gregorian.partial_date
    dst_calc_rule = _date_time_.dst_calc_rule
    first_last_dst_rule = _date_time_.first_last_dst_rule
    first_last_dst_rule.date_type = gregorian.date
    first_last_dst_rule.start_rule = gregorian.first_kday_of_month
    first_last_dst_rule.end_rule = gregorian.last_kday_of_month    
    last_last_dst_rule = _date_time_.last_last_dst_rule
    last_last_dst_rule.date_type = gregorian.date
    last_last_dst_rule.start_rule = gregorian.last_kday_of_month
    last_last_dst_rule.end_rule = gregorian.last_kday_of_month
    nth_last_dst_rule = _date_time_.nth_last_dst_rule
    nth_last_dst_rule.date_type = gregorian.date
    nth_last_dst_rule.start_rule = gregorian.nth_kday_of_month
    nth_last_dst_rule.end_rule = gregorian.last_kday_of_month
    nth_kday_dst_rule = _date_time_.nth_kday_dst_rule
    nth_kday_dst_rule.date_type = gregorian.date
    nth_kday_dst_rule.start_rule = gregorian.nth_kday_of_month
    nth_kday_dst_rule.end_rule = gregorian.nth_kday_of_month
    nth_day_of_the_week_in_month_dst_rule = nth_kday_dst_rule
    local_time_period = _date_time_.local_time_period
    class local_sec_clock:
        pass
    #local_sec_clock = _date_time_.local_sec_clock
    local_sec_clock.local_time = staticmethod( _date_time_.local_sec_clock__local_time )
    class local_microsec_clock:
        pass    
    #local_microsec_clock = _date_time_.local_microsec_clock
    local_microsec_clock.local_time = staticmethod( _date_time_.local_microsec_clock__local_time )
    dst_adjustment_offsets = _date_time_.dst_adjustment_offsets