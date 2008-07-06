/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestLogging.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>

// Project Includes
#include "core/include/Logging.h"
#include "core/include/ConfigNode.h"

namespace fs = boost::filesystem;
using namespace ram;

struct LoggingFixture
{
    void suppressLogMessages()
    {
        log4cpp::Category::getInstance("Logging").setPriority(
            log4cpp::Priority::FATAL);
    }
    void allowLogMessages()
    {
        log4cpp::Category::getInstance("Logging").setPriority(
            log4cpp::Priority::NOTSET);
    }
};

TEST_FIXTURE(LoggingFixture, CategoryCreation)
{
    core::ConfigNode config(core::ConfigNode::fromString("{"
        "'Categories' : {"
        "     'Control' : {"
        "         'priority' : 'warning'"
        "     },"
        "     'Vehicle' : {"
        "         'priority' : 'debug'"
        "     }"
        "}}"));
    suppressLogMessages(); // Get rid of no appender warnings
    core::Logging logSys(config);
    allowLogMessages();
    
    // Make sure the categories were actually
    CHECK(log4cpp::Category::exists("Control"));
    CHECK_EQUAL(log4cpp::Priority::WARN, 
                log4cpp::Category::exists("Control")->getPriority());
    CHECK(log4cpp::Category::exists("Vehicle"));
    CHECK_EQUAL(log4cpp::Priority::DEBUG, 
                log4cpp::Category::exists("Vehicle")->getPriority());
}

TEST_FIXTURE(LoggingFixture, AppenderCreation)
{
    core::ConfigNode config(core::ConfigNode::fromString("{"
        "'Categories' : {"
        "     'Control' : {"
        "         'appenders' : ['screen', 'file']"
        "     }"
        "},"
        "'Appenders' : {"
        "    'screen' : {"
        "        'type' : 'Console',"
        "        'threshold' : 'critical'"
        "    },"
        "    'file' : {"
        "        'type' : 'File',"
        "        'fileName' : 'unittest',"
        "        'threshold' : 'info'"
        "    }"
        "}}"));
    core::Logging* logSys = new core::Logging(config);

    // Ensure our category was actually created
    log4cpp::Category* category = log4cpp::Category::exists("Control");
    CHECK(category);

    // Make sure it has the proper appenders
    log4cpp::Appender* appender = category->getAppender("screen");
    CHECK(appender);
    log4cpp::OstreamAppender* ostreamAppender =
        dynamic_cast<log4cpp::OstreamAppender*>(appender);
    CHECK(ostreamAppender);

    appender = category->getAppender("file");
    CHECK(appender);
    log4cpp::FileAppender* fileAppender =
        dynamic_cast<log4cpp::FileAppender*>(appender);
    CHECK(fileAppender);

    // Ensure the thresholds were set
    CHECK_EQUAL(log4cpp::Priority::CRIT,
                ostreamAppender->getThreshold());
    CHECK_EQUAL(log4cpp::Priority::INFO,
                fileAppender->getThreshold());
    
    // Ensure that the proper logging directory was created
    fs::path logPath = core::Logging::getLogDir();
    CHECK(fs::exists(logPath));
    
    std::string pathName(logPath.string());
    std::string dateName(boost::posix_time::to_iso_extended_string(
                             boost::posix_time::second_clock::local_time()));
    std::string expectedLogDirName = dateName.substr(0, 16);
    std::string actualLogDirName = pathName.substr(pathName.length() - 19, 16);
    CHECK_EQUAL(expectedLogDirName, actualLogDirName);

    // Ensure that priorities are proper set

    // Make sure all appenders are deleted
    delete logSys;
    CHECK(0 == category->getAppender());
}
