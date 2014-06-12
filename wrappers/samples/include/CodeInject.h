/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/samples/include/CodeInject.h
 */

#ifndef RAM_SAMPLES_CODEINJECT_H
#define RAM_SAMPLES_CODEINJECT_H

// STD Includes
#include <string>

namespace samples {

class CodeInject
{
public:
    CodeInject(int value);

    int getValue();

    int setValue(int value);

    int setValue(std::string str);

    static int staticFunc();        
    
private:
    int m_value;
};

} // namespace samples

#endif // RAM_SAMPLES_CODEINJECT_H
