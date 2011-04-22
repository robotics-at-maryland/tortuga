/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/PlotPanel.cpp
 */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <ctime>

#include "../include/PlotPanel.h"

#include "control/include/IController.h"
#include "estimation/include/IStateEstimator.h"
#include "math/include/Events.h"
#include "math/include/Vector2.h"

PlotPanel::PlotPanel(wxWindow* parent) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL) 
{
    wxSizer *outerBox = new wxBoxSizer(wxVERTICAL);
    wxSizer *plotBox = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);

    outerBox->Add(plotBox, 4, wxEXPAND);
    outerBox->Add(buttonBox, 1, wxEXPAND);

    m_plotWindow = new mpWindow(this, -1);
    plotBox->Add(m_plotWindow, 1, wxEXPAND | wxALL, 15);

    m_plotWindow->SetMargins(5,5,25,25);
    m_plotWindow->AddLayer(new mpScaleX(wxT("Time"), mpALIGN_BORDER_BOTTOM, true));
    m_plotWindow->AddLayer(new mpScaleY(wxT("Y-Value"), mpALIGN_BORDER_LEFT, true));
    m_plotWindow->Fit();
    m_plotWindow->EnableMousePanZoom(false);
    m_plotWindow->UpdateAll();
    m_bufferSize = 2000;
    m_lastUpdate = static_cast<time_t>(0.0);
    m_updatePeriod = static_cast<time_t>(1.0/30);

    buttonBox->Add(new wxButton(this, -1, _("Button1")), 1, wxEXPAND | wxALL, 10);
    buttonBox->Add(new wxButton(this, -1, _("Button2")), 1, wxEXPAND | wxALL, 10);
    buttonBox->Add(new wxButton(this, -1, _("Button3")), 1, wxEXPAND | wxALL, 10);

    SetSizer(outerBox);
}

void PlotPanel::addData(std::string dataSeriesName, double x, double y)
{
    DataSeriesBuffer &xBuffer = m_xBuffers[dataSeriesName];
    DataSeriesBuffer &yBuffer = m_yBuffers[dataSeriesName];

    xBuffer.push_back(x);
    yBuffer.push_back(y);

    if(xBuffer.size() > m_bufferSize)
        xBuffer.pop_front();
    if(yBuffer.size() > m_bufferSize)
        yBuffer.pop_front();
}

void PlotPanel::addDataSeries(std::string name)
{
    m_xBuffers[name] = DataSeriesBuffer();
    m_yBuffers[name] = DataSeriesBuffer();
    mpFXYVector* newPlot = new mpFXYVector(wxString(name.c_str(), wxConvUTF8));
    m_dataSeries[name] = newPlot;
    m_plotWindow->AddLayer(newPlot);
}

void PlotPanel::redraw()
{
    time_t currTime = std::time(NULL);
    time_t elapsed = std::difftime(currTime, m_lastUpdate);

    if(elapsed > m_updatePeriod)
    {
        for(DataSeriesMapIter it = m_dataSeries.begin(); it != m_dataSeries.end(); it++)
        {
            std::string dataSeriesName = (*it).first;
            mpFXYVector *plot = (*it).second;

            DataSeriesBuffer &xBuffer = m_xBuffers[dataSeriesName];
            DataSeriesBuffer &yBuffer = m_yBuffers[dataSeriesName];

            // need to put the data into vectors for the plot
            PlotData xData(xBuffer.size());
            PlotData yData(yBuffer.size());
    
            xData.assign(xBuffer.begin(), xBuffer.end());
            yData.assign(yBuffer.begin(), yBuffer.end());

            plot->SetData(xData, yData);
        }

        // recalculate bounds and force a redraw
        m_plotWindow->Fit();
        m_plotWindow->UpdateAll();
        m_lastUpdate = currTime;
    }
}

size_t PlotPanel::getBufferSize()
{
    return m_bufferSize;
}

void PlotPanel::setBufferSize(size_t numPoints)
{
    m_bufferSize = numPoints;

    for(DataSeriesMapIter it = m_dataSeries.begin(); it != m_dataSeries.end(); it++)
    {
        std::string dataSeriesName = (*it).first;
        DataSeriesBuffer &xBuffer = m_xBuffers[dataSeriesName];
        DataSeriesBuffer &yBuffer = m_yBuffers[dataSeriesName];

        while(xBuffer.size() > m_bufferSize)
            xBuffer.pop_front();
        while(yBuffer.size() > m_bufferSize)
            yBuffer.pop_front();
    }
}

std::string PlotPanel::name()
{
    return m_name;
}

std::string PlotPanel::description()
{
    return m_description;
}






TestPanel::TestPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    PlotPanel(parent)
{
    m_name = "Test Panel";
    m_description = "Plotting fake data";

    addDataSeries("Series 1");
    addDataSeries("Series 2");

    for(int n = 0; n < 20; n++)
    {
        addData("Series 1", n, n*n);
        addData("Series 2", n, 3*n);
    }

    redraw();
}

