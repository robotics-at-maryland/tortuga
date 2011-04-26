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
#include "wx/aui/aui.h"
#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"
#include "core/include/Event.h"

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
    PlotPanel(wxWindow* parent, wxString name = wxT("Unnamed"),
              wxString caption = wxT("Undescribed"));

    ~PlotPanel() {}

    wxString name();
    wxString caption();

    size_t getBufferSize();
    void setBufferSize(size_t numPoints);
    void onBufferSizeUpdate(wxCommandEvent& event);
    void onClear(wxCommandEvent& event);
    wxAuiPaneInfo& info();

protected:

    void addData(std::string name, double x, double y);
    void addDataSeries(std::string name,
                       wxPen pen = wxPen(wxColour(wxT("BLACK")), 2, wxDOT));
    void redraw();
    void clear();

    DECLARE_EVENT_TABLE();

    wxString m_name;
    wxString m_caption;
    wxAuiPaneInfo m_info;

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

struct DataSeriesInfo
{
    DataSeriesInfo() :
        name("unnamed"),
        pen(wxPen(wxColour(wxT("BLACK")))) {}

    DataSeriesInfo(std::string name,
                         wxPen pen = wxPen(wxColour(wxT("BLACK")))) :
        name(name),
        pen(pen) {}

    std::string name;
    wxPen pen;
};

class EventBasedPlotPanel : public PlotPanel
{
public:
    EventBasedPlotPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~EventBasedPlotPanel();

    virtual void update(core::EventPtr event) = 0;

protected:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;

};

class TestPanel : public PlotPanel
{
public:
    TestPanel(wxWindow* parent);
    ~TestPanel() {}
};

class DepthErrorPanel : public EventBasedPlotPanel
{
public:
    DepthErrorPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~DepthErrorPanel() {}
    virtual void update(core::EventPtr event);

private:
    double m_estDepth;
    double m_desDepth;
};

class PositionPanel : public EventBasedPlotPanel
{
public:
    PositionPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~PositionPanel() {}
    virtual void update(core::EventPtr event);
};

class VelocityPanel : public EventBasedPlotPanel
{
public:
    VelocityPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~VelocityPanel() {}
    virtual void update(core::EventPtr event);
};

typedef std::map<core::Event::EventType, DataSeriesInfo> EventSeriesMap;
class NumericVsTimePlot : public EventBasedPlotPanel
{
public:
    NumericVsTimePlot(wxWindow* parent, core::EventHubPtr eventHub,
                      EventSeriesMap series, wxString name, 
                      wxString caption = wxT(""));

    ~NumericVsTimePlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
};

#endif // RAM_TOOLS_PLOTPANEL
