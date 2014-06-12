#ifndef __vector_with_shared_data_hpp__
#define __vector_with_shared_data_hpp__

#include <vector>
#include <boost/shared_ptr.hpp>

namespace samples{

class data{};

std::vector< boost::shared_ptr<data> > do_smth();

} 

namespace pyplusplus{ namespace aliases{
    typedef std::vector< boost::shared_ptr<samples::data> > shared_data_container_t;
} }

#endif//__vector_with_shared_data_hpp__
