#pragma once
#include "../Includes.h"
#include "../GlIncludes.h"
#include "../ManagedIncludes.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

RAYLIB::Vector4 Engine::Components::Quaternion::toNative()
{
	return { this->x, this->y, this->z, this->w };
}

Engine::Components::Vector3^ Engine::Components::Quaternion::toEuler()
{
	return gcnew Engine::Components::Vector3(x, y, z);
}