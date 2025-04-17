#include "../Includes.h"
#include "../ManagedIncludes.h"
#include "../GlIncludes.h"
#include "Vector2.h"
#include "Vector3.h"

using namespace Newtonsoft::Json;

[[JsonConstructorAttribute]]
Engine::Components::Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Engine::Components::Vector3::Vector3()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
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

RAYLIB::Color Engine::Components::Vector3::toColor()
{
	return GetColor(x + y + z);
}

RAYLIB::Vector3 Engine::Components::Vector3::toNative()
{
	return { x,y,z };
}

Engine::Components::Vector3^ Engine::Components::Vector3::add(Vector3^ origin)
{
	Vector3^ copy = Vector3::Create();
	copy->copy(this);

	copy->x += origin->x;
	copy->y += origin->y;
	copy->z += origin->z;
	
	return copy;
}

Engine::Components::Vector3^ Engine::Components::Vector3::multiply(Vector3^ origin)
{
	Vector3^ copy = Vector3::Create();
	copy->copy(this);

	copy->x *= origin->x;
	copy->y *= origin->y;
	copy->z *= origin->z;

	return copy;
}

Engine::Components::Vector3^ Engine::Components::Vector3::divide(Vector3^ origin)
{
	Vector3^ copy = Vector3::Create();
	copy->copy(this);

	copy->x /= origin->x;
	copy->y /= origin->y;
	copy->z /= origin->z;

	return this;
}

Engine::Components::Vector3^ Engine::Components::Vector3::sub(Vector3^ origin)
{
	Vector3^ copy = Vector3::Create();
	copy->copy(this);

	copy->x -= origin->x;
	copy->y -= origin->y;
	copy->z -= origin->z;

	return copy;
}

void Engine::Components::Vector3::copy(const Vector3^ origin)
{
	this->x = origin->x;
	this->y = origin->y;
	this->z = origin->z;
}

Engine::Components::Vector2^ Engine::Components::Vector3::toVector2()
{
	return gcnew Engine::Components::Vector2(x, y);
}

System::Numerics::Vector3^ Engine::Components::Vector3::toNumericsVector3()
{
	return gcnew System::Numerics::Vector3(this->x, this->y, this->z);
}