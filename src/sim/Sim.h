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

typedef ram::math::Vector2 Vec2;
typedef ram::math::Vector3 Vec3;
typedef ram::math::Vector4 Vec4;
typedef ram::math::Matrix2 Mat2;
typedef ram::math::Matrix3 Mat3;
typedef ram::math::Matrix4 Mat4;
typedef ram::math::Quaternion Quat;

// Simulation

#include "vehicle/SimVehicleFactory.h"

// Graphics

#include "Shader.h"

#endif