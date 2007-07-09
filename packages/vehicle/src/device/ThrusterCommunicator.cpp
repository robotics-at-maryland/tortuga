/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/ThrusterCommunicator.cpp
 */

// STD Includes
#include <sstream>
#include <iostream>
#include <cstdio>

// UNIX Includes (needed for serial stuff)
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>

// Project Includes
#include "vehicle/include/device/ThrusterCommunicator.h"
#include "vehicle/include/device/ThrusterCommand.h"
#include "vehicle/include/device/Thruster.h"

namespace ram {
namespace vehicle {
namespace device {

static const int MOTOR_CMD_TIMEOUT = 1000;
static const char* MOTOR_CONTROLLER_DEV_FILE = "/dev/motor";
    
// Open the serial port and do some maybe needed linux foo
int openSerialPort(const char* devname);

int hasData(int fd, int timeout);
    
template<> ThrusterCommunicator* 
    pattern::Singleton<ThrusterCommunicator>::ms_Singleton = 0;
    
ThrusterCommunicator& ThrusterCommunicator::getSingleton()
{  
    assert(ms_Singleton && 
           "Singleton instance of ThrusterCommunicator not created");  
    return *ms_Singleton;    
}

ThrusterCommunicator* ThrusterCommunicator::getSingletonPtr()
{
    return ms_Singleton;
}  

ThrusterCommunicator::ThrusterCommunicator() :
    m_serialFD(-1)
{
    // Open and store my serial port FD here
    m_serialFD = openSerialPort(MOTOR_CONTROLLER_DEV_FILE);

    if (m_serialFD < 0)
        std::cout << "Could not open serial port" << std::endl;
    
    std::cout << "Thruster Communicator Created" << std::endl;
}

ThrusterCommunicator::~ThrusterCommunicator()
{
    // Process all pending messages, then shutdown message thread
    processCommands();
    
    unbackground(true);
    // close my serial port FD here
    std::cout << "Thruster Communicator Shutdown" << std::endl;
}
    
void ThrusterCommunicator::registerThruster(Thruster* thruster)
{
    if (0 == ThrusterCommunicator::getSingletonPtr())
    {
        // Create communicator singleton (Move me somewhere else?)
        new ThrusterCommunicator();
    }
    
    ThrusterCommunicator::getSingleton().addThruster(thruster);
}


void ThrusterCommunicator::unRegisterThruster(Thruster* thruster)
{
    ThrusterCommunicator::getSingleton().removeThruster(thruster);
}

void ThrusterCommunicator::sendThrusterCommand(ThrusterCommandPtr cmd)
{
    m_commandQueue.push(cmd);
}

void ThrusterCommunicator::update(double timestep)
{
    // Add any timed task here

    // Process all pending commands (there really shouldn't be any)     
    processCommands();
}
    
void ThrusterCommunicator::waitForUpdate(long microseconds)
{
    // This time format is UNIX dependent
    boost::xtime xtime;
    xtime.sec = 0;
    xtime.nsec = microseconds * 1000;

    // Wait for a new message
    ThrusterCommandPtr newCommand;
    if(m_commandQueue.popTimedWait(xtime, newCommand))
    {
        // Run the current command then process the rest
        runCommand(newCommand);
        processCommands();
    }
}
    
void ThrusterCommunicator::addThruster(Thruster* thruster)
{
    m_thrusters.insert(m_thrusters.begin(), thruster);
}

void ThrusterCommunicator::removeThruster(Thruster* thruster)
{
    ThrusterSetIter iter = m_thrusters.find(thruster);

    assert(iter != m_thrusters.end() && "Thruster not registered");
    m_thrusters.erase(iter);

    // If there are no more thrusters left, just delete the communicator
    if (0 == m_thrusters.size())
        delete this;
}

void ThrusterCommunicator::processCommands()
{
    ThrusterCommandPtr nextCommand;

    // Only process commands if the port is actually open
    if (m_serialFD >= 0)
    {
        // Runs every command untill the queue is empty
        while(m_commandQueue.popNoWait(nextCommand))
        {
            runCommand(nextCommand);
        }
    }
    // Port not open, attempt to open it up and reprocess commands, or just
    // drop them
    else
    {
        m_serialFD = openSerialPort(MOTOR_CONTROLLER_DEV_FILE);

        // Good connection, run commands
        if (m_serialFD >= 0)
        {
            processCommands();
        }
        // Bad connection, drop all commands
        else
        {
            while(m_commandQueue.popNoWait(nextCommand)) {}
        }
    }
}
    
void ThrusterCommunicator::runCommand(ThrusterCommandPtr command)
{
    // Only run serial command if port is open
    if (m_serialFD >= 0)
    {
        std::stringstream ss;

        ss << command->getCommandType() << command->getAddress()
           << command->getArgs() << " \r\n";
    
        std::cout << "Thruster command: " << ss.str() << std::endl;
        /// TODO: figure out what to do with a bad return type
        std::string cmd(ss.str());
        int ret = write(m_serialFD, cmd.c_str(), cmd.length());
	if (ret < 0)
            std::cout << "Write failure" << std::endl;

        usleep(100 * 1000);
/*	if(!hasData(m_serialFD, MOTOR_CMD_TIMEOUT))
	{
	    std::cout << "Motor controller not responding!!" << std::endl;
        }
        else
	{
	    int i;
            unsigned char buf[5]={0,0,0,0,0};
	  
	  
            for(i=0; i<4; i++)
	    {
                if(hasData(m_serialFD, MOTOR_CMD_TIMEOUT))
		{
                    int ret = read(m_serialFD, buf+i, 1);
		}
                else
                {
                    std::cout << "MC Not Responding" << std::endl;
                }
	    }

            std::cout << "Response: " << (const char*)&buf[0] << std::endl;

	  // Check buffer contains OK\n\r

        }
*/
    }
}

/** Checks for data waiting on the file descriptor  */
int hasData(int fd, int timeout)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    poll(&pfd, 1, timeout);

    return pfd.revents & POLLIN;
}

/* Some code from cutecom, which in turn may have come from minicom */
int openSerialPort(const char* devName)
{
   int fd = open(devName, O_RDWR, O_ASYNC); // | O_ASYNC); //, O_RDWR, O_NONBLOCK);

    if(fd == -1)
        return -1;

    //printf("FD=%d\n", fd);
    struct termios newtio;
    if (tcgetattr(fd, &newtio)!=0)
        printf("\nFirst stuff failed\n");

    unsigned int _baud=B19200;
    cfsetospeed(&newtio, _baud);
    cfsetispeed(&newtio, _baud);


    newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~(PARENB | PARODD);

    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag &= ~CSTOPB;

    newtio.c_iflag=IGNBRK;
    newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

    newtio.c_lflag=0;

    newtio.c_oflag=0;


    newtio.c_cc[VTIME]=1;
    newtio.c_cc[VMIN]=60;

//   tcflush(m_fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
        printf("tsetaddr1 failed!\n");

    int mcs=0;

    ioctl(fd, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(fd, TIOCMSET, &mcs);

    if (tcgetattr(fd, &newtio)!=0)
        printf("tcgetattr() 4 failed\n");

    newtio.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
      printf("tcsetattr() 2 failed\n");

    return fd;
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
