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
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

using namespace ram;

typedef std::map< std::string, mpFXYDeque* > DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

struct DataSeriesInfo
{
    DataSeriesInfo() :
        name("unnamed"),
        pen(wxPen(wxColour(wxT("BLACK")))),
        repeat(false) {}

    DataSeriesInfo(std::string name,
                   wxPen pen = wxPen(wxColour(wxT("BLACK"))),
                   bool repeat = false) :
        name(name),
        pen(pen),
        repeat(repeat) {}

    std::string name;
    wxPen pen;
    bool repeat;
};

typedef std::map<core::Event::EventType, DataSeriesInfo> EventSeriesMap;

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
    void onPaint(wxPaintEvent& event);
    wxAuiPaneInfo& info();

    void addData(std::string name, double x, double y);
    void addDataSeries(std::string name,
                       wxPen pen = wxPen(wxColour(wxT("BLACK")), 2, wxDOT));

    void redraw();
    void clear();

protected:
    DECLARE_EVENT_TABLE();

    wxString m_name;
    wxString m_caption;
    wxAuiPaneInfo m_info;

private:
    size_t m_bufferSize;
    time_t m_lastUpdate;
    time_t m_updatePeriod;

    // layer -> plot for the layer
    DataSeriesMap m_dataSeries;

    // container for all the plot layers
    mpWindow *m_plotWindow;

    wxTextCtrl *m_bufferSizeCtrl;
};

class EventBased
{
public:
    EventBased(core::EventHubPtr eventHub);
    ~EventBased();

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

class DepthErrorPanel : public EventBased, public PlotPanel
{
public:
    DepthErrorPanel(wxWindow* parent, core::EventHubPtr eventHub);
    ~DepthErrorPanel() {}
    virtual void update(core::EventPtr event);

private:
    double m_estDepth;
    double m_desDepth;
};

class NumericVsTimePlot : public EventBased, public PlotPanel
{
public:
    NumericVsTimePlot(wxWindow* parent, core::EventHubPtr eventHub,
                      EventSeriesMap series, wxString name, 
                      wxString caption = wxT(""));

    ~NumericVsTimePlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
    std::map<core::Event::EventType, double> m_prevData;
};

class Vector2PhasePlot : public EventBased, public PlotPanel
{
public:
    Vector2PhasePlot(wxWindow* parent, core::EventHubPtr eventHub,
                      EventSeriesMap series, wxString name, 
                      wxString caption = wxT(""));

    ~Vector2PhasePlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
};

class Vector2ComponentPlot : public EventBased, public PlotPanel
{
public:
    Vector2ComponentPlot(wxWindow* parent, core::EventHubPtr eventHub,
                         EventSeriesMap series, unsigned int component,
                         wxString name, wxString caption = wxT(""));

    ~Vector2ComponentPlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
    unsigned int m_component;
    std::map<core::Event::EventType, double> m_prevData;
};


class Vector3ComponentPlot : public EventBased, public PlotPanel
{
public:
    Vector3ComponentPlot(wxWindow* parent, core::EventHubPtr eventHub,
                         EventSeriesMap series, unsigned int component,
                         wxString name, wxString caption = wxT(""));

    ~Vector3ComponentPlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
    unsigned int m_component;
    std::map<core::Event::EventType, double> m_prevData;
};

class ObstacleLocationPlot : public EventBased, public PlotPanel
{
public:
    ObstacleLocationPlot(wxWindow* parent, core::EventHubPtr eventHub,
                         EventSeriesMap series,
                         wxString name, wxString caption = wxT(""));

    ~ObstacleLocationPlot() {}

    virtual void update(core::EventPtr event);

private:
    EventSeriesMap m_series;
    std::map<core::Event::EventType, math::Vector3> m_prevData;
};

#endif // RAM_TOOLS_PLOTPANEL
