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
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

// Project Includes
#include "vision/include/Recorder.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
/** Sends data from the given camera over the network */
class RAM_EXPORT NetworkRecorder : public Recorder
{
  public:
    /** Creates a record which broadcasts data using TCP on the given port */
    NetworkRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                    boost::uint16_t port, int policyArg = 0,
                    int recordWidth = 640, int recordHeight = 480);

    virtual ~NetworkRecorder();
    
  protected:
    /** Called whenever there is a frame to record, sends data over network */
    virtual void recordFrame(Image* image);
    
  private:
    class Connection : public boost::enable_shared_from_this<Connection>
    {
    public:
        typedef boost::shared_ptr<Connection> pointer;

        static pointer create(boost::asio::io_service& io);

        void sendImage(Image *img);

        boost::asio::ip::tcp::socket& socket();

    private:
        Connection(boost::asio::io_service& io);

        boost::asio::ip::tcp::socket m_socket;
    };

    void run_service()
    {
        io_service.run();
    }

    void start_accept();
    void handle_accept(Connection::pointer new_connection,
                       const boost::system::error_code& error);

    boost::asio::io_service io_service;

    boost::asio::ip::tcp::acceptor m_acceptor;
    
    /** Holds the width, height, and data size of the last image sent */
    boost::mutex m_lock;
    Image *m_buffer;

    boost::thread *m_bthread;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_NETWORKRECORDER_H_02_25_2008
