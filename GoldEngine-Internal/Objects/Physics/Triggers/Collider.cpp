#include "../SDK.h"
#include "../CollisionType.h"
#include "Collider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#include "../../Abstract/Renderer.h"

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

Collider::Collider(String^ name, Engine::Internal::Components::Transform^ transform)
	: Engine::EngineObjects::Script(name, transform)
{
	this->wireColor = gcnew Engine::Components::Color(0xFF00FF00);
	this->renderWires = true;
}

Engine::EngineObjects::Physics::Collider::Collider()
	: Engine::EngineObjects::Script()
{
	this->wireColor = gcnew Engine::Components::Color(0xFF00FF00);
	this->renderWires = true;
}

void Collider::Start()
{
	if (Parent != nullptr && Parent->IsA<Engine::EngineObjects::Geometry::Abstract::Renderer^>() && FindFirstSibling<Collider^>() != this)
	{
		Destroy(this);
		printError("A renderer cannot have multiple instances of a collider");
		return;
	}
}

void Collider::Update()
{
	if (Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Engine::EngineObjects::Physics::PhysicsService^ physService = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance;

		Engine::Native::CollisionShape* collisionShape =
			(root != nullptr) ? (Engine::Native::CollisionShape*)root->getCollisionShape()
			: (Engine::Native::CollisionShape*)this->getCollisionShape();

		if (collisionShape->hasCollisionObject())
		{
			btBroadphaseProxy* proxy = collisionShape->getCollisionObject()->getBroadphaseHandle();
			if (proxy != nullptr)
			{
				std::lock_guard(physService->RequestMutex());

				proxy->m_collisionFilterGroup = physService->GetCollisionGroup(this->layerMask);
				proxy->m_collisionFilterMask = physService->GetCollisionMask(this->layerMask);
			}
		}

	}
}

void Engine::EngineObjects::Physics::Collider::OnCollisionEnter(GameObject^ instance)
{
	if (instance == root || instance == this) return;

	if (this->collisionType == ColliderType::Trigger)
		Parent->OnTriggerEnter(instance);
	else
		Parent->OnCollisionEnter(instance);

	HitBegin->raiseExecution(gcnew cli::array<System::Object^> { instance });
}

void Engine::EngineObjects::Physics::Collider::OnCollisionStay(GameObject^ instance)
{
	if (instance == root || instance == this) return;

	if (this->collisionType == ColliderType::Trigger)
		Parent->OnTriggerStay(instance);
	else
		Parent->OnCollisionStay(instance);

	Hit->raiseExecution(gcnew cli::array<System::Object^> { instance });
}

void Engine::EngineObjects::Physics::Collider::OnCollisionExit(GameObject^ instance)
{
	if (instance == root || instance == this) return;

	if (this->collisionType == ColliderType::Trigger)
		Parent->OnTriggerExit(instance);
	else
		Parent->OnCollisionExit(instance);


	HitEnded->raiseExecution(gcnew cli::array<System::Object^> { instance });
}

void Engine::EngineObjects::Physics::Collider::OnCollided(GameObject^ instance)
{
	if (instance == root || instance == this) return;

	if (this->collisionType == ColliderType::Trigger)
		Parent->OnTriggered(instance);
	else
		Parent->OnCollided(instance);

	Hit->raiseExecution(gcnew cli::array<System::Object^> { instance });
}

void Collider::DrawGizmo()
{
}

#endif