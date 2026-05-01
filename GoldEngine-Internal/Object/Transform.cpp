#include "Transform.h"
#include "../ManagedIncludes.h"
#include "../Event.h"
#include "GameObject.h"
#include "../SDK.h"

using namespace System;
using namespace Engine::Internal::Components;

Engine::Internal::Components::Transform::Transform(Engine::Components::Vector3 position, Engine::Components::Quaternion rotation, Engine::Components::Vector3 scale, Transform^ parent)
{
	this->uid = System::Guid::NewGuid();
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->parent = parent;
}

System::Guid Engine::Internal::Components::Transform::GetGUID()
{
	return this->uid;
}

String^ Engine::Internal::Components::Transform::GetUID()
{
	return uid.ToString();
}

void Engine::Internal::Components::Transform::SetUID(String^ uid)
{
	this->uid = System::Guid(uid);
}

void Engine::Internal::Components::Transform::SetGUID(System::Guid uid)
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
	return (T)Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(this->GetUID());
}

System::Object^ Engine::Internal::Components::Transform::GetObject()
{
	return Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(this->GetUID());
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::InverseTransformPoint(Engine::Components::Vector3 worldPoint)
{
	Engine::Components::Vector3 v = worldPoint - this->position;

	v = this->rotation.Inverse() * v;

	v.x /= this->scale.x;
	v.y /= this->scale.y;
	v.z /= this->scale.z;

	return v;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::position::get()
{
	return worldPosition;
}

void Engine::Internal::Components::Transform::position::set(Engine::Components::Vector3 value)
{
	worldPosition = value;
}

Engine::Components::Quaternion Engine::Internal::Components::Transform::rotation::get()
{
	return worldRotation;
}

void Engine::Internal::Components::Transform::rotation::set(Engine::Components::Quaternion value)
{
	worldRotation = value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::localPosition::get()
{
	auto parent = GetParent();
	if (parent == nullptr)
		return worldPosition;

	return worldPosition - parent->position;
}

void Engine::Internal::Components::Transform::localPosition::set(Engine::Components::Vector3 value)
{
	if (GetParent() == nullptr)
	{
		worldPosition = value;
		return;
	}

	auto parent = GetParent();

	worldPosition = parent->position + value;
}

Engine::Components::Quaternion Engine::Internal::Components::Transform::localRotation::get()
{
	if (GetParent() == nullptr)
		return worldRotation;

	return GetParent()->rotation.Inverse() * worldRotation;
}

void Engine::Internal::Components::Transform::localRotation::set(Engine::Components::Quaternion value)
{
	if (GetParent() == nullptr) 
	{
		worldRotation = value;
		return;
	}

	worldRotation = GetParent()->rotation * value;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::forward::get()
{
	Engine::Components::Vector3 worldForward = FRONT;
	return rotation.Rotate(worldForward).Normalized();
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::right::get()
{
	return Engine::Components::Vector3::Cross(forward, up).Normalized();

	/*
	Engine::Components::Vector3 worldRight = RIGHT;
	return rotation.Rotate(worldRight).Normalized() * -1; // Cheap fix cause otherwise it will be flipped, somehow.
	*/
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::up::get()
{
	Engine::Components::Vector3 worldUp = TOP;
	return rotation.Rotate(worldUp).Normalized();
}

Engine::Internal::Components::Transform^ Engine::Internal::Components::Transform::parent::get()
{
	if (this->parentCache == nullptr && parentUid != Guid::Empty)
	{
		if (Singleton<Engine::Scripting::ObjectManager^>::Instantiated)
		{
			auto obj = Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjectByUid(parentUid.ToString());
			if (obj != nullptr) parentCache = obj->transform;
		}
	}

	return this->parentCache;
}

void Engine::Internal::Components::Transform::parent::set(Engine::Internal::Components::Transform^ value)
{
	parentCache = value;
	parentUid = (value != nullptr) ? value->GetGUID() : Guid::Empty;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::scale::get()
{
	return worldScale;
}

void Engine::Internal::Components::Transform::scale::set(Engine::Components::Vector3 v)
{
	this->worldScale = v;
}

Engine::Components::Vector3 Engine::Internal::Components::Transform::localScale::get()
{
	if (GetParent() != nullptr)
		return worldScale / GetParent()->worldScale; // component-wise division
	else
		return worldScale;
}

void Engine::Internal::Components::Transform::localScale::set(Engine::Components::Vector3 value)
{
	if (GetParent() != nullptr)
		worldScale = value * GetParent()->worldScale; // component-wise multiplication
	else
		worldScale = value;
}

Engine::Components::Matrix16^ Engine::Internal::Components::Transform::WorldMatrix::get()
{
	Engine::Components::Matrix16^ local = Engine::Components::Matrix16::FromTRS(localPosition, localRotation, localScale);

	if (parent != nullptr)
	{
		return Matrix16::MatrixMultiply(parent->WorldMatrix, local);
	}

	return local;
}
