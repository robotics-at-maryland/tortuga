// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __unnamed_enums_to_be_exported_hpp__
#define __unnamed_enums_to_be_exported_hpp__

namespace unnamed_enums{ 

enum{ OK=1, CANCEL=0 }; 

struct enum_holder_t{
    enum {ZERO=0, ONE=1, TWO=2 };
};

struct enum_user_t{
  void do_smth1(unsigned param=enum_holder_t::ZERO){};
  void do_smth2(int param=enum_holder_t::ONE){};
  void do_smth3(char param=enum_holder_t::TWO){};
};


}

#endif//__unnamed_enums_to_be_exported_hpp__
