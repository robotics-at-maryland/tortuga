// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __abstract_to_be_exported_hpp__
#define __abstract_to_be_exported_hpp__

namespace abstract {

enum colors{ red, green, blue};

class shape{
public:
    shape(){}
    virtual ~shape() {}
public:    
    virtual colors color() const = 0;
};

class square : public shape{
public:

    virtual ~square(){}
    
public:
    virtual colors color() const 
    { return red; }
    
};

colors call(shape *s){ 
    return s->color(); 
}
  
}

#endif//__abstract_to_be_exported_hpp__
