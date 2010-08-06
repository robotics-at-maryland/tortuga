/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/include/Exception.h
 */

#ifndef RAM_CORE_EXCEPTION_01_21_2010
#define RAM_CORE_EXCEPTION_01_21_2010

// STD Includes
#include <string>
#include <exception>

namespace ram {
namespace core {

class MakerNotFoundException : public std::exception
{
public:
    MakerNotFoundException(std::string key) :
        m_key(key)
    {
    }

    virtual ~MakerNotFoundException() throw ()
    {
    }

    virtual const char* what() const throw()
    {
        return m_key.c_str();
    }

private:
    std::string m_key;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_EXCEPTION_01_21_2010
