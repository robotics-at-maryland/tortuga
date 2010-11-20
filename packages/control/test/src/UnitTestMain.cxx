/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/UnitTestMain.cxx
 */

// STD Includes
#include <fstream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/detail/MathChecksImp.h"

int main(int argc, char* argv[])
{
    int returnCode = UnitTest::RunAllTests();
    if (returnCode == 0) {
        /* Write blank success file to disk */
        std::string filename(argv[0]);
        filename += ".success";
        std::ofstream outfile (filename.c_str());
        outfile.close();
    }
    return returnCode;
}
