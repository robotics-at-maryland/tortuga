// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __regression_1_to_be_exported_hpp__
#define __regression_1_to_be_exported_hpp__

namespace regression_1{

struct data{
    int a;
};

class updater_i{
public:
    virtual void update_by_ref( data& d ) = 0;
    virtual void update_by_ptr( data* d ) = 0;
};

class some_updater : public updater_i{
public:
    virtual void update_by_ref( data& d )
    { d.a = 10; }
    
    virtual void update_by_ptr( data* d ){ 
        if( d ){
            d->a = 101; 
        }
    }

};

class updater_holder{
public:

    void set_updater( updater_i * new_updater )
    { m_updater = new_updater; }

    void update_by_ref()
    { m_updater->update_by_ref( m_data ); }

    void update_by_ptr()
    { m_updater->update_by_ptr( &m_data ); }

    const data& get_data() const
    { return m_data; }

private:
    data m_data;
    updater_i* m_updater;
};

}

#endif//__regression_1_to_be_exported_hpp__

