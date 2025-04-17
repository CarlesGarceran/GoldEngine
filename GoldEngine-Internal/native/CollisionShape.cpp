#include "../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionShape.h"
#include "../Objects/Physics/CollisionType.h"
#include "../Objects/Physics/Native/NativePhysicsService.h"
#include "../Objects/Physics/RigidBody.h"
#include "../Objects/Physics/PhysicsService.h"

#define INSTANCE_CHECK() if (this == nullptr) return nullptr;
#define OBJECT_CHECK() if (this->collisionObject == nullptr) return nullptr;
#define SHAPE_CHECK() if (this->collisionShape == nullptr) return nullptr;

using namespace Engine::Native;
using namespace System::Runtime::InteropServices;

void onCollisionShapeDeleted(btCollisionShape* shape)
{
	delete shape;
}

void onCollisionObjectDeleted(btCollisionObject* object)
{
	Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->RemoveCollisionObject(object);
	delete object;
}

CollisionShape::CollisionShape(Engine::Internal::Components::GameObject^ userPtr)
{
	this->handle = GCHandle::Alloc(userPtr);
	this->userHandler = static_cast<void*>(GCHandle::ToIntPtr(handle).ToPointer());
	this->gameObject = msclr::gcroot(userPtr);
	this->collisionShape = nullptr;
	this->collisionObject = nullptr;
}

CollisionShape::~CollisionShape()
{
	this->handle.Free();
	delete collisionObject;
	delete collisionShape;
}

void CollisionShape::createCollisionShape(btCollisionShape* shape)
{
	if (this->collisionShape == nullptr)
		this->collisionShape = new Engine::Native::EnginePtr<btCollisionShape*>(shape, &onCollisionShapeDeleted);
	else
	{
		this->collisionShape->setInstance(shape);
	}
}

void CollisionShape::createBulletObject()
{
	try
	{
		this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(new btCollisionObject(), &onCollisionObjectDeleted);

		this->collisionObject->getInstance()->setCollisionShape(this->collisionShape->getInstance());
		this->collisionObject->getInstance()->setUserPointer(this->userHandler);

		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(this->collisionObject->getInstance());
	}
	catch (Exception^ ex)
	{
		// print the fancy message
		printError(ex->Message);
	}
}

btCollisionObject* CollisionShape::getCollisonObject()
{
	INSTANCE_CHECK();
	OBJECT_CHECK();

	return this->collisionObject->getInstance();
}

btCollisionShape* CollisionShape::getCollisionShape()
{
	INSTANCE_CHECK();
	SHAPE_CHECK();

	return this->collisionShape->getInstance();
}


GameObject^ CollisionShape::getGameObject()
{
	INSTANCE_CHECK();

	return this->gameObject;
}
#endif