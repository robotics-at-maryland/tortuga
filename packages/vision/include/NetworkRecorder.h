/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/NetworkRecorder.h
 */

#ifndef RAM_NETWORKRECORDER_H_02_25_2008
#define RAM_NETWORKRECORDER_H_02_25_2008

// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

// Project Includes
#include "vision/include/Recorder.h"

// Must be included last
#include "vision/include/Export.h"

struct sockaddr_in;

namespace ram {
namespace vision {

struct ImagePacketHeader
{
    boost::uint16_t width;
    boost::uint16_t height;
    boost::uint32_t dataSize;
};
    
/** Sends data from the given camera over the network */
class RAM_EXPORT NetworkRecorder : public Recorder
{
  public:
    /** Creates a record which broadcasts data using TCP on the given port */
    NetworkRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                    boost::uint16_t port, int policyArg = 0);

    virtual ~NetworkRecorder();

    /** Waits for incoming connections, then loops sending off images
     *
     *  If there is no current client connected, this will run an accept call
     *  until it gets one.
     */
    virtual void update(double timeSinceLastUpdate);

    /** Stops the background thread and closes network connections */
    virtual void unbackground(bool join = false);

    /** Waits until we can accept a client connection
     *
     *  Returns immedietly if we already have a connection.
     */
    virtual void waitForAccepting();
    
  protected:
    /** Called whenever there is a frame to record, sends data over network */
    virtual void recordFrame(Image* image);
    
  private:
    /** If not done already, sets up the listenSocket for the accept call */
    void setupListenSocket();

    /** Send data out on the current socket */
    bool sendData(void* buf, size_t len);

    /** Accept connections on the listening socket */
    int acceptConnection(double timeout);
    
    /** Port to listen for connections on */
    boost::uint16_t m_port;

    /** Protect access to network sockets and addresse */
    boost::mutex m_mutex;

    /** Allows waiting for a client to connect */
    boost::condition m_waitForAccepting;
    
    /** TCP socket used to listen for connections on */
    int m_listenSocket;

    /** TCP socket we use to send data to a client */
    int m_currentSocket;

    /** Our address, just needed for bind call */
    struct sockaddr_in* m_addr;

    /** The address of the current connected socket */
    struct sockaddr_in* m_currentAddr;

    /** Holds the width, height, and data size of the last image sent */
    ImagePacketHeader m_packet;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_NETWORKRECORDER_H_02_25_2008
