/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/App.h
 */

#ifndef RAM_TOOLS_VISIONVWR_APP_H_01_20_2008
#define RAM_TOOLS_VISIONVWR_APP_H_01_20_2008

#include <wx/app.h>

namespace ram {
namespace tools {
namespace visionvwr {

class App : public wxApp
{
    virtual bool OnInit();
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONVWR_APP_H_01_20_2008
