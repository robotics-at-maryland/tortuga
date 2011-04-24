/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/RotationCtrl.h
 */

// STD Includes
#include <iostream>
#include <sstream>

// Library Includes
#include "wx/wx.h"

// Project Includes
#include "../include/RotationCtrl.h"
#include "math/include/Math.h"

using namespace ram;
using namespace std;

RotationCtrl::RotationCtrl(wxWindow *parent, math::Radian offset, int direction) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, 
            wxDefaultSize, wxTAB_TRAVERSAL,
            wxT("RotationCtrl")),
    m_estimatedAngle(math::Radian(0)),
    m_desiredAngle(math::Radian(0)),
    m_offset(offset),
    m_direction(direction)
{
}


void RotationCtrl::setOrientation(math::Radian estimatedAngle,
                                  math::Radian desiredAngle)
{
    m_estimatedAngle = estimatedAngle;
    m_desiredAngle = desiredAngle;

    Refresh();
}

void RotationCtrl::onPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    gc->PushState();
    draw(*gc);
    gc->PopState();

    gc->PushState();
    drawText(*gc, dc);
    gc->PopState();
}

void RotationCtrl::draw(wxGraphicsContext& gc)
{
    int penSize = 3;
    wxColour penColour = wxColour(wxT("BLACK"));
    wxColour brushColour = wxColour(0,0,0,98);
    gc.SetPen(wxPen(penColour, penSize));
    gc.SetBrush(wxBrush(brushColour));

    // radius should be half the smallest dimension
    int radius = 0;

    int width, height;
    GetSize(&width, &height);

    if(width < height)
        radius = width / 2;
    else
        radius = height / 2;
    radius -= penSize;

    // calculate positions of the triangle corners
    int xCoord = 0.573576436 * radius; // cos(55) * r
    int yCoord = 0.819152044 * radius; // sin(55) * r

    wxPoint p1 = wxPoint(-xCoord, yCoord);
    wxPoint p2 = wxPoint(xCoord, yCoord);
    wxPoint p3 = wxPoint(0, -radius);

    // form the triangle outline
    wxGraphicsPath triangle = gc.CreatePath();
    triangle.MoveToPoint(p1);
    triangle.AddLineToPoint(p2);
    triangle.AddLineToPoint(p3);
    triangle.AddLineToPoint(p1);
    triangle.CloseSubpath();

    // shift the gc center
    int xShift = width / 2;
    int yShift = height / 2;
    gc.Translate(xShift, yShift);
    
    // draw the outer circle
    wxGraphicsPath circle = gc.CreatePath();
    circle.AddCircle(0,0,radius);
    gc.StrokePath(circle);

    // determine the rotation to apply to the triangle
    math::Radian estimatedAngle = (m_estimatedAngle + m_offset) * m_direction;
    math::Radian desiredAngle = (m_desiredAngle + m_offset) * m_direction;

    // finally, draw the triangles
    wxPen outlinePen = wxPen(wxColour(wxT("BLACK")), 3);
    gc.SetPen(outlinePen);

    wxColour desiredFillColour = wxColour(0, 255, 0, 90); // translucent green
    wxColour estimatedFillColour = wxColour(0, 0, 0, 90); // translucent black

    wxBrush desiredBrush = wxBrush(desiredFillColour);
    wxBrush estimatedBrush = wxBrush(estimatedFillColour);

    gc.SetBrush(desiredBrush);
    gc.Rotate(desiredAngle.valueRadians());
    gc.DrawPath(triangle);
    gc.Rotate(-desiredAngle.valueRadians());

    gc.SetBrush(estimatedBrush);
    gc.Rotate(estimatedAngle.valueRadians());
    gc.DrawPath(triangle);
    gc.Rotate(-estimatedAngle.valueRadians());
}

void RotationCtrl::drawText(wxGraphicsContext& gc, wxDC& dc)
{
    // set the font
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    font.SetPointSize(1.5 * font.GetPointSize());
    gc.SetFont(font, wxColour(wxT("BLACK")));

    wxString defaultText = wxT("-000.0");
    int textAreaWidth = 0, textAreaHeight = 0;
    GetTextExtent(defaultText, &textAreaWidth, &textAreaHeight);

    // calculate the text box size
    int rectHeight = textAreaHeight * 1.25;
    int rectWidth = textAreaWidth * 1.5;

    int width, height;
    GetSize(&width, &height); 

    int topLeftX = (width - rectWidth) / 2;
    int topLeftY = (height - rectHeight) / 2;

    // draw the text box
    wxColour textColour = wxColour(wxT("BLACK"));
    wxColour fillColour = wxColour(wxT("WHITE"));

    gc.SetPen(wxPen(textColour, 3));
    gc.SetBrush(fillColour);
    gc.DrawRectangle(topLeftX, topLeftY, rectWidth, rectHeight);

    // draw the text
    stringstream ss (stringstream::out);
    ss.precision(1);
    ss.setf(ios::fixed, ios::floatfield);
    ss << m_estimatedAngle.valueDegrees();
    std::string angleStr = ss.str();
    wxString wAngleStr(angleStr.c_str(), wxConvUTF8);
    
    int textWidth = 0, textHeight = 0;
    GetTextExtent(wAngleStr, &textWidth, &textHeight);
    int leftOffset = textAreaWidth - textWidth;
    
    int textX = (width - textAreaWidth) / 2 + leftOffset;
    int textY = (width - textAreaHeight) / 2;
    gc.DrawText(wAngleStr, textX, textY);
}


BEGIN_EVENT_TABLE(RotationCtrl, wxPanel)
EVT_PAINT(RotationCtrl::onPaint)
END_EVENT_TABLE()
