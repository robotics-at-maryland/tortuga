/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Logging.cpp
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STD Includes
#include <iostream>
#include <vector>
#include <cstdlib>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Category.hh>

// Project Includes
#include "core/include/Logging.h"

namespace fs = boost::filesystem;

namespace ram {
namespace core {

Logging::Logging(core::ConfigNode config) :
    Subsystem(config["name"].asString("VisionSystem")),
    m_logger(0)
{
    init(config);
}
    
Logging::Logging(core::ConfigNode config, core::SubsystemList deps) :
    Subsystem(config["name"].asString("VisionSystem"), deps),
    m_logger(0)
{
    init(config);
}

Logging::~Logging()
{
    // Clean up al appenders
    std::pair<log4cpp::Appender*, std::vector<std::string> > pair;
    BOOST_FOREACH(pair, m_appenders)
    {
        log4cpp::Appender* appender = pair.first;
        // Remove from all categories
        BOOST_FOREACH(std::string categoryName, pair.second)
        {
            log4cpp::Category::getInstance(categoryName).
                removeAppender(appender);
        }

        // Free the appender
        delete appender;
    }
}

boost::filesystem::path Logging::getLogDir()
{
    // Gets the current time only once
    static boost::posix_time::ptime
        now(boost::posix_time::second_clock::local_time());

    // The startup time as a string
    std::string dateName(boost::posix_time::to_iso_extended_string(now));

    // The root of source directory
    fs::path root(getenv("RAM_SVN_DIR"));
    
    // The resulting path
    return root / "logs" / dateName;
}

void Logging::init(ConfigNode config)
{
    // Setup logging system category
    log4cpp::Category::getInstance("Logging");
    m_logger = log4cpp::Category::exists("Logging");
    log4cpp::Appender* appender =
        new log4cpp::OstreamAppender("console", &std::cout);
    appender->setLayout(new log4cpp::SimpleLayout);
    m_logger->setAppender(appender);
    
    std::map<std::string, log4cpp::Appender*> appenders;

    if (config.exists("Appenders"))
    {
        // Create Appenders
        ConfigNode appenderConfig(config["Appenders"]);
        NodeNameList subnodes = appenderConfig.subNodes();
        BOOST_FOREACH(std::string appenderName, subnodes)
        {
            ConfigNode config(appenderConfig[appenderName]);
            createAppender(appenderName, config, appenders);
        }
    }

    // Create categories
    if (config.exists("Categories"))
    {
        ConfigNode categoryConfig(config["Categories"]);
        NodeNameList subnodes = categoryConfig.subNodes();
            
        // Go through each category
        BOOST_FOREACH(std::string categoryName, subnodes)
        {
            createCategory(categoryName, categoryConfig[categoryName],
                           appenders);
        }
    }
}
    
void Logging::createCategory(
    std::string name,
    ConfigNode config,
    std::map<std::string, log4cpp::Appender*>& appenders)
{
    // Create Category
    log4cpp::Category::getInstance(name);
    log4cpp::Category* category = log4cpp::Category::exists(name);

    if (config.exists("appenders"))
    {
        ConfigNode appenderList(config["appenders"]);
        int appenderCount = appenderList.size();

        // Go through each appender name and attemp to add it to config
        for (int i = 0; i < appenderCount; ++i)
        {
            std::string appenderName(appenderList[i].asString());
            std::map<std::string, log4cpp::Appender*>::iterator iter =
                appenders.find(appenderName);

            if (iter != appenders.end())
            {
                // Add appender but don't shift ownership to category
                category->addAppender(*(iter->second));
                m_appenders[iter->second].push_back(name);
            }
            else
            {
                m_logger->errorStream() << "No such appender '"
                                        << appenderName;
            }
        }
    }
    else
    {
        m_logger->warnStream() << "Category '" << name
                               << "' has no appenders all logging messages"
                               <<" will be suppressed";
    }

    // Set priority
    category->setPriority(
        stringToPriority(config["priority"].asString("")));
}

log4cpp::Appender* Logging::createAppender(
    std::string name,
    ConfigNode config,
    std::map<std::string, log4cpp::Appender*>& appenders)
{
    std::string type = config["type"].asString();
    log4cpp::Appender* appender = 0;

    // Created based on given type (Not a factory but will do for now)
    if (type == "Console")
    {
        appender = new log4cpp::OstreamAppender(name, &std::cout);
    }
    else if (type == "File")
    {
        // Make sure log directory exists
        fs::path path(getLogDir());
        
        if (!fs::exists(path))
            fs::create_directories(path);
            
        // Determine file path
        std::string fileName = config["fileName"].asString(name);
        std::string filePath = (getLogDir() / fileName).string();
        
        // Create the appender which logs to the given file
        appender = new log4cpp::FileAppender(name, filePath, false);
    }
    else
    {
        m_logger->errorStream() << "Invalid appender type: '" << type << "'";
    }

    if (appender)
    {
        // Set layout (using default if needed)
        log4cpp::Layout* layout = 0;
        if (config.exists("Layout"))
            layout = createLayout(config["Layout"]);
        if (!layout)
            layout = new log4cpp::BasicLayout;
        appender->setLayout(layout);

        // Set threshold
        appender->setThreshold(
             stringToPriority(config["threshold"].asString("")));

        // Store the appender
        appenders[name] = appender;
        m_appenders[appender] = std::vector<std::string>();
    }

    return appender;
}
    
log4cpp::Layout* Logging::createLayout(ConfigNode config)
{
    std::string type = config["type"].asString();
    log4cpp::Layout* layout = 0;

    // Created based on given type (Not a factory but will do for now)
    if (type == "Pattern")
    {
        log4cpp::PatternLayout* patternLayout = new log4cpp::PatternLayout();
        patternLayout->setConversionPattern(
            config["pattern"].asString(
                log4cpp::PatternLayout::DEFAULT_CONVERSION_PATTERN));
    }
    else if (type == "Simple")
    {
        layout = new log4cpp::SimpleLayout();
    }
    else
    {
        m_logger->errorStream() << "Invalid layout type: '" << type << "'";
    }

    return layout;
}

log4cpp::Priority::PriorityLevel Logging::stringToPriority(std::string value)
{
    // Build string to priority map
    std::map<std::string, log4cpp::Priority::PriorityLevel> nameToPriority;
    nameToPriority["emergency"] = log4cpp::Priority::EMERG;
    nameToPriority["emerg"] = log4cpp::Priority::EMERG;
    nameToPriority["fatal"] = log4cpp::Priority::FATAL;
    nameToPriority["alert"] = log4cpp::Priority::ALERT;
    nameToPriority["crit"] = log4cpp::Priority::CRIT;
    nameToPriority["critical"] = log4cpp::Priority::CRIT;
    nameToPriority["error"] = log4cpp::Priority::ERROR;
    nameToPriority["warn"] = log4cpp::Priority::WARN;
    nameToPriority["warning"] = log4cpp::Priority::WARN;
    nameToPriority["info"] = log4cpp::Priority::INFO;
    nameToPriority["debug"] = log4cpp::Priority::DEBUG;
    
    // Convert to priority
    boost::to_lower(value);
    log4cpp::Priority::PriorityLevel priority =
        log4cpp::Priority::NOTSET;

    std::map<std::string, log4cpp::Priority::PriorityLevel>::iterator iter =
        nameToPriority.find(value);
    if (nameToPriority.end() != iter)
    {
        priority = iter->second;
    }
    else if (std::string("") != value)
    {
        m_logger->errorStream() << "Priority: '" << value << "' is not valid";
    }

    return priority;
}
    
} // namespace core
} // namespace ram
