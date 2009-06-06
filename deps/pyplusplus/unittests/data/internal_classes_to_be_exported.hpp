// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __internal_classes_to_be_exported_hpp__
#define __internal_classes_to_be_exported_hpp__

namespace internal_classes{

struct external_t{
    struct internal_t{
        virtual int do_nothing() const { return 1; }
    };
};
    
}//internal_classes

#endif//__internal_classes_to_be_exported_hpp__

