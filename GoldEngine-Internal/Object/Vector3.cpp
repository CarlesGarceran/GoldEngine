#include "../Includes.h"
#include "../ManagedIncludes.h"
#include "../GlIncludes.h"
#include "Vector2.h"
#include "Vector3.h"

using namespace Newtonsoft::Json;

Engine::Components::Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Engine::Components::Vector3::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Engine::Components::Vector3::Set(float* v)
{
	this->x = v[0];
	this->y = v[1];
	this->z = v[2];
}

void Engine::Components::Vector3::Set(RAYLIB::Vector3 v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

RAYLIB::Color Engine::Components::Vector3::toColor()
{
	return GetColor(x + y + z);
}

RAYLIB::Vector3 Engine::Components::Vector3::toNative()
{
	return { x,y,z };
}

Engine::Components::Vector3 Engine::Components::Vector3::add(Vector3 origin)
{
	return *this + origin;
}

Engine::Components::Vector3 Engine::Components::Vector3::multiply(Vector3 origin)
{
	return *this * origin;
}

Engine::Components::Vector3 Engine::Components::Vector3::divide(Vector3 origin)
{
	return *this / origin;
}

Engine::Components::Vector3 Engine::Components::Vector3::sub(Vector3 origin)
{
	return *this - origin;
}

float Engine::Components::Vector3::angle(Vector3 left, Vector3 right)
{
	return RAYMATH::Vector3Angle({ left.x, left.y, left.z }, { right.x, right.y, right.z });
}

Engine::Components::Vector3 Engine::Components::Vector3::cross(Vector3 left, Vector3 right)
{
	RAYLIB::Vector3 product = RAYMATH::Vector3CrossProduct({ left.x, left.y, left.z }, { right.x, right.y, right.z });
	return Engine::Components::Vector3(product.x, product.y, product.z);
}

Engine::Components::Vector3 Engine::Components::Vector3::lerp(Vector3 origin, Vector3 target, float interpolate)
{
	auto newX = RAYMATH::Lerp(origin.x, target.x, interpolate);
	auto newY = RAYMATH::Lerp(origin.y, target.y, interpolate);
	auto newZ = RAYMATH::Lerp(origin.z, target.z, interpolate);

	return Vector3(newX, newY, newZ);
}

inline void Engine::Components::Vector3::Lerp(Vector3 target, float interpolate)
{
	float newX, newY, newZ;

	newX = RAYMATH::Lerp(x, target.x, interpolate);
	newY = RAYMATH::Lerp(y, target.y, interpolate);
	newZ = RAYMATH::Lerp(z, target.z, interpolate);

	this->Set(newX, newY, newZ);
}

float Engine::Components::Vector3::Dot(Engine::Components::Vector3 left, Engine::Components::Vector3 right)
{
	return (RAYMATH::Vector3DotProduct(left.toNative(), right.toNative()));
}

float Engine::Components::Vector3::DistanceScalar(Engine::Components::Vector3 left, Engine::Components::Vector3 right)
{
	return RAYMATH::Vector3Distance(left.toNative(), right.toNative());
}

Engine::Components::Vector3 Engine::Components::Vector3::Direction(Engine::Components::Vector3 from, Engine::Components::Vector3 to)
{
	return to - from;
}

Engine::Components::Vector3 Engine::Components::Vector3::DirectionNormalized(Engine::Components::Vector3 from, Engine::Components::Vector3 to)
{
	return (to - from).Normalized();
}

Engine::Components::Vector3 Engine::Components::Vector3::Normalized()
{
	return Engine::Components::Vector3::create(
		RAYMATH::Vector3Normalize(
			this->toNative()
		)
	);
}

float Engine::Components::Vector3::Distance(Vector3 left, Vector3 right)
{
	return DistanceScalar(left, right);
}

Engine::Components::Vector2 Engine::Components::Vector3::toVector2()
{
	return Engine::Components::Vector2(x, y);
}

System::Numerics::Vector3 Engine::Components::Vector3::toNumericsVector3()
{
	return System::Numerics::Vector3(this->x, this->y, this->z);
}