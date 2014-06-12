#ifndef __date_time__pypp__hpp_
#define __date_time__pypp__hpp_

#include "boost/date_time/special_defs.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "date_time_wrapper.hpp"

namespace details{

inline void export_templates(){
    sizeof( boost::posix_time::time_period );
    sizeof( boost::gregorian::date_period );
    sizeof( boost::gregorian::weeks );
    sizeof( boost::posix_time::milliseconds );
    sizeof( boost::posix_time::microseconds );
    //In order to get nanoseconds boost::date_time should be compiled with BOOST_DATE_TIME_HAS_NANOSECONDS
    //sizeof( boost::posix_time::nanoseconds );
    sizeof( boost::gregorian::day_clock );
    sizeof( boost::gregorian::gregorian_calendar::day_of_year_type(1) ); 
    sizeof( boost::gregorian::greg_weekday_rep(1) ); 
    sizeof( boost::gregorian::greg_day_rep(1) ); 
    sizeof( boost::gregorian::greg_month_rep(1) ); 
    sizeof( boost::gregorian::greg_year_rep(1) ); 
    sizeof( boost::date_time::year_based_generator<boost::gregorian::date> );
    sizeof( boost::date_time::partial_date<boost::gregorian::date> );
    sizeof( boost::date_time::nth_kday_of_month<boost::gregorian::date> );
    sizeof( boost::date_time::first_kday_of_month<boost::gregorian::date> );
    sizeof( boost::date_time::last_kday_of_month<boost::gregorian::date> );
    sizeof( boost::date_time::first_kday_after<boost::gregorian::date> );
    sizeof( boost::date_time::first_kday_before<boost::gregorian::date> );
    sizeof( boost::date_time::time_zone_names_base<char> );
    
    //exporting 4 functions
    boost::gregorian::date sunday(2003, boost::date_time::Feb,2);
    boost::gregorian::greg_weekday sat(boost::date_time::Saturday);
    boost::gregorian::days_until_weekday(sunday, sat);
    boost::gregorian::days_before_weekday(sunday, sat);
    boost::gregorian::next_weekday(sunday, sat);
    boost::gregorian::previous_weekday(sunday, sat);
    //exporting static function
    boost::date_time::int_adapter<int>::from_special( boost::date_time::not_a_date_time );
    boost::date_time::int_adapter<long int>::from_special( boost::date_time::not_a_date_time );
    boost::date_time::int_adapter<long unsigned int>::from_special( boost::date_time::not_a_date_time );
    
    sizeof( boost::local_time::first_last_dst_rule );
    sizeof( boost::local_time::last_last_dst_rule );
    sizeof( boost::local_time::nth_day_of_the_week_in_month_dst_rule );
    sizeof( boost::local_time::posix_time_zone );
    sizeof( boost::local_time::tz_database );
    sizeof( boost::local_time::partial_date_dst_rule );
    sizeof( boost::local_time::nth_last_dst_rule );
    sizeof( boost::local_time::nth_day_of_the_week_in_month_dst_rule );
    sizeof( boost::local_time::local_time_period );
    sizeof( boost::local_time::local_sec_clock );
    sizeof( boost::local_time::local_microsec_clock );

    sizeof( boost::posix_time::second_clock );
    sizeof( boost::posix_time::microsec_clock );
    sizeof( boost::posix_time::no_dst );
    sizeof( boost::posix_time::us_dst );

}

}

#endif//__date_time__pypp__hpp_