TestPanel::~TestPanel()
{
}


DepthPanel::DepthPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    PlotPanel(parent)
{
    m_name = "Depth Plot";
    m_description = "A plot of depth vs time.";
    m_eventHub = eventHub;

    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE,
            boost::bind(&DepthPanel::update, this, _1)));

    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_DEPTH_UPDATE,
            boost::bind(&DepthPanel::update, this, _1)));

    addDataSeries("Estimated Depth");
    addDataSeries("Desired Depth");
}
 
DepthPanel::~DepthPanel()
{
    BOOST_FOREACH(core::EventConnectionPtr conn, m_connections)
    {
        conn->disconnect();
    }
}

void DepthPanel::update(core::EventPtr event)
{
    math::NumericEventPtr nEvent = 
        boost::dynamic_pointer_cast<math::NumericEvent>(event);

    if(nEvent)
    {
        double depth = nEvent->number;
        double time = nEvent->timeStamp;

        if(event->type == estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE)
        {
            addData("Estimated Depth", time, depth);
        } 
        else if(event->type == control::IController::DESIRED_DEPTH_UPDATE)
        {
            addData("Desired Depth", time, depth);
        }
        redraw();
    }
}




DepthRatePanel::DepthRatePanel(wxWindow* parent, core::EventHubPtr eventHub) :
    PlotPanel(parent)
{
    m_name = "Depth Rate Plot";
    m_description = "A plot of the time rate change of depth vs time.";

    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_DEPTHRATE_UPDATE,
            boost::bind(&DepthRatePanel::update, this, _1)));

    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_DEPTHRATE_UPDATE,
            boost::bind(&DepthRatePanel::update, this, _1)));

    addDataSeries("Estimated Depth Rate");
    addDataSeries("Desired Depth Rate");
}

DepthRatePanel::~DepthRatePanel()
{
    BOOST_FOREACH(core::EventConnectionPtr conn, m_connections)
    {
        conn->disconnect();
    }
}

void DepthRatePanel::update(core::EventPtr event)
{
    math::NumericEventPtr nEvent = 
        boost::dynamic_pointer_cast<math::NumericEvent>(event);

    if(nEvent)
    {
        double depthRate = nEvent->number;
        double time = nEvent->timeStamp;

        if(event->type == estimation::IStateEstimator::ESTIMATED_DEPTHRATE_UPDATE)
        {
            addData("Estimated Depth Rate", time, depthRate);
        } 
        else if(event->type == control::IController::DESIRED_DEPTHRATE_UPDATE)
        {
            addData("Desired Depth Rate", time, depthRate);
        }
        redraw();
    }
}


PositionPanel::PositionPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    PlotPanel(parent)
{
    m_name = "Position Plot";
    m_description = "A plot of y-position vs x-position.";

    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_POSITION_UPDATE,
            boost::bind(&PositionPanel::update, this, _1)));

    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_POSITION_UPDATE,
            boost::bind(&PositionPanel::update, this, _1)));

    addDataSeries("Estimated Position");
    addDataSeries("Desired Position");
}

PositionPanel::~PositionPanel()
{
    BOOST_FOREACH(core::EventConnectionPtr conn, m_connections)
    {
        conn->disconnect();
    }
}

void PositionPanel::update(core::EventPtr event)
{
    math::Vector2EventPtr vEvent = 
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    if(vEvent)
    {
        math::Vector2 position = vEvent->vector2;
        
        if(event->type == estimation::IStateEstimator::ESTIMATED_POSITION_UPDATE)
        {
            addData("Estimated Position", position[0], position[1]);
        } 
        else if(event->type == control::IController::DESIRED_POSITION_UPDATE)
        {
            addData("Desired Position", position[0], position[1]);
        }
        redraw();
    }
}




VelocityPanel::VelocityPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    PlotPanel(parent)
{
    m_name = "Velocity Plot";
    m_description = "A plot of y-velocity vs x-velocity";

    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE,
            boost::bind(&VelocityPanel::update, this, _1)));

    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_VELOCITY_UPDATE,
            boost::bind(&VelocityPanel::update, this, _1)));

    addDataSeries("Estimated Velocity");
    addDataSeries("Desired Velocity");
}

VelocityPanel::~VelocityPanel()
{
    BOOST_FOREACH(core::EventConnectionPtr conn, m_connections)
    {
        conn->disconnect();
    }
}

void VelocityPanel::update(core::EventPtr event)
{
    math::Vector2EventPtr vEvent = 
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    if(vEvent)
    {
        math::Vector2 velocity = vEvent->vector2;
        
        if(event->type == estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE)
        {
            addData("Estimated Velocity", velocity[0], velocity[1]);
        } 
        else if(event->type == control::IController::DESIRED_VELOCITY_UPDATE)
        {
            addData("Desired Velocity", velocity[0], velocity[1]);
        }
        redraw();
    }
}
