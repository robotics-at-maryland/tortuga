/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/pattern/test/include/TestMaker.h
 */

#ifndef RAM_PATTERN_TESTMAKER_H_08_10_2007
#define RAM_PATTERN_TESTMAKER_H_08_10_2007

// STD Includes
#include <sstream>
#include <iostream>
#include <string>

// A simple example set of class we wish to make dynamically
class Number
{
public:
    virtual ~Number() {};
};
    
class Int : public Number
{
public:
    Int(int _val) : value(_val) {};
    
    int value;
};

class Double : public Number
{
public:
    Double(double _val) : value(_val) {};
    
    double value;
};

// A 
struct StreamKeyExtractor
{
    static std::string extractKey(std::iostream& param)
    {
        std::string key;
        param >> key;
        return key;
    }
};
 
typedef Maker<Number*,            // The type of object created by the maker
              std::iostream&,     // The parameter used to create the object
              std::string,        // The type of key used to register makers
              StreamKeyExtractor> // Gets the key from the paramters
NumberMaker;


class IntMaker : public NumberMaker
{
public:
    IntMaker() : NumberMaker("Int") {};
    
    virtual Number* makeObject(std::iostream& param)
    {
        int value;
        param >> value;
        
        return new Int(value);
    }
private:
    static IntMaker registerThis;
};

class DoubleMaker : public NumberMaker
{
public:
    DoubleMaker() : NumberMaker("Double") {};
    
    virtual Number* makeObject(std::iostream& param)
    {
        double value;
        param >> value;
        
        return new Double(value);
    }
private:
    static DoubleMaker registerThis;
};

#endif // RAM_PATTERN_TESTMAKER_H_08_10_2007
