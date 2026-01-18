#include "Transform.h"
#include "../ManagedIncludes.h"
#include "../Event.h"
#include "GameObject.h"
#include "../SDK.h"

using namespace System;
using namespace Engine::Internal::Components;

Engine::Internal::Components::Transform::Transform(Engine::Components::Vector3 position, Engine::Components::Vector3 rotation, Engine::Components::Vector3 scale, Transform^ parent)
{
	this->uid = System::Guid::NewGuid().ToString();
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->parent = parent;
}

String^ Engine::Internal::Components::Transform::GetUID()
{
	return uid;
}

void Engine::Internal::Components::Transform::SetUID(String^ uid)
{
	this->uid = uid;
}

void Engine::Internal::Components::Transform::setParent(Transform^ newTransform)
{
	this->parent = newTransform;
}

Engine::Internal::Components::Transform^ Engine::Internal::Components::Transform::getParent()
{
	return parent;
}

generic <class T>
T Engine::Internal::Components::Transform::GetObject()
{
	return (T)Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(this->uid);
}

System::Object^ Engine::Internal::Components::Transform::GetObject()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(this->uid);
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::position::get()
{
	return worldPosition;
}

void Engine::Internal::Components::Transform::position::set(Engine::Components::Vector3 value)
{
	worldPosition = value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::rotation::get()
{
	return worldRotation;
}

void Engine::Internal::Components::Transform::rotation::set(Engine::Components::Vector3 value)
{
	worldRotation = value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::localPosition::get()
{
	if (GetParent() == nullptr)
		return worldPosition;

	return GetParent()->position - worldPosition;
}

void Engine::Internal::Components::Transform::localPosition::set(Engine::Components::Vector3 value)
{
	if (GetParent() == nullptr)
		worldPosition = value;

	worldPosition = GetParent()->position + value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::localRotation::get()
{
	if (GetParent() == nullptr)
		return worldRotation;

	return GetParent()->rotation - worldRotation;
}

void Engine::Internal::Components::Transform::localRotation::set(Engine::Components::Vector3 value)
{
	if (GetParent() == nullptr)
		worldRotation = value;

	worldRotation = GetParent()->rotation + value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::forward::get()
{
	float pitch = rotation.x * PI / 180.0f;
	float yaw = rotation.y * PI / 180.0f;

	float x = sin(yaw) * cos(pitch);
	float y = sin(pitch);
	float z = cos(yaw) * cos(pitch);

	return Engine::Components::Vector3(x, y, z);
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::right::get()
{
	float pitch = rotation.x * (PI / 180.0f);
	float yaw = rotation.y * (PI / 180.0f);
	float roll = rotation.z * (PI / 180.0f);

	float x = cos(yaw) * cos(roll) + sin(yaw) * sin(pitch) * sin(roll);
	float y = cos(pitch) * sin(roll);
	float z = sin(yaw) * cos(roll) - cos(yaw) * sin(pitch) * sin(roll);

	return Engine::Components::Vector3(x, y, z);
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::up::get()
{
	float pitch = rotation.x * (PI / 180.0f);
	float yaw = rotation.y * (PI / 180.0f);
	float roll = rotation.z * (PI / 180.0f);

	float x = -cos(yaw) * sin(roll) + sin(yaw) * sin(pitch) * cos(roll);
	float y = cos(pitch) * cos(roll);
	float z = -sin(yaw) * sin(roll) - cos(yaw) * sin(pitch) * cos(roll);

	return Engine::Components::Vector3(x, y, z);
}
