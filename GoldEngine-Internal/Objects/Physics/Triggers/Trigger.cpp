#include "../SDK.h"
#include "Trigger.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../CollisionType.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#ifdef USE_BULLET_PHYS

#pragma managed(push, off)

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

btBoxShape* createBoxShape(float x, float y, float z)
{
	btBoxShape* boxShape = new btBoxShape({ 0,0,0 });
	boxShape->setLocalScaling(btVector3(x, y, z));
	return boxShape;
}


void UpdateSizeExtents(btBoxShape* boxShape, float size[3])
{
	boxShape->setLocalScaling(btVector3(size[0], size[1], size[2]));
}

#pragma managed(pop)

using namespace Engine::EngineObjects::Physics;

void Trigger::onColliderShapeChanged(ColliderShape newShape, ColliderShape oldShape)
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
		collisionShape->createCollisionShape(createBoxShape(sX, sY, sZ));
		break;
	}

	collisionShape->createBulletObject();
}

Trigger::Trigger(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
{
	this->colliderShape = ColliderShape::Box;
	this->wireColor = gcnew Engine::Components::Color(0xFFFFFFFF);
	this->renderWires = true;
}


void Trigger::Start()
{
	onColliderShapeChanged(colliderShape, colliderShape);
	this->attributes->getAttribute("colliderShape")->onPropertyChanged->connect(gcnew Action<ColliderShape, ColliderShape>(this, &Trigger::onColliderShapeChanged));
}

void Trigger::Update()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (colliderShape == ColliderShape::Box)
	{
		float size[3] = { transform->scale->x, transform->scale->y, transform->scale->z };
		UpdateSizeExtents((btBoxShape*)collisionShape->getCollisionShape(), size);
	}
}

void Trigger::DrawGizmo()
{
	if (!renderWires)
		return;

	if (colliderShape == ColliderShape::Box)
	{
		RAYLIB::DrawCubeWires(transform->position->toNative(), transform->scale->x, transform->scale->y, transform->scale->z, wireColor->toNative());
	}
	else if (colliderShape == ColliderShape::Sphere)
	{
		RAYLIB::DrawSphereWires(transform->position->toNative(), transform->scale->x, transform->scale->x, transform->scale->x, wireColor->toNative());
	}
}

#endif