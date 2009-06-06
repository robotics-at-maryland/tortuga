// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __hierarchy3_to_be_exported_hpp__
#define __hierarchy3_to_be_exported_hpp__

namespace hierarchy3{

struct grandpa_t{
   virtual int gp_pure(int i) = 0;
};

struct father_t : public grandpa_t{
   virtual int gp_pure(int i){ return i; }
};

struct son_t : public father_t
{};

}

#endif//__hierarchy3_to_be_exported_hpp__
