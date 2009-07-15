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

IMPLEMENT_APP(ram::tools::visionvwr::App)

namespace ram {
namespace tools {
namespace visionvwr {

bool App::OnInit()
{
    Frame* frame = new Frame(_T("Vision Tool"), wxPoint(50, 50),
                             wxSize(640, 480));
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

} // namespace visionvwr
} // namespace tools
} // namespace ram
