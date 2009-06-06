#include "vector_with_shared_data_to_be_exported.hpp"

namespace samples{

std::vector< boost::shared_ptr<data> > do_smth(){
    std::vector< boost::shared_ptr<data> > result;
    result.push_back( boost::shared_ptr<data>( new data() ) );
    result.push_back( boost::shared_ptr<data>( new data() ) );
    return result;
}

} 
