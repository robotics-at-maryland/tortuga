/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/RotationCtrl.h
 */

#ifndef RAM_TOOLS_PLOT_ROTATIONCTRL_H
#define RAM_TOOLS_PLOT_ROTATIONCTRL_H

// STD Includes

// Library Includes
#include "wx/wx.h"

// Project Includes
#include "math/include/Math.h"

using namespace ram;

class RotationCtrl : public wxPanel
{
public:

    RotationCtrl(wxWindow *parent,
                 math::Radian offset = math::Radian(0),
                 int direction = 1);

    ~RotationCtrl() {}
    
    void setOrientation(math::Radian estimatedAngle,
                        math::Radian desiredAngle);
    
    void onPaint(wxPaintEvent& event);

protected:
    DECLARE_EVENT_TABLE()

private:
    
    void draw(wxGraphicsContext& gc);
    void drawText(wxGraphicsContext& gc, wxDC& dc);

    math::Radian m_estimatedAngle;
    math::Radian m_desiredAngle;
    math::Radian m_offset;
    int m_direction;
};

#endif // RAM_TOOLS_PLOT_ROTATIONCTRL_H
