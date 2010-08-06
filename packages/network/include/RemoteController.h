/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/network/include/RemoteController.h
 */

#ifndef RAM_NETWORK_REMOVECONTROLLER_03_04_2008
#define RAM_NETWORK_REMOVECONTROLLER_03_04_2008

// Library Includes
#include <boost/cstdint.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/Updatable.h"
#include "core/include/ConfigNode.h"
#include "control/include/Common.h"

namespace ram {
namespace network {

/** An object which transfers network commands from OISclient to a Controller
 *
 *  This <b>DOES NOT</b> implement any fail safe functionality.
 */
class RAM_EXPORT RemoteController :
        public core::Subsystem,
        public core::Updatable
{
  public:
    RemoteController(core::ConfigNode config,
                     core::SubsystemList deps = core::SubsystemList());

    virtual ~RemoteController();

    /** Turn on network control inputs */
    void enable();

    /** Turn off network control inputs */
    void disable();

    /** Runs in a continous loop blocking on reading messages */
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

    virtual bool backgrounded()
    {
        return Updatable::backgrounded();
    }

  private:
    /** Sets up the networking based on the setting defined above
     *
     * @return  A socket file descriptor to listen on, if < 0 there was an
     *          error.
     */
    void setupNetworking(int16_t port);

    /** Process the incomming network messages
     *
     * @return If false it means an emergency shutdown command was sent
     */
    bool processMessage(unsigned char cmd, signed char param = 0);

    /** The port to listen on*/
    int m_port;
    
    /** Socket to recieve on */
    int m_sockfd;

    /** The controller to send commands to */
    control::IControllerPtr m_controller;

    double m_maxDepth;
    double m_minDepth;
    double m_depthEnc;
    double m_turnEnc;
    double m_minSpeed;
    double m_maxSpeed;
    double m_speedEnc;
    double m_yawGain;
    double m_pitchGain;
    double m_rollGain;
};

} // namespace network
} // namespace ram

#endif // RAM_NETWORK_REMOVECONTROLLER_03_04_2008
