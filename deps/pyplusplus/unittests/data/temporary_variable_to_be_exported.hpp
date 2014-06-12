// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __temporary_variable_to_be_exported_hpp__
#define __temporary_variable_to_be_exported_hpp__

#include <string>

namespace temporary_variables{ 

struct named_item_t{  
    
    named_item_t() : m_name( "no name" ) {}
        
    virtual const 
    std::string& name() const
    { return m_name; }
    
private:
    std::string m_name;    
};

const std::string& get_name( const named_item_t& ni ){
    return ni.name();
}

struct pure_virtual_t{
    virtual const std::string& name() const = 0;
    virtual std::string& name_ref() = 0;
};


const std::string& get_name( const pure_virtual_t& ni ){
    return ni.name();
}

std::string& get_name_ref( pure_virtual_t& ni ){
    return ni.name_ref();
}


struct virtual_t{

    virtual_t() : m_name( "no name" ){}
    virtual const std::string& name() const { return m_name; }
    virtual std::string& name_ref() { return m_name; }
    
protected:

    virtual const std::string& name_protected() const { return m_name; }

private:
    
    virtual const std::string& name_private() const { return m_name; }

public:

    std::string m_name;
};

struct virtual2_t{
    virtual2_t() : m_name( "no name" ){}
protected:

    virtual const std::string& name_protected_pure() const = 0;
    virtual const std::string& name_protected() const { return m_name; }

private:
    
    virtual const std::string& name_private_pure() const = 0;
    virtual const std::string& name_private() const { return m_name; }

public:

    std::string m_name;
};

const std::string& get_name( const virtual_t& ni ){
    return ni.name();
}

std::string& get_name_ref( virtual_t& ni ){
    return ni.name_ref();
}

}

#endif//__temporary_variable_to_be_exported_hpp__
