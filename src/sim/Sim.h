#ifndef SIM_H
#define SIM_H

// Core

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Vector4.h"
#include "math/include/Matrix2.h"
#include "math/include/Matrix3.h"
#include "math/include/Matrix4.h"
#include "math/include/Quaternion.h"

typedef ram::math::impl::Vector2 Vec2;
typedef ram::math::impl::Vector3 Vec3;
typedef ram::math::impl::Vector4 Vec4;
typedef ram::math::impl::Matrix2 Mat2;
typedef ram::math::impl::Matrix3 Mat3;
typedef ram::math::impl::Matrix4 Mat4;
typedef ram::math::impl::Quaternion Quat;

// Simulation

#include "vehicle/SimVehicleFactory.h"

// Graphics

#include "Shader.h"

#endif