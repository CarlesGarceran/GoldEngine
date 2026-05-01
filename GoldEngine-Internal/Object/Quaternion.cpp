#pragma once
#include "../Includes.h"
#include "../GlIncludes.h"
#include "../ManagedIncludes.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

#include <cmath>
#include <stdexcept>

#pragma managed(push off)

inline btQuaternion& doQuaternion(float x, float y, float z, float w)
{
	return btQuaternion(x, y, z, w);
}

inline std::array<float, 4> getValues(btQuaternion quaternion)
{
	return { quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w() };
}

#pragma managed(pop)

constexpr float sPI = 3.14159265358979323846f;

Engine::Components::Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Engine::Components::Quaternion::Quaternion(Engine::Components::Vector3 axis, float angleRadians)
{
	float halfAngle = angleRadians * 0.5f;
	float s = std::sin(halfAngle);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	w = std::cos(halfAngle);
}

Engine::Components::Quaternion::Quaternion(RAYLIB::Quaternion quat)
{
	this->x = quat.x;
	this->y = quat.y;
	this->z = quat.z;
	this->w = quat.w;
}

bool Engine::Components::Quaternion::IsIdentity()
{
	return (x == 0 && y == 0 && z == 0 && w == 1);
}

void Engine::Components::Quaternion::Normalize()
{
	float mag = std::sqrt(x * x + y * y + z * z + w * w);
	if (mag == 0.0f)
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
		return;
	}

	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;
}

Engine::Components::Quaternion Engine::Components::Quaternion::Normalized()
{
	Quaternion q = *this;
	q.Normalize();
	return q;
}

Engine::Components::Quaternion Engine::Components::Quaternion::Conjugate()
{
	return Quaternion(-x, -y, -z, w);
}

Engine::Components::Quaternion Engine::Components::Quaternion::Inverse()
{
	float magSq = x * x + y * y + z * z + w * w;

	if (magSq < 1e-8f)
		return Quaternion::Identity;

	if (magSq == 0.0f)
		throw std::runtime_error("Cannot invert a zero quaternion");

	return Conjugate() * (1.0f / magSq);
}

float Engine::Components::Quaternion::Dot(Engine::Components::Quaternion q)
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}

