#include "../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionShape.h"
#include "../Objects/Physics/CollisionType.h"
#include "../Objects/Physics/Native/NativePhysicsService.h"
#include "../Objects/Physics/RigidBody.h"
#include "../Objects/Physics/PhysicsService.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#define INSTANCE_CHECK(retvalue) if (this == nullptr) return retvalue;
#define OBJECT_CHECK(retvalue) if (this->collisionObject == nullptr) return retvalue;
#define SHAPE_CHECK(retvalue) if (this->collisionShape == nullptr) return retvalue;

using namespace Engine::Native;
using namespace System::Runtime::InteropServices;

void onCollisionShapeDeleted(btCollisionShape* shape)
{
	if (shape == nullptr) return;

	delete shape;
}

void onCollisionObjectDeleted(btCollisionObject* object)
{
	if (object == nullptr) return;

	if (object->getInternalType() == btCollisionObject::CollisionObjectTypes::CO_RIGID_BODY)
	{
		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->RemovePhysicsObject((btRigidBody*)object);
	}
	else
	{
		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->RemoveCollisionObject(object);
	}

	delete object;
}

CollisionShape::CollisionShape(Engine::Internal::Components::GameObject^ userPtr)
{
	this->handle = GCHandle::Alloc(userPtr);
	this->userHandler = GCHandle::ToIntPtr(handle).ToPointer();
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
	if (shape == nullptr) return;

	if (this->collisionShape == nullptr)
	{
		this->collisionShape = new Engine::Native::EnginePtr<btCollisionShape*>(shape, &onCollisionShapeDeleted, &onCollisionShapeDeleted);
	}
	else
	{
		this->collisionShape->setInstanceRef(shape);
	}

	if (hasCollisionObject())
	{
		collisionObject->getInstance()->setCollisionShape(collisionShape->getInstance());
		resampleAABB();
	}
}

void CollisionShape::createBulletObject(bool uploadToService)
{
	try
	{
		if (collisionObject == nullptr)
		{
			this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(new btCollisionObject(), onCollisionObjectDeleted, onCollisionObjectDeleted);
		}
		else
		{
			btCollisionObject* collisionObject = new btCollisionObject();
			this->collisionObject->setInstanceRef(collisionObject);
		}

		this->collisionObject->getInstance()->setCollisionShape(this->collisionShape->getInstance());
		this->collisionObject->getInstance()->setUserPointer(this->userHandler);

		if (uploadToService) // if flagged it will report the object automatically to the physics service, otherwise it will just allocate it, useful for creating physObjects and not reporting them instantly.
			Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(this->collisionObject->getInstance());
	}
	catch (std::exception ex)
	{
		printError(gcnew String(ex.what()));
	}
	catch (Exception^ ex)
	{
		// print the fancy message
		printError(ex->Message);
	}
}

void Engine::Native::CollisionShape::createBulletGhostObject(bool uploadToService)
{
	try
	{
		if (this->collisionObject == nullptr)
		{
			this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(new btGhostObject(), &onCollisionObjectDeleted, &onCollisionObjectDeleted);
		}
		else
		{
			btCollisionObject* collisionObject = new btGhostObject();
			this->collisionObject->setInstanceRef(collisionObject);
		}

		this->collisionObject->getInstance()->setCollisionFlags(this->collisionObject->getInstance()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		this->collisionObject->getInstance()->setCollisionShape(this->collisionShape->getInstance());
		this->collisionObject->getInstance()->setUserPointer(this->userHandler);

		if (uploadToService) // if flagged it will report the object automatically to the physics service, otherwise it will just allocate it, useful for creating physObjects and not reporting them instantly.
			Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(this->collisionObject->getInstance());
	}
	catch (std::exception ex)
	{
		printError(gcnew String(ex.what()));
	}
	catch (Exception^ ex)
	{
		// print the fancy message
		printError(ex->Message);
	}
}

void Engine::Native::CollisionShape::setCollisionObject(btRigidBody* collisionObject, bool uploadToService)
{
	try
	{
		if (this->collisionObject == nullptr)
		{
			this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(collisionObject, &onCollisionObjectDeleted, &onCollisionObjectDeleted);
		}
		else
		{
			this->collisionObject->setInstanceRef((btCollisionObject*&)collisionObject);
		}

		if(this->collisionObject->getInstance()->getCollisionShape() == nullptr)
			this->collisionObject->getInstance()->setCollisionShape(this->collisionShape->getInstance());

		this->collisionObject->getInstance()->setUserPointer(this->userHandler);

		if (uploadToService) // if flagged it will report the object automatically to the physics service, otherwise it will just allocate it, useful for creating physObjects and not reporting them instantly.
			Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddPhysicsObject(collisionObject);
	}
	catch (std::exception ex)
	{
		printError(gcnew String(ex.what()));
	}
	catch (Exception^ ex)
	{
		// print the fancy message
		printError(ex->Message);
	}
}

void Engine::Native::CollisionShape::setCollisionObject(btCollisionObject* collisionObject, bool uploadToService, bool bindDeleter)
{
	try
	{
		if (collisionObject == nullptr)
		{
			if (bindDeleter)
			{
				this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(collisionObject, &onCollisionObjectDeleted, &onCollisionObjectDeleted);
			}
			else
			{
				this->collisionObject = new Engine::Native::EnginePtr<btCollisionObject*>(collisionObject, nullptr, nullptr);
			}
		}
		else
		{
			this->collisionObject->setInstanceRef(collisionObject);
		}

		this->collisionObject->getInstance()->setCollisionShape(this->collisionShape->getInstance());
		this->collisionObject->getInstance()->setUserPointer(this->userHandler);

		if (uploadToService) // if flagged it will report the object automatically to the physics service, otherwise it will just allocate it, useful for creating physObjects and not reporting them instantly.
			Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(this->collisionObject->getInstance());
	}
	catch (std::exception ex)
	{
		printError(gcnew String(ex.what()));
	}
	catch (Exception^ ex)
	{
		// print the fancy message
		printError(ex->Message);
	}
}

btCollisionObject*& CollisionShape::getCollisionObject()
{
	return this->collisionObject->getInstance();
}

btCollisionShape*& CollisionShape::getCollisionShape()
{
	return this->collisionShape->getInstance();
}

bool Engine::Native::CollisionShape::hasCollisionObject()
{
	if (this->collisionObject == nullptr)
		return false;

	return this->collisionObject->isLoaded();
}

bool Engine::Native::CollisionShape::hasCollisionShape()
{
	if (this->collisionShape == nullptr)
		return false;

	return this->collisionShape->isLoaded();
}

void Engine::Native::CollisionShape::resampleAABB()
{
	Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->resampleAABB();
}

void Engine::Native::CollisionShape::freeCollisionObject()
{
	INSTANCE_CHECK();
	OBJECT_CHECK();

	if (!hasCollisionObject()) return;

	collisionObject->free();
}


GameObject^ CollisionShape::getGameObject()
{
	INSTANCE_CHECK(nullptr);

	return this->gameObject;
}

void Engine::Native::CollisionShape::setGameObject(GameObject^ instance)
{
	this->handle = GCHandle::Alloc(instance);
	this->userHandler = GCHandle::ToIntPtr(handle).ToPointer();
	this->gameObject = msclr::gcroot(instance);

	if (this->getCollisionObject() != nullptr)
		this->collisionObject->getInstance()->setUserPointer(this->userHandler);
}
#endif