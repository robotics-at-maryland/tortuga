// Copyright 2004 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __arrays_bug_to_be_exported_hpp__
#define __arrays_bug_to_be_exported_hpp__

struct arrays_bug{

struct item{ int values[10]; };

struct container{ item items[10]; };

};

#endif//__arrays_bug_to_be_exported_hpp__