Engine::Components::Vector3 Engine::Components::Quaternion::Rotate(Engine::Components::Vector3 vector)
{
	return Engine::Components::Quaternion::Rotate(*this, vector);
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator+(Engine::Components::Quaternion left, Engine::Components::Quaternion right)
{
	return Quaternion(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator-(Engine::Components::Quaternion left, Engine::Components::Quaternion right)
{
	return Quaternion(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator*(Engine::Components::Quaternion left, float scalar)
{
	return Engine::Components::Quaternion(left.x * scalar, left.y * scalar, left.z * scalar, left.w * scalar);
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator*(Engine::Components::Quaternion left, Engine::Components::Quaternion q)
{
	float w, x, y, z;
	w = left.w;
	x = left.x;
	y = left.y;
	z = left.z;

	return Engine::Components::Quaternion(
		w * q.x + x * q.w + y * q.z - z * q.y,  // x
		w * q.y - x * q.z + y * q.w + z * q.x,  // y
		w * q.z + x * q.y - y * q.x + z * q.w,  // z
		w * q.w - x * q.x - y * q.y - z * q.z   // w
	);
}

Engine::Components::Vector3 Engine::Components::Quaternion::operator*(Engine::Components::Vector3 left, Engine::Components::Quaternion right)
{
	Quaternion vecQuat = Quaternion(left.x, left.y, left.z, 0.0f);

	Quaternion resQuat = right * vecQuat * right.Inverse();

	return Engine::Components::Vector3(resQuat.x, resQuat.y, resQuat.z);
}

Engine::Components::Vector3 Engine::Components::Quaternion::operator*(Engine::Components::Quaternion left, Engine::Components::Vector3 right)
{
	Engine::Components::Vector3 qv(left.x, left.y, left.z);

	Engine::Components::Vector3 t = Engine::Components::Vector3::Cross(qv, right) * 2.0f;

	return right + (t * left.w) + Engine::Components::Vector3::Cross(qv, t);
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator/(Engine::Components::Quaternion left, Engine::Components::Quaternion right)
{
	return (left) * right.Inverse();
}

Engine::Components::Quaternion Engine::Components::Quaternion::operator/(Engine::Components::Quaternion left, float scalar)
{
	if (scalar == 0.0f)
		throw std::runtime_error("Cannot divide quaternion by zero scalar");

	return Quaternion(left.x / scalar, left.y / scalar, left.z / scalar, left.w / scalar);
}

RAYLIB::Quaternion Engine::Components::Quaternion::toNative()
{
	return RAYLIB::Quaternion({ x, y, z, w });
}

Engine::Components::Quaternion::operator Engine::Components::Vector3(Engine::Components::Quaternion quat)
{
	return quat.ToEulerAngles();
}

Engine::Components::Quaternion::operator System::Numerics::Quaternion(Engine::Components::Quaternion quaternion)
{
	return System::Numerics::Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

bool Engine::Components::Quaternion::Equals(System::Object^ obj)
{
	if (obj == nullptr) return false;
	if (obj->GetType() != Engine::Components::Quaternion::typeid) return false;

	Engine::Components::Quaternion other = (Engine::Components::Quaternion)obj;
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

System::Object^ Engine::Components::Quaternion::Clone()
{
	return Quaternion(x, y, z, w);
}

Engine::Components::Quaternion Engine::Components::Quaternion::Lerp(Quaternion a, Quaternion b, float t)
{
	return NLerp(a, b, t);
}

Engine::Components::Quaternion Engine::Components::Quaternion::NLerp(Quaternion a, Quaternion b, float t)
{
	// Ensure shortest path
	float dot = a.Dot(b);
	if (dot < 0.0f)
	{
		b = Quaternion(-b.x, -b.y, -b.z, -b.w);
	}

	// Linear interpolation
	Quaternion result(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t,
		a.w + (b.w - a.w) * t
	);

	// Normalize the result
	result.Normalize();
	return result;
}

Engine::Components::Quaternion Engine::Components::Quaternion::SLerp(Quaternion a, Quaternion b, float t)
{
	float dot = a.Dot(b);

	// Ensure shortest path
	if (dot < 0.0f)
	{
		dot = -dot;
		b = Quaternion(-b.x, -b.y, -b.z, -b.w);
	}

	// Clamp dot to avoid NaNs from acos
	if (dot > 1.0f) dot = 1.0f;
	if (dot < -1.0f) dot = -1.0f;

	float theta = std::acos(dot); // angle between quaternions
	float sinTheta = std::sin(theta);

	// If angle is very small, fallback to NLerp
	if (sinTheta < 0.001f)
	{
		return NLerp(a, b, t);
	}

	float ratioA = std::sin((1.0f - t) * theta) / sinTheta;
	float ratioB = std::sin(t * theta) / sinTheta;

	Quaternion result(
		a.x * ratioA + b.x * ratioB,
		a.y * ratioA + b.y * ratioB,
		a.z * ratioA + b.z * ratioB,
		a.w * ratioA + b.w * ratioB
	);

	return result;
}

Engine::Components::Vector3 Engine::Components::Quaternion::Rotate(Engine::Components::Quaternion q, Engine::Components::Vector3 v)
{
	Quaternion qv(v.x, v.y, v.z, 0.0f);

	Quaternion qn = q.Normalized();
	
	if (std::isnan(qn.x) || std::isnan(qn.w))
	{
		return v;
	}

	Quaternion result = qn * qv * qn.Conjugate();

	return Engine::Components::Vector3(result.x, result.y, result.z);
}

Engine::Components::Quaternion Engine::Components::Quaternion::LookRotation(Engine::Components::Vector3 forward, Engine::Components::Vector3 up)
{
	Vector3 f = forward.Normalized();

	if (System::Math::Abs(Vector3::Dot(f, up)) > 0.999f)
		up = Vector3(0, 0, 1);

	Vector3 r = up.Cross(f).Normalized();
	Vector3 u = f.Cross(r);

	// COLUMN-MAJOR BASIS (right, up, forward)
	float m00 = r.x, m01 = u.x, m02 = f.x;
	float m10 = r.y, m11 = u.y, m12 = f.y;
	float m20 = r.z, m21 = u.z, m22 = f.z;

	float trace = m00 + m11 + m22;
	Quaternion q;

	if (trace > 0.0f)
	{
		float s = std::sqrt(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (m21 - m12) / s;
		q.y = (m02 - m20) / s;
		q.z = (m10 - m01) / s;
	}
	else if (m00 > m11 && m00 > m22)
	{
		float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
		q.w = (m21 - m12) / s;
		q.x = 0.25f * s;
		q.y = (m01 + m10) / s;
		q.z = (m02 + m20) / s;
	}
	else if (m11 > m22)
	{
		float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
		q.w = (m02 - m20) / s;
		q.x = (m01 + m10) / s;
		q.y = 0.25f * s;
		q.z = (m12 + m21) / s;
	}
	else
	{
		float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f;
		q.w = (m10 - m01) / s;
		q.x = (m02 + m20) / s;
		q.y = (m12 + m21) / s;
		q.z = 0.25f * s;
	}

	q.Normalize();
	return q;
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromAxis(Engine::Components::Vector3 axis, float angleRadians)
{
	Vector3 normAxis = axis.Normalized();
	float halfAngle = angleRadians * 0.5f;
	float s = std::sin(halfAngle);

	return Quaternion(
		normAxis.x * s,
		normAxis.y * s,
		normAxis.z * s,
		std::cos(halfAngle)
	);
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromAxis(RAYLIB::Vector3 axis, float angleRadians)
{
	RAYMATH::Vector3 normAxis = RAYMATH::Vector3Normalize(axis);
	float halfAngle = angleRadians * 0.5f;
	float s = std::sin(halfAngle);

	return Quaternion(
		normAxis.x * s,
		normAxis.y * s,
		normAxis.z * s,
		std::cos(halfAngle)
	);
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerRadians(RAYLIB::Vector3 euler)
{
	float pitch = euler.x;
	float yaw = euler.y;
	float roll = euler.z;

	float cy = cos(yaw * 0.5f);
	float sy = sin(yaw * 0.5f);
	float cp = cos(pitch * 0.5f);
	float sp = sin(pitch * 0.5f);
	float cr = cos(roll * 0.5f);
	float sr = sin(roll * 0.5f);


	Quaternion q;

	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sp * cy * cr + cp * sy * sr;
	q.y = cp * sy * cr - sp * cy * sr;
	q.z = cp * cy * sr - sp * sy * cr;

	return q;
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerRadians(Engine::Components::Vector3 euler)
{

	float pitch = euler.x;
	float yaw = euler.y;
	float roll = euler.z;

	float cy = std::cos(yaw * 0.5f);
	float sy = std::sin(yaw * 0.5f);
	float cp = std::cos(pitch * 0.5f);
	float sp = std::sin(pitch * 0.5f);
	float cr = std::cos(roll * 0.5f);
	float sr = std::sin(roll * 0.5f);

	Quaternion q;

	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sp * cy * cr + cp * sy * sr;
	q.y = cp * sy * cr - sp * cy * sr;
	q.z = cp * cy * sr - sp * sy * cr;

	return q;
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerRadians(float x, float y, float z)
{
	float pitch = x;
	float yaw = y;
	float roll = z;

	float cy = std::cos(yaw * 0.5f);
	float sy = std::sin(yaw * 0.5f);
	float cp = std::cos(pitch * 0.5f);
	float sp = std::sin(pitch * 0.5f);
	float cr = std::cos(roll * 0.5f);
	float sr = std::sin(roll * 0.5f);

	Quaternion q;

	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sp * cy * cr + cp * sy * sr;
	q.y = cp * sy * cr - sp * cy * sr;
	q.z = cp * cy * sr - sp * sy * cr;

	return q;
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerAngles(RAYLIB::Vector3 euler)
{
	RAYMATH::Vector3 v3 = {
		euler.x * DEG2RAD,
		euler.y * DEG2RAD,
		euler.z * DEG2RAD
	};

	return FromEulerRadians(v3);
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerAngles(Engine::Components::Vector3 euler)
{
	return FromEulerRadians(euler * DEG2RAD);
}

Engine::Components::Quaternion Engine::Components::Quaternion::FromEulerAngles(float x, float y, float z)
{
	return FromEulerRadians(x * DEG2RAD, y * DEG2RAD, z * DEG2RAD);
}

Engine::Components::Vector3 Engine::Components::Quaternion::p_toEuler()
{
	Vector3 euler;

	// Roll (x-axis)
	float sinr_cosp = 2 * (w * x + y * z);
	float cosr_cosp = 1 - 2 * (x * x + y * y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (y-axis)
	float sinp = 2 * (w * y - z * x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(sPI / 2, sinp);
	else
		euler.y = std::asin(sinp);

	// Yaw (z-axis)
	float siny_cosp = 2 * (w * z + x * y);
	float cosy_cosp = 1 - 2 * (y * y + z * z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	return euler;
}

Engine::Components::Vector3 Engine::Components::Quaternion::ToEuler()
{
	return (p_toEuler() * RAD2DEG);
}

Engine::Components::Vector3 Engine::Components::Quaternion::ToEulerRadians()
{
	return p_toEuler();
}

Engine::Components::Vector3 Engine::Components::Quaternion::ToEulerAngles()
{
	return ToEuler();
}

bool Engine::Components::Quaternion::operator==(Engine::Components::Quaternion left, Engine::Components::Quaternion right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

bool Engine::Components::Quaternion::operator==(const Engine::Components::Quaternion& left, const Engine::Components::Quaternion& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}

bool Engine::Components::Quaternion::operator!=(Engine::Components::Quaternion left, Engine::Components::Quaternion other)
{
	return !other.Equals(left);
}

float Engine::Components::Quaternion::Length::get()
{
	return std::sqrt(x * x + y * y + z * z + w * w);
}

float Engine::Components::Quaternion::LengthSquared::get()
{
	return x * x + y * y + z * z + w * w;
}