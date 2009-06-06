// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __regression_3_to_be_exported_hpp__
#define __regression_3_to_be_exported_hpp__

namespace regression_3{

class base{
public:
    base(){}
    virtual ~base(){}
    virtual int get_value() const = 0;
private:
    base( const base& );
    base& operator=( const base& );
};
    
class middle : public base{
public:
    virtual void set_value(int) = 0;
};

class final : public middle{
public:
    virtual int get_value() const{ return m_value; } 
    virtual void set_value(int v){ m_value = v; }
private:
    int m_value;
};

inline int get_value( const base& b ){ return b.get_value(); }

inline void set_value( middle& m, int value ){ m.set_value( value ); }

}

#endif//__regression_3_to_be_exported_hpp__

