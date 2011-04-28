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
#include "wx/wx.h"
#include "PlotPanel.h"

#include "control/include/IController.h"
#include "estimation/include/IStateEstimator.h"
#include "math/include/Events.h"
#include "math/include/Vector2.h"

#define ID_CLEAR_BUTTON 4000
#define ID_BUFFER_SIZE_CTRL 4001

PlotPanel::PlotPanel(wxWindow* parent, wxString name, wxString caption) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTAB_TRAVERSAL | wxVSCROLL | wxHSCROLL) 
{
    // set default wxAuiPaneInfo parameters
    m_info.Gripper(true);
    m_info.GripperTop(true);
    m_info.MinSize(240, 180);
    m_info.BestSize(512,320);
    m_info.Movable(true);
    m_info.Resizable(true);
    m_info.CaptionVisible(true);
    m_info.Floatable(true);
    m_info.DestroyOnClose(true);
    m_info.CloseButton(false);
    m_info.Hide();

    SetMinSize(wxSize(240, 180));

    wxSizer *outerBox = new wxBoxSizer(wxVERTICAL);
    wxSizer *plotBox = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);

    outerBox->Add(plotBox, 4, wxEXPAND);
    outerBox->Add(buttonBox, 0, wxEXPAND);

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

    wxStaticText *bufferSizeLabel = new wxStaticText(this, -1, wxT("Buffer Size"));
    m_bufferSizeCtrl = new wxTextCtrl(this, ID_BUFFER_SIZE_CTRL, wxT("2000"),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_PROCESS_ENTER);

    wxButton *clearButton = new wxButton(this, ID_CLEAR_BUTTON, wxT("Clear Data"));

    buttonBox->Add(bufferSizeLabel, 1, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 10);
    buttonBox->Add(m_bufferSizeCtrl, 1, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 10);
    buttonBox->Add(clearButton, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 10);

    SetBackgroundColour(wxColour(wxT("WHITE")));
    SetSizer(outerBox);
}

wxAuiPaneInfo& PlotPanel::info()
{
    return m_info;
}

void PlotPanel::addData(std::string dataSeriesName, double x, double y)
{
    DataSeriesBufferMap::iterator iter;
    for(iter = m_yBuffers.begin(); iter != m_yBuffers.end(); iter++)
    {
        std::string name = (*iter).first;
        DataSeriesBuffer &yBuffer = m_yBuffers[name];
        if(yBuffer.empty())
        {
            m_xBuffers[name].push_back(x);
            yBuffer.push_back(0);
        }
    }
    
    DataSeriesBuffer &xBuffer = m_xBuffers[dataSeriesName];
    DataSeriesBuffer &yBuffer = m_yBuffers[dataSeriesName];

    xBuffer.push_back(x);
    yBuffer.push_back(y);

    if(xBuffer.size() > m_bufferSize)
        xBuffer.pop_front();
    if(yBuffer.size() > m_bufferSize)
        yBuffer.pop_front();
}

void PlotPanel::addDataSeries(std::string name, wxPen pen)
{
    m_xBuffers[name] = DataSeriesBuffer();
    m_yBuffers[name] = DataSeriesBuffer();
    //mpFXYVector* newPlot = new mpFXYVector(wxString(name.c_str(), wxConvUTF8));
    mpFXYVector* newPlot = new mpFXYVector(wxT(""));
    newPlot->SetPen(pen);
    newPlot->SetContinuity(true);
    m_dataSeries[name] = newPlot;
    m_plotWindow->AddLayer(newPlot);
}

void PlotPanel::redraw()
{
    time_t currTime = std::time(NULL);
    time_t elapsed = std::difftime(currTime, m_lastUpdate);
    elapsed = elapsed;
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

    wxString bufSizeStr = wxString::Format(wxT("%i"), numPoints);
    m_bufferSizeCtrl->SetValue(bufSizeStr);
    redraw();
}

void PlotPanel::onBufferSizeUpdate(wxCommandEvent& event)
{
    wxString numStr = event.GetString();
    unsigned long num = 0;
    bool successfulConversion = numStr.ToULong(&num);

    if(successfulConversion)
        setBufferSize(num);
    else
        setBufferSize(getBufferSize());
}

void PlotPanel::onClear(wxCommandEvent& WXUNUSED(event))
{
    clear();
}

void PlotPanel::clear()
{
    m_xBuffers.clear();
    m_yBuffers.clear();
    redraw();
}

wxString PlotPanel::name()
{
    return m_name;
}

wxString PlotPanel::caption()
{
    return m_caption;
}

BEGIN_EVENT_TABLE(PlotPanel, wxPanel)
EVT_TEXT_ENTER(ID_BUFFER_SIZE_CTRL, PlotPanel::onBufferSizeUpdate)
EVT_BUTTON(ID_CLEAR_BUTTON, PlotPanel::onClear)
END_EVENT_TABLE();



