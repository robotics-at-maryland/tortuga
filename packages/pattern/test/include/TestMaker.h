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

// Project Includes
#include "pattern/include/Maker.h"

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

// Extracts the key from the iostream
struct StreamKeyExtractor
{
    static std::string extractKey(std::iostream& param)
    {
        std::string key;
        param >> key;
        return key;
    }
};

// A more verbose but simpler approach to using the pattern

typedef ram::pattern::Maker<Number*, // The type of object created by the maker
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


// A More complex but less typing intensive method, this also lessens the
// amount a user of the system needs to know about its workings

template<class PrimType, class NumType>
struct NumberMakerTemplate : public NumberMaker
{
    NumberMakerTemplate(std::string makerName) : NumberMaker(makerName) {};
    
    virtual Number* makeObject(std::iostream& param)
    {
        PrimType number;
        param >> number;
        return new NumType(number);
    }
};

class DoubleMakerVer2 : public NumberMakerTemplate<double, Double>
{
    static DoubleMakerVer2 registerThis;
    DoubleMakerVer2() : NumberMakerTemplate<double, Double>("DoubleVer2") {};
};

class IntMakerVer2 : public NumberMakerTemplate<int, Int>
{
    static IntMakerVer2 registerThis;
    IntMakerVer2() : NumberMakerTemplate<int, Int>("IntVer2") {};
};

#endif // RAM_PATTERN_TESTMAKER_H_08_10_2007
