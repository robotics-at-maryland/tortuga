#ifndef LOGGING_SERIALIZE_H
#define LOGGING_SERIALIZE_H

#include<boost/serialization/serialization.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/shared_ptr.hpp>

#include<fstream>

#include "vision/include/Events.h"

namespace boost{
namespace serialization{

template <class Archive>
void serialize(Archive &ar, ram::core::Event &t,
               const unsigned int file_version)
{
}

template <class Archive>
void serialize(Archive &ar, ram::vision::ImageEvent &t,
               const unsigned int file_version)
{
  //ar & t->image;
}
    
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
template <class Archive> void serialize(Archive &ar, ram::vision::PipeEventPtr &t, const unsigned int file_version)
{
  ar & t->y;
  ar & t->x;
  ar & t->y;
  ar & t->angle;
}
template <class Archive> void serialize(Archive &ar, ram::vision::BinEventPtr &t, const unsigned int file_version)
{
  ar & t->y;
  ar & t->y;
  ar & t->x;
  ar & t->y;
  ar & t->id;
  ar & t->suit;
  ar & t->angle;
}
template <class Archive> void serialize(Archive &ar, ram::vision::DuctEventPtr &t, const unsigned int file_version)
{
  printf("Calling serialize for DuctEvent.\n");
  ar & t->y;
  ar & t->x;
  ar & t->range;
  ar & t->alignment;
  ar & t->aligned;
  ar & t->visible;
}
template <class Archive> void serialize(Archive &ar, ram::vision::SafeEventPtr &t, const unsigned int file_version)
{
  ar & t->x;
  ar & t->y;
}
} // serialization
} // boost

BOOST_SERIALIZATION_SHARED_PTR(ram::core::Event)
BOOST_SERIALIZATION_SHARED_PTR(ram::vision::RedLightEvent)

#endif