EventBased::EventBased(core::EventHubPtr eventHub) :
    m_eventHub(eventHub)
{
}

EventBased::~EventBased()
{
    BOOST_FOREACH(core::EventConnectionPtr conn, m_connections)
    {
        if(conn->connected())
            conn->disconnect();
    }
}

TestPanel::TestPanel(wxWindow* parent) :
    PlotPanel(parent)
{
    m_name = wxT("Test Panel");
    m_caption = m_name;

    m_info.Name(m_name);
    m_info.Caption(m_caption);

    addDataSeries("Series 1", wxPen(wxColour(wxT("RED")), 5));
    addDataSeries("Series 2", wxPen(wxColour(wxT("DARK GREEN")), 5));

    for(int n = 0; n < 20; n++)
    {
        addData("Series 1", n, n*n);
        addData("Series 2", n, 3*n);
    }

    redraw();
}

DepthErrorPanel::DepthErrorPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    EventBased(eventHub),
    PlotPanel(parent)
{
    m_name = wxT("Depth Errpr Plot");
    m_caption = m_name;

    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE,
            boost::bind(&DepthErrorPanel::update, this, _1)));

    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_DEPTH_UPDATE,
            boost::bind(&DepthErrorPanel::update, this, _1)));

    m_estDepth = 0;
    m_desDepth = 0;

    addDataSeries("Depth Error", wxPen(wxColour(wxT("RED")), 5));
}

void DepthErrorPanel::update(core::EventPtr event)
{
    math::NumericEventPtr nEvent = 
        boost::dynamic_pointer_cast<math::NumericEvent>(event);

    if(nEvent)
    {
        double depth = nEvent->number;
        double time = nEvent->timeStamp;

        if(event->type == estimation::IStateEstimator::ESTIMATED_DEPTHRATE_UPDATE)
            m_estDepth = depth;

        else if(event->type == control::IController::DESIRED_DEPTHRATE_UPDATE)
            m_desDepth = depth;

        double depthError = std::fabs(m_desDepth - m_estDepth);
        addData("Depth Error", time, depthError);

        redraw();
    }
}

NumericVsTimePlot::NumericVsTimePlot(wxWindow *parent, core::EventHubPtr eventHub,
                                     EventSeriesMap series, wxString name,
                                     wxString caption) :
    EventBased(eventHub),
    PlotPanel(parent),
    m_series(series)
{
    m_name = name;
    m_caption = caption;

    m_info.Name(name);
    m_info.Caption(caption);

    EventSeriesMap::iterator iter;
    for(iter = m_series.begin(); iter != m_series.end(); iter++)
    {
        core::Event::EventType type = (*iter).first;
        DataSeriesInfo seriesInfo = (*iter).second;

        m_connections.push_back(
            eventHub->subscribeToType(
                type, boost::bind(&NumericVsTimePlot::update, this, _1)));

        addDataSeries(seriesInfo.name, seriesInfo.pen);
    }
}

void NumericVsTimePlot::update(core::EventPtr event)
{
    math::NumericEventPtr nEvent = 
        boost::dynamic_pointer_cast<math::NumericEvent>(event);

    if(nEvent)
    {
        double number = nEvent->number;
        double time = nEvent->timeStamp;

        DataSeriesInfo seriesInfo = m_series[event->type];
        addData(seriesInfo.name, time,  number);

        redraw();
    }
}

Vector2PhasePlot::Vector2PhasePlot(wxWindow *parent, core::EventHubPtr eventHub,
                                   EventSeriesMap series, wxString name,
                                   wxString caption) :
    EventBased(eventHub),
    PlotPanel(parent),
    m_series(series)
{
    m_name = name;
    m_caption = caption;

    m_info.Name(name);
    m_info.Caption(caption);

    EventSeriesMap::iterator iter;
    for(iter = m_series.begin(); iter != m_series.end(); iter++)
    {
        core::Event::EventType type = (*iter).first;
        DataSeriesInfo seriesInfo = (*iter).second;

        m_connections.push_back(
            eventHub->subscribeToType(
                type, boost::bind(&Vector2PhasePlot::update, this, _1)));

        addDataSeries(seriesInfo.name, seriesInfo.pen);
    }
}

void Vector2PhasePlot::update(core::EventPtr event)
{
    math::Vector2EventPtr vEvent = 
        boost::dynamic_pointer_cast<math::Vector2Event>(event);

    if(vEvent)
    {
        math::Vector2 vector2 = vEvent->vector2;

        DataSeriesInfo seriesInfo = m_series[event->type];
        addData(seriesInfo.name, vector2[0],  vector2[1]);

        redraw();
    }
}
