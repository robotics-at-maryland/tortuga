
// Library Includes
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// Project Includes
#include "logging/include/Serialize.h"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(ram::core::Event)
BOOST_CLASS_EXPORT(ram::core::StringEvent)

#ifdef RAM_WITH_MATH
BOOST_CLASS_EXPORT(ram::math::OrientationEvent)
BOOST_CLASS_EXPORT(ram::math::Vector3Event)
BOOST_CLASS_EXPORT(ram::math::Vector2Event)
BOOST_CLASS_EXPORT(ram::math::NumericEvent)
#endif // RAM_WITH_MATH

#ifdef RAM_WITH_VISION
BOOST_CLASS_EXPORT(ram::vision::ImageEvent)
BOOST_CLASS_EXPORT(ram::vision::RedLightEvent)
BOOST_CLASS_EXPORT(ram::vision::PipeEvent)
BOOST_CLASS_EXPORT(ram::vision::BinEvent)
BOOST_CLASS_EXPORT(ram::vision::DuctEvent)
BOOST_CLASS_EXPORT(ram::vision::SafeEvent)
BOOST_CLASS_EXPORT(ram::vision::TargetEvent)
BOOST_CLASS_EXPORT(ram::vision::BarbedWireEvent)
#endif // RAM_WITH_VISION

#ifdef RAM_WITH_VEHICLE
BOOST_CLASS_EXPORT(ram::vehicle::PowerSourceEvent)
BOOST_CLASS_EXPORT(ram::vehicle::TempSensorEvent)
BOOST_CLASS_EXPORT(ram::vehicle::ThrusterEvent)
BOOST_CLASS_EXPORT(ram::vehicle::SonarEvent)
BOOST_CLASS_EXPORT(ram::vehicle::RawIMUDataEvent)
BOOST_CLASS_EXPORT(ram::vehicle::RawDVLDataEvent)
BOOST_CLASS_EXPORT(ram::vehicle::RawDepthSensorDataEvent)
#endif // RAM_WITH_VEHICLE

#ifdef RAM_WITH_CONTROL
BOOST_CLASS_EXPORT(ram::control::ParamSetupEvent)
BOOST_CLASS_EXPORT(ram::control::ParamUpdateEvent)
#endif // RAM_WITH_CONTROL
