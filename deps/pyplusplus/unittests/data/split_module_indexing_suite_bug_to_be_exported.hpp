// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __split_module_indexing_suite_bug_to_be_exported_hpp__
#define __split_module_indexing_suite_bug_to_be_exported_hpp__

#include <string>
#include <boost/any.hpp>
#include <map>
#include <vector>


struct generic_string_map_t
{
public:
    generic_string_map_t() {};

    std::vector<std::string> get_value(std::string key) const{
        return std::vector<std::string>();
    }

private:
    std::map<std::string, boost::any> m_fields;

};


#endif//__split_module_indexing_suite_bug_to_be_exported_hpp__
