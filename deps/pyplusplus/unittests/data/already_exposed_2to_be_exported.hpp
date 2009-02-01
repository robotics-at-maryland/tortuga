// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __already_exposed_2to_be_exported_hpp__
#define __already_exposed_2to_be_exported_hpp__

#include "already_exposed_to_be_exported.hpp"
#include <vector>
#include <string>

namespace to_be_exposed{

struct ae_derived : public already_exposed::ae_base
{};

inline std::vector< std::string > do_nothing(){
    return std::vector< std::string >();
}

}

#endif//__already_exposed_2to_be_exported_hpp__
