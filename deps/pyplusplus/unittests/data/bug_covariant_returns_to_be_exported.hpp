// Copyright 2004-2008 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __bug_virtual_functions_overload_to_be_exported_hpp__
#define __bug_virtual_functions_overload_to_be_exported_hpp__

struct data_t {
    int i;
};

struct more_data_t : public data_t{
};

struct algorithm_t{
    algorithm_t(){};
    virtual data_t* f(){ 
        data_t* d = new data_t();
        d->i = 0;
        return d;
    }
};

class better_algorithm_t : public algorithm_t{
public:
    better_algorithm_t(){};
    virtual more_data_t* f(){ 
        more_data_t* d = new more_data_t();
        d->i = 1;
        return d;
    }

};


struct FObjectState {};
    
struct DObjectState;


struct FSerializable {
public:
    virtual FObjectState* saveState() const = 0;
};

struct DSerializable {
public:
    virtual DObjectState* saveState() const = 0;
};


class FMachinePart : public FSerializable {
};

class DMachinePart : public DSerializable {
};


class FComponent : public FMachinePart {
public:
    // methods inherited from Serializable interface
    virtual FObjectState* saveState() const{ return 0; }
};


class DComponent : public DMachinePart {
public:
    // methods inherited from Serializable interface
    virtual DObjectState* saveState() const{ return 0; }
};

#endif//__bug_virtual_functions_overload_to_be_exported_hpp__
