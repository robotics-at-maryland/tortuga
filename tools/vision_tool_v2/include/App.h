/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/App.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_APP_H
#define RAM_TOOLS_VISIONTOOL_APP_H

#include <wx/app.h>

namespace ram {
namespace tools {
namespace visiontool {

class App : public wxApp
{
    virtual bool OnInit();
};

} // namespace visiontool
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONTOOL_APP_H_01_20_2008
