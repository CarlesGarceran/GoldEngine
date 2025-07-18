#include "../SDK.h"
#include "../CollisionType.h"
#include "../Triggers/Collider.h"
#include "BoxCollider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

ref class CreatePhysicsObjectWhenPhysicsServiceIsInitialized : public System::Collections::IEnumerator
{
	virtual property System::Object^ Current
	{
		System::Object^ get() sealed = System::Collections::IEnumerator::Current::get
		{
			throw gcnew System::NotSupportedException("Current not supported.");
		}
	}

	virtual bool MoveNext() sealed = System::Collections::IEnumerator::MoveNext
	{
		if (!Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
			return true;

		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)instance->getCollisionShape());

		if (collisionShape->getCollisonObject() != nullptr)
			return false;

		GameObject^ inst = collisionShape->getGameObject();

		float sX, sY, sZ;
		sX = inst->transform->scale->x;
		sY = inst->transform->scale->y;
		sZ = inst->transform->scale->z;

		btCollisionShape* btCs = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService()->getCollisionShapeForBox(sX, sY, sZ);
		collisionShape->createCollisionShape(btCs);

		collisionShape->createBulletObject();

		return false;
	}

	virtual void Reset() sealed = System::Collections::IEnumerator::Reset
	{
		throw gcnew System::NotSupportedException("Reset not supported.");
	}

private:
	GameObject^ instance;

public:
	CreatePhysicsObjectWhenPhysicsServiceIsInitialized(GameObject^ self)
	{
		this->instance = self;
	}
};

Engine::EngineObjects::Physics::BoxCollider::BoxCollider(String^ name, Engine::Internal::Components::Transform^ transform) : 
	Collider(name, transform)
{
	colliderShape = Enums::ColliderShape::Box;
	LaunchCoroutine(gcnew CreatePhysicsObjectWhenPhysicsServiceIsInitialized(this));
}

void Engine::EngineObjects::Physics::BoxCollider::Start()
{
	throw gcnew System::NotImplementedException();
}

void Engine::EngineObjects::Physics::BoxCollider::DrawGizmo()
{
	if (!renderWires)
		return;

	RAYLIB::DrawCubeWires(transform->position->toNative(), transform->scale->x, transform->scale->y, transform->scale->z, wireColor->toNative());
}

void Engine::EngineObjects::Physics::BoxCollider::Update()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (!registered && Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->AddCollisionObject(collisionShape->getCollisonObject());
		registered = true;
	}
}
