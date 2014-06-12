/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/samples/src/CodeInject.cpp
 */

#include "include/CodeInject.h"

namespace samples {

CodeInject::CodeInject(int value) : m_value(value)
{
}

int CodeInject::getValue()
{
    return m_value;
}

int CodeInject::setValue(int value)
{
    int tmp = m_value;
    m_value = value;
    return tmp;
}

int CodeInject::setValue(std::string str)
{
    int tmp = m_value;
    
    if (str.size() > 0)
        m_value = (int)str[0];

    return tmp;
}

int CodeInject::staticFunc()
{
    return 42;
}
    
} // namespace samples
