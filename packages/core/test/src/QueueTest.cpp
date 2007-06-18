/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Original Author: Paul Bridger <paulbridger.net>
 * Brought in by: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ReadWriteMutex.h
 */

// Library Includes
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

// Project Includes
#include "core/include/ThreadedQueue.h"

using namespace ram::core;

// Our global shared object
ThreadedQueue<char> producedChars;
// Number of chars to produce
const int NUM_CHARS = 10000;

// Function declarations for producer and consumer
void dataProducer();
void dataConsumer();

int main()
{
    boost::thread consumer(&dataConsumer);
    boost::thread producer(&dataProducer);

    consumer.join();
    producer.join();
    
    return 0;
}

// To sync the iostream
boost::mutex io_mutex;

// Producers characters and puts them on the queue
void dataProducer()
{
    unsigned char c = 'A';
    for(uint i = 0; i < NUM_CHARS;)
    {
        char out = 0;
        if((c >= 'A' && c <= 'Z'))
        {
            out = c;
            producedChars.push(c++);
            ++i;
        }
        else
        {
            c = 'A';
            out = c;
        }

        // Print out what we just made in a thread safe manner
        {
            //boost::mutex::scoped_lock lock(io_mutex);
            std::cout << "Produced: " << (char)out << std::endl;
        }
    }

    producedChars.push(EOF);
}

// Remove thread from the queue
void dataConsumer()
{
    while(true)
    {
        char got = producedChars.popWait();
        if (got == EOF)
            return;

        // Print out what we just made in a thread safe manner
        {
            //  boost::mutex::scoped_lock lock(io_mutex);
            std::cout << "Consumed: " << got << std::endl;
        }
    }
}
