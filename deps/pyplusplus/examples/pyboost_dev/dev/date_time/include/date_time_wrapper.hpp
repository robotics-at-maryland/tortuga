#ifndef __date_time_wrappers_hpp__
#define __date_time_wrappers_hpp__
#include "boost/date_time/local_time/local_time.hpp"

namespace pyplusplus{

inline boost::local_time::local_date_time
local_microsec_clock__local_time( boost::shared_ptr< boost::local_time::time_zone > tz ){
    return boost::local_time::local_microsec_clock::local_time(tz);
}

inline boost::local_time::local_date_time
local_sec_clock__local_time( boost::shared_ptr< boost::local_time::time_zone > tz ){
    return boost::local_time::local_sec_clock::local_time(tz);
}

}

#endif//__date_time_wrappers_hpp__
