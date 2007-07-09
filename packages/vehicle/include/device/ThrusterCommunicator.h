/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/ThrusterCommunicator.h
 */

#ifndef RAM_CORE_THRUSTERCOMMUNICATOR_06_24_2006
#define RAM_CORE_THRUSTERCOMMUNICATOR_06_24_2006

// STD Includes
#include <set>

// Project Includes
#include "pattern/include/Singleton.h"
#include "core/include/Updatable.h"
#include "core/include/ThreadedQueue.h"

#include "vehicle/include/device/Common.h"

namespace ram {
namespace vehicle {
namespace device{

typedef std::set<Thruster*> ThrusterSet;
typedef ThrusterSet::iterator ThrusterSetIter;

typedef core::ThreadedQueue<ThrusterCommandPtr> ThrusterCommandQueue;

class ThrusterCommunicator : public pattern::Singleton<ThrusterCommunicator>,
                             public core::Updatable
                                 
{
public:
    ThrusterCommunicator();
    ~ThrusterCommunicator();
    
    /** Grab reference to singleton object (asserts if singleton not present) */
    static ThrusterCommunicator& getSingleton();
    /** Grab pointer to singleton object (can be NULL) */
    static ThrusterCommunicator* getSingletonPtr();

    /** Registers a thruster with the communicator
     *
     *  If the singleton instance has not been created it will be created at
     *  this time, and then the thruster will be added.
     */
    static void registerThruster(Thruster* thruster);

    /** Ungersiters a thruster, delete singleton when there are zero left */
    static void unRegisterThruster(Thruster* thruster);

    /** Adds a command to background thruster command queue
     *
     *  Ownership of the ThrusteCommand passes to the ThrusterCommunicator.
     */
    void sendThrusterCommand(ThrusterCommandPtr cmd);

    /** Run in the background at a fixed rate */
    virtual void update(double timestep);
    
protected:
    /** Called to wait until the next iteration
     *
     *  This will wait for the given time, but will wake up early if given a
     *  command.  This is called over and over untill there it no time left to
     *  sleep.
     */
    virtual void waitForUpdate(long microseconds);
    
private:
    /** Actually add the thruster to the internal list */
    void addThruster(Thruster* thruster);

    /** Actually add the thruster to the internal list */
    void removeThruster(Thruster* thruster);

    /** Runs all messages in the command queue */
    void processCommands();

    /** Runs the given thruster command, and checks the return code */
    void runCommand(ThrusterCommandPtr command);

    /** Clears read buffer */
    void clearReadBuffer();
    
    /** List of Internal Thrusters*/
    ThrusterSet m_thrusters;

    /** ThreadQueue of incomming commands */
    ThrusterCommandQueue m_commandQueue;

    /** file descriptor of the serial device file*/
    int m_serialFD;

    bool m_captureOutput;
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_CORE_THRUSTERCOMMUNICATOR_06_24_2006
