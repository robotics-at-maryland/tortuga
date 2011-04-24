/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/include/PlotPanel.h
 */

#ifndef RAM_TOOLS_PLOTPANEL
#define RAM_TOOLS_PLOTPANEL

#include <deque>
#include <vector>
#include <map>

#include "../include/mathplot.h"
#include "wx/wx.h"
#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"

using namespace ram;

typedef std::vector<double> PlotData;

typedef std::deque<double> DataSeriesBuffer;
typedef DataSeriesBuffer::iterator DataSeriesBufferIter;

typedef std::map< std::string, DataSeriesBuffer > DataSeriesBufferMap;
typedef DataSeriesBufferMap::iterator DataSeriesBufferMapIter;

typedef std::map< std::string, mpFXYVector* > DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

class PlotPanel : public wxPanel
{
public:
    PlotPanel(wxWindow* parent);

    ~PlotPanel() {}

    std::string name();
    std::string description();

    size_t getBufferSize();
    void setBufferSize(size_t numPoints);
    void onBufferSizeUpdate(wxCommandEvent& event);
    void onClear(wxCommandEvent& event);

protected:
    DECLARE_EVENT_TABLE();

    void addData(std::string name, double x, double y);
    void addDataSeries(std::string name,
                       wxPen pen = wxPen(wxColour(wxT("BLACK")), 2, wxDOT));
    void redraw();
    void clear();

    std::string m_name;
    std::string m_description;

private:
    size_t m_bufferSize;
    time_t m_lastUpdate;
    time_t m_updatePeriod;

    // layer -> data buffer
    DataSeriesBufferMap m_xBuffers;
    DataSeriesBufferMap m_yBuffers;

    // layer -> plot for the layer
    DataSeriesMap m_dataSeries;

    // container for all the plot layers
    mpWindow *m_plotWindow;

    wxTextCtrl *m_bufferSizeCtrl;
};

class TestPanel : public PlotPanel
{
public:
    TestPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~TestPanel();
};

class DepthPanel : public PlotPanel
{
public:
    DepthPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~DepthPanel();

    void update(core::EventPtr event);
    
private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;
};

class DepthRatePanel : public PlotPanel
{
public:
    DepthRatePanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~DepthRatePanel();

    void update(core::EventPtr event);

private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;
};

class DepthErrorPanel : public PlotPanel
{
public:
    DepthErrorPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~DepthErrorPanel();

    void update(core::EventPtr event);

private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;

    double m_estDepth;
    double m_desDepth;
};

class PositionPanel : public PlotPanel
{
public:
    PositionPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~PositionPanel();

    void update(core::EventPtr event);
private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;
};

class VelocityPanel : public PlotPanel
{
public:
    VelocityPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~VelocityPanel();

    void update(core::EventPtr event);
private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;
};

#endif // RAM_TOOLS_PLOTPANEL
