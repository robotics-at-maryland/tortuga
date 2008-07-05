/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Logging.h
 */

#ifndef RAM_CORE_LOGGING_07_04_2008
#define RAM_CORE_LOGGING_07_04_2008

// STD Includes
#include <map>
#include <string>
#include <vector>

// Library Includes
#include <boost/filesystem/path.hpp>

namespace log4cpp {
    class Appender;
    class Layout;
};

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"

namespace ram {
namespace core {

/** Handles the initialization of the Log4cpp based logging system */
class Logging : public Subsystem, public Updatable
{
public:
    Logging(ConfigNode config);
    Logging(ConfigNode config, SubsystemList deps);
    ~Logging();

    /** Gets the directory where all log files will be placed */
    static boost::filesystem::path getLogDir();


        virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    // IUpdatable methods
    virtual void update(double) {}
    
    virtual IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };
    
private:
    /** Creates all parts of the underlying logging system */
    void init(ConfigNode config);
    
    /** Creates a category with from given config and appenders */
    void createCategory(std::string name, ConfigNode config,
                        std::map<std::string, log4cpp::Appender*>& appenders);
    
    /** Creates a single appender from the given config section */
    log4cpp::Appender* createAppender(
        std::string name, ConfigNode config,
        std::map<std::string, log4cpp::Appender*>& appenders);
    
    /** Creates a layout from the given config section */
    log4cpp::Layout* createLayout(ConfigNode config);
    
    /** A list of the appenders current being used */
    std::map<log4cpp::Appender*, std::vector<std::string> > m_appenders;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_LOGGIN_07_04_2008
