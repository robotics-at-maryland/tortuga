
// Project Includes
#include "logging/include/Serialize.h"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_GUID(ram::core::Event, "ram.core.Event")
BOOST_CLASS_EXPORT_GUID(ram::core::StringEvent, "ram.core.StringEvent")

#ifdef RAM_WITH_MATH
BOOST_CLASS_EXPORT_GUID(ram::math::OrientationEvent, "ram.math.OrientationEvent")
BOOST_CLASS_EXPORT_GUID(ram::math::Vector3Event, "ram.math.Vector3Event")
BOOST_CLASS_EXPORT_GUID(ram::math::NumericEvent, "ram.math.NumericEvent")
#endif // RAM_WITH_MATH

#ifdef RAM_WITH_VISION
BOOST_CLASS_EXPORT_GUID(ram::vision::ImageEvent, "ram.vision.ImageEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::RedLightEvent, "ram.vision.RedLightEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::PipeEvent, "ram.vision.PipeEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::BinEvent, "ram.vision.BinEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::DuctEvent, "ram.vision.DuctEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::SafeEvent, "ram.vision.SafeEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::TargetEvent, "ram.vision.TargetEvent")
BOOST_CLASS_EXPORT_GUID(ram::vision::BarbedWireEvent, "ram.vision.BarbedWireEvent")
#endif // RAM_WITH_VISION

#ifdef RAM_WITH_VEHICLE
BOOST_CLASS_EXPORT_GUID(ram::vehicle::PowerSourceEvent, "ram.vehicle.PowerSourceEvent")
BOOST_CLASS_EXPORT_GUID(ram::vehicle::TempSensorEvent, "ram.vehicle.TempSensorEvent")
BOOST_CLASS_EXPORT_GUID(ram::vehicle::ThrusterEvent, "ram.vehicle.ThrusterEvent")
BOOST_CLASS_EXPORT_GUID(ram::vehicle::SonarEvent, "ram.vehicle.SonarEvent")
#endif // RAM_WITH_VEHICLE

#ifdef RAM_WITH_CONTROL
BOOST_CLASS_EXPORT_GUID(ram::control::ParamSetupEvent, "ram.control.ParamSetupEvent")
BOOST_CLASS_EXPORT_GUID(ram::control::ParamUpdateEvent, "ram.control.ParamUpdateEvent")
#endif // RAM_WITH_CONTROL
