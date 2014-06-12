// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __already_exposed_to_be_exported_hpp__
#define __already_exposed_to_be_exported_hpp__

#include <vector>
#include <string>

namespace already_exposed{

struct ae_t{};

enum ae_e { x1, x2 };

struct ae_base{};

void do_smth( const std::vector< std::string >& );
    
}
    
#endif//__already_exposed_to_be_exported_hpp__
