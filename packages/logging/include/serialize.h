#ifndef LOGGING_SERIALIZE_H
#define LOGGING_SERIALIZE_H

#include<boost/serialization/serialization.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include<fstream>

#include "vision/include/Events.h"

namespace boost{
namespace serialization{

template <class Archive> void serialize(Archive &ar, ram::vision::ImageEventPtr &t, const unsigned int file_version)
{
  ar & t->image;
}
template <class Archive> void serialize(Archive &ar, ram::vision::RedLightEventPtr &t, const unsigned int file_version)
{
  ar & t->y;
  ar & t->azimuth;
  ar & t->elevation;
  ar & t->range;
  ar & t->x;
  ar & t->y;
  ar & t->pixCount;
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

#endif
