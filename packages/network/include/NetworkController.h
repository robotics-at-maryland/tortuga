/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/network/include/NetworkController.h
 */

#ifndef RAM_NETWORK_NETWORKCONTROLLER_03_04_2008
#define RAM_NETWORK_NETWORKCONTROLLER_03_04_2008

// Library Includes
#include <boost/cstdint.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "control/include/Common.h"
#include "estimation/include/Common.h"
#include "math/include/Vector2.h"
#include "network/include/Receiver.h"

namespace ram {
namespace network {

/** An object which transfers network commands from OISclient to a Controller
 *
 *  This <b>DOES NOT</b> implement any fail safe functionality.
 */
class RAM_EXPORT NetworkController :
        public core::Subsystem,
        public core::Updatable
{
  public:
    NetworkController(core::ConfigNode config,
                     core::SubsystemList deps = core::SubsystemList());

    virtual ~NetworkController();

    /** Turn on network control inputs */
    void enable();

    /** Turn off network control inputs */
    void disable();

    /** Does nothing (reads are done in a background process) */
    virtual void update(double timeSinceLastUpdate);

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval);

    virtual void unbackground(bool join = false);

    virtual bool backgrounded();

  private:
    void accept(const char* msg);

    /** Process the incomming network messages
     *
     * @return If false it means an emergency shutdown command was sent
     */
    bool processMessage(unsigned char cmd, signed char param = 0);

    /** The port to listen on*/
    int m_port;

    /** Message receiver */
    Receiver* m_receiver;

    /** Internal speed values from the connection */
    double m_speed;
    double m_tspeed;
    double m_yaw;
    double m_pitch;
    double m_roll;
    bool m_descending;
    bool m_ascending;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_NETWORKCONTROLLER_03_04_2008
