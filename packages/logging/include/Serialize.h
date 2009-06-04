/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 John Edmonds <pocketcookies2@gmail.com>
 * All rights reserved.
 *
 * Author: John Edmonds <pocketcookies2@gmail.com>
 * File:  packages/logging/include/Serialize.h"
 */


#ifndef RAM_LOGGING_SERIALIZE_H_03_05_2009
#define RAM_LOGGING_SERIALIZE_H_03_05_2009

// Library Includes
#include<boost/serialization/serialization.hpp>
#include<boost/serialization/split_free.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/shared_ptr.hpp>

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/EventPublisherRegistry.h"
#include "core/include/Events.h"

#include "vision/include/Events.h"


namespace ram {
namespace logging {

// ------------------------------------------------------------------------- //
//                     T Y P E   R E G I S T R A T I O N                     //
// ------------------------------------------------------------------------- //

template <class Archive>
void registerTypes(Archive& ar)
{
    ar.register_type(static_cast<ram::vision::RedLightEvent*>(NULL));
}
    
    
} // logging
} // ram


namespace boost {
namespace serialization {

// ------------------------------------------------------------------------- //
//                          B A S E   C L A S S E S                          //
// ------------------------------------------------------------------------- //

    
template<class Archive>
void save(Archive& ar, const ram::core::Event& t, unsigned int version)
{
    ar & t.type;
    ar & t.timeStamp;

    // Lookup the name of the sending publisher
    std::string name = "UNNAMED";
    if (t.sender)
        name = t.sender->getPublisherName();
    ar & name;
}
    
template<class Archive>
void load(Archive& ar, ram::core::Event& t, unsigned int version)
{
    ar & t.type;
    ar & t.timeStamp;

    // Read back the name of the sending publisher and assign back to an
    // existing publishing if possible
    std::string name;
    ar & name;
    t.sender = ram::core::EventPublisherRegistry::lookupByName(name);
}

// This wraps up the use of the different save and load functionality
template<class Archive>
void serialize(Archive & ar, ram::core::Event& t,
               const unsigned int file_version)
{
    split_free(ar, t, file_version); 
}
    
BOOST_SERIALIZATION_SHARED_PTR(ram::core::Event)

// ------------------------------------------------------------------------- //
//                           C O R E   E V E N T S                           //
// ------------------------------------------------------------------------- //

template <class Archive>
void serialize(Archive &ar, ram::core::StringEvent &t,
               const unsigned int file_version)
{ 
  ar & boost::serialization::base_object<ram::core::Event>(t);
  ar & t.string;
}


// ------------------------------------------------------------------------- //
//                         V I S I O N   E V E N T S                         //
// ------------------------------------------------------------------------- //

// Not currently serialized
//template <class Archive>
//void serialize(Archive &ar, ram::vision::ImageEvent &t,
//               const unsigned int file_version)
//{
  //ar & t->image;
//}
    
template <class Archive>
void serialize(Archive &ar, ram::vision::RedLightEvent &t,
               const unsigned int file_version)
{ 
  ar & boost::serialization::base_object<ram::core::Event>(t);
  ar & t.y;
  ar & (*((double*)(&t.azimuth)));
  ar & (*((double*)(&t.elevation)));
  ar & t.range;
  ar & t.x;
  ar & t.y;
  ar & t.pixCount;
}

BOOST_SERIALIZATION_SHARED_PTR(ram::vision::RedLightEvent)

template <class Archive>
void serialize(Archive &ar, ram::vision::PipeEventPtr &t,
               const unsigned int file_version)
{
  ar & t->y;
  ar & t->x;
  ar & t->y;
  ar & t->angle;
}

BOOST_SERIALIZATION_SHARED_PTR(ram::vision::PipeEvent)    
    
template <class Archive>
void serialize(Archive &ar, ram::vision::BinEventPtr &t, 
               const unsigned int file_version)
{
  ar & t->y;
  ar & t->y;
  ar & t->x;
  ar & t->y;
  ar & t->id;
  ar & t->suit;
  ar & t->angle;
}

BOOST_SERIALIZATION_SHARED_PTR(ram::vision::BinEvent)
    
template <class Archive>
void serialize(Archive &ar, ram::vision::DuctEventPtr &t, 
               const unsigned int file_version)
{
  printf("Calling serialize for DuctEvent.\n");
  ar & t->y;
  ar & t->x;
  ar & t->range;
  ar & t->alignment;
  ar & t->aligned;
  ar & t->visible;
}

BOOST_SERIALIZATION_SHARED_PTR(ram::vision::DuctEvent)
    
template <class Archive>
void serialize(Archive &ar, ram::vision::SafeEventPtr &t, 
               const unsigned int file_version)
{
  ar & t->x;
  ar & t->y;
}

BOOST_SERIALIZATION_SHARED_PTR(ram::vision::SafeEventPtr)
    
} // serialization
} // boost

#endif // RAM_LOGGING_SERIALIZE_H_03_05_2009
