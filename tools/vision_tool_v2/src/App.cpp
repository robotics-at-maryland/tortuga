/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/src/App.cpp
 */

// Project Includes
#include "App.h"
#include "Frame.h"

IMPLEMENT_APP(ram::tools::visiontool::App)

namespace ram {
namespace tools {
namespace visiontool {

bool App::OnInit()
{
    Frame* frame = new Frame(_T("Vision Tool"), wxPoint(50, 50),
                             wxSize(960, 960));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

} // namespace visiontool
} // namespace tools
} // namespace ram
