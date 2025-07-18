#include "../SDK.h"
#include "../CollisionType.h"
#include "Collider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#ifdef USE_BULLET_PHYS

#pragma managed(push, off)

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

void UpdateSizeExtents(btBoxShape* boxShape, float size[3])
{
	boxShape->setLocalScaling(btVector3(size[0], size[1], size[2]));
}

#pragma managed(pop)

using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Enums;

void Collider::onColliderShapeChanged(ColliderShape newShape, ColliderShape oldShape)
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (collisionShape->getCollisonObject() != nullptr && (newShape == oldShape))
		return;

	GameObject^ object = collisionShape->getGameObject();

	float sX = object->transform->scale->x;
	float sY = object->transform->scale->y;
	float sZ = object->transform->scale->z;

	switch (newShape)
	{
	case ColliderShape::Box:
		{
			btCollisionShape* cs = Singleton<PhysicsService^>::Instance->getNativePhysicsService()->getCollisionShapeForBox(sX, sY, sZ);
			collisionShape->createCollisionShape(cs);
			break;
		}
	}

	collisionShape->createBulletObject();
}

Collider::Collider(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
{
	this->wireColor = gcnew Engine::Components::Color(0xFFFFFFFF);
	this->renderWires = true;
}

void Collider::Start()
{
	onColliderShapeChanged(colliderShape, colliderShape);
	this->attributes->getAttribute("colliderShape")->onPropertyChanged->connect(gcnew Action<ColliderShape, ColliderShape>(this, &Collider::onColliderShapeChanged));

	if (Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());
		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(collisionShape->getCollisonObject());
		registered = true;
	}
}

void Collider::Update()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (!registered && Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(collisionShape->getCollisonObject());
		registered = true;
	}

	if (colliderShape == ColliderShape::Box)
	{
		float size[3] = { transform->scale->x, transform->scale->y, transform->scale->z };
		UpdateSizeExtents((btBoxShape*)collisionShape->getCollisionShape(), size);
	}
}

void Collider::DrawGizmo()
{
}

#endif