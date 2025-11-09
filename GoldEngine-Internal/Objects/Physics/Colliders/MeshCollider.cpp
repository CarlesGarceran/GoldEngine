#include "../SDK.h"

#ifdef USE_BULLET_PHYS

#include "../CollisionType.h"
#include "../Triggers/Collider.h"
#include "MeshCollider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#include "../../Abstract/Renderer.h"
#include "../../MeshRenderer/MeshRenderer.h"
#include "../../ModelRenderer/ModelRenderer.hpp"

using namespace Engine::EngineObjects::Geometry;
using namespace Engine::EngineObjects::Geometry::Abstract;
using namespace Engine::EngineObjects::Physics::Native;

UNMANAGED_BEGIN

#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btConvexPolyhedron.h>

inline std::array<RAYLIB::Vector3, 2> GetEdge(btConvexHullShape* shape, int i)
{
	btVector3 pa(0, 0, 0);
	btVector3 pb(0, 0, 0);

	shape->getEdge(i, pa, pb);
	
	return { 
		RAYLIB::Vector3{ pa.x(), pa.y(), pa.z() },
		RAYLIB::Vector3{ pb.x(), pb.y(), pb.z() }
	};
}

void SetShapeScaling(btCollisionObject* object, std::array<float, 3> scale)
{
	object->getCollisionShape()->setLocalScaling({ scale[0], scale[1], scale[2] });
}

void _setCollisionShape(Engine::Native::CollisionShape* hookedShape, RAYLIB::Mesh& meshInstance, int collisionType)
{
	if (hookedShape == nullptr) return;

	btCollisionShape* collisionShape = NativeSingleton<NativePhysicsService>::Get().getCollisionShapeFromMesh(meshInstance, collisionType);
	
	hookedShape->createCollisionShape(collisionShape);
}

UNMANAGED_END

void SetCollisionShape(GameObject^ Instance, Engine::EngineObjects::Physics::Enums::MeshCollisionType meshCollisionType)
{
	if (Instance == nullptr) return;

	if (Instance->Parent != nullptr && Instance->Parent->IsA<Renderer^>())
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)Instance->getCollisionShape());
		Engine::Native::CollisionShape* collisionShape2 = ((Engine::Native::CollisionShape*)Instance->Parent->getCollisionShape());

		Renderer^ renderer = Instance->Parent->As<Renderer^>();

		int meshIndex = 0;

		if (renderer->IsA<MeshRenderer^>())
		{
			meshIndex = renderer->As<MeshRenderer^>()->meshIndex;

			if (meshIndex > (*renderer->As<MeshRenderer^>()->GetModel()).meshCount)
				return;
		}
		else if (renderer->IsA<ModelRenderer^>())
		{
			meshIndex = 0;

			if (meshIndex > (*renderer->As<ModelRenderer^>()->GetModel()).meshCount)
				return;
		}

		_setCollisionShape(collisionShape, (*renderer->GetModel()).meshes[meshIndex], (int)meshCollisionType);
		_setCollisionShape(collisionShape2, (*renderer->GetModel()).meshes[meshIndex], (int)meshCollisionType);
	}
}

Engine::EngineObjects::Physics::MeshCollider::MeshCollider(String^ name, Engine::Internal::Components::Transform^ transform) :
	Collider(name, transform)
{
	colliderShape = Enums::ColliderShape::Mesh;

	if (Parent != nullptr) 
	{
		transform->position = Parent->transform->position;
	}
}

Engine::EngineObjects::Physics::MeshCollider::MeshCollider() :
	Collider()
{
	colliderShape = Enums::ColliderShape::Mesh;
}

void Engine::EngineObjects::Physics::MeshCollider::Awake()
{
	if (this->attributes->hasAttribute("meshCollisionType"))
		this->attributes->getAttribute("meshCollisionType")->onPropertyChanged->connect(gcnew Action<Engine::EngineObjects::Physics::Enums::MeshCollisionType, Engine::EngineObjects::Physics::Enums::MeshCollisionType>(this, &MeshCollider::OnMeshCollisionTypeChanged));

	if (this->attributes->hasAttribute("collisionType"))
		this->attributes->getAttribute("collisionType")->onPropertyChanged->connect(gcnew Action<Engine::EngineObjects::Physics::Enums::ColliderType, Engine::EngineObjects::Physics::Enums::ColliderType>(this, &MeshCollider::OnCollisionTypeChanged));

	this->onPropertyChanged->connect(gcnew Action<String^, System::Object^, System::Object^>(this, &MeshCollider::OnParentChanged));
}

void Engine::EngineObjects::Physics::MeshCollider::Start()
{
	Collider::Start();

	if (root != nullptr && root->IsA<RigidBody^>() && !root->As<RigidBody^>()->Kinematic && meshCollisionType == Enums::MeshCollisionType::Concave)
	{
		printError("A non kinematic rigidbody can't use a concave collider.");
		meshCollisionType = Enums::MeshCollisionType::Convex;
		registered = false;
	}

	if (!registered && Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

		SetCollisionShape(this, this->meshCollisionType);

		if (this->collisionType == Enums::ColliderType::Trigger)
			collisionShape->createBulletGhostObject(true);
		else
			collisionShape->createBulletObject(true);

		registered = true;

		if (Parent != nullptr)
		{
			this->originalCollisionShape = (Engine::Native::CollisionShape*)Parent->getCollisionShape();
			Parent->setCollisionShape(this->getCollisionShape());
		}
	}
}

void Engine::EngineObjects::Physics::MeshCollider::DrawGizmo()
{
	if (!renderWires) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (Parent != nullptr && Parent->IsA<Renderer^>())
	{
		btCollisionObject* collisionObject = collisionShape->getCollisionObject();

		if(meshCollisionType == Enums::MeshCollisionType::Concave)
		{
			RAYLIB::Model& model = *Parent->As<Renderer^>()->GetModel();
			int meshIndex = 0;

			if (Parent->IsA<MeshRenderer^>())
				meshIndex = Parent->As<MeshRenderer^>()->meshIndex;

			RAYLIB::Mesh& mesh = model.meshes[meshIndex];

			RAYLIB::DrawModelWiresEx(
				model,
				transform->position.toNative(),
				transform->rotation.toNative(),
				0.0f,
				transform->scale.toNative(),
				wireColor->toNative()
			);
		}
		else if (meshCollisionType == Enums::MeshCollisionType::Convex) 
		{
			Engine::Native::CollisionShape* collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();
			btConvexHullShape* convexHullShape = (btConvexHullShape*)collisionShape->getCollisionShape();

			int numVerts = convexHullShape->getNumEdges();

			for (int x = 0; x < numVerts; x++)
			{
				std::array<RAYLIB::Vector3, 2> vtxs = GetEdge(convexHullShape, x);

				RAYLIB::Vector3 vtx = vtxs[0];
				RAYLIB::Vector3 vtx1 = vtxs[1];

				RAYLIB::Vector3 start = {
					static_cast<float>(vtx.x) * Parent->transform->scale.x,
					static_cast<float>(vtx.y) * Parent->transform->scale.y,
					static_cast<float>(vtx.z) * Parent->transform->scale.z
				};

				RAYLIB::Vector3 end = {
					static_cast<float>(vtx1.x) * Parent->transform->scale.x,
					static_cast<float>(vtx1.y) * Parent->transform->scale.y,
					static_cast<float>(vtx1.z) * Parent->transform->scale.z
				};

				start.x += Parent->transform->position.x;
				start.y += Parent->transform->position.y;
				start.z += Parent->transform->position.z;

				end.x += Parent->transform->position.x;
				end.y += Parent->transform->position.y;
				end.z += Parent->transform->position.z;

				RAYLIB::DrawLine3D(start, end, wireColor->toNative());
			}
		}
	}
}

void Engine::EngineObjects::Physics::MeshCollider::Destroy()
{
	Parent->setCollisionShape(this->originalCollisionShape);
}

bool Engine::EngineObjects::Physics::MeshCollider::IsOwned()
{
	return (root != nullptr);
}

bool Engine::EngineObjects::Physics::MeshCollider::ClaimOwnership(GameObject^ instance)
{
	if (!instance->IsA<Engine::EngineObjects::Physics::RigidBody^>())
		return false;

	if (root == instance)
		return false;

	root = instance;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());
	SetCollisionShape(this, this->meshCollisionType);

	if (registered && collisionShape->getCollisionObject() != nullptr)
	{
		Singleton<PhysicsService^>::Instance->RemoveCollisionObject(collisionShape->getCollisionObject());
		registered = false;
	}


	return true;
}

void Engine::EngineObjects::Physics::MeshCollider::CreateShape()
{
	SetCollisionShape(this, this->meshCollisionType);
	root = nullptr;
}

void Engine::EngineObjects::Physics::MeshCollider::Disown()
{
	root = nullptr;
	registered = false;
}

void Engine::EngineObjects::Physics::MeshCollider::OnCollisionTypeChanged(Enums::ColliderType newType, Enums::ColliderType oldType)
{
	if (newType == oldType) return;

	root = nullptr;
}

void Engine::EngineObjects::Physics::MeshCollider::OnOriginChanged(Engine::Components::Vector3 newOrigin, Engine::Components::Vector3 oldOrigin)
{

}

void Engine::EngineObjects::Physics::MeshCollider::OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform)
{
	if (property == "parent")
	{
		this->transform->position = ((Engine::Internal::Components::Transform^)transform)->position;

		root = nullptr;
	}
}

void Engine::EngineObjects::Physics::MeshCollider::OnMeshCollisionTypeChanged(Engine::EngineObjects::Physics::Enums::MeshCollisionType newValue, Engine::EngineObjects::Physics::Enums::MeshCollisionType oldValue)
{
	if (newValue == oldValue) return;

	if (root != nullptr && root->IsA<RigidBody^>() && newValue == Enums::MeshCollisionType::Concave)
	{
		printError("A non kinematic rigidbody can't use a concave collider.");
		newValue = Enums::MeshCollisionType::Convex;
		return;
	}

	root = nullptr;
}

void Engine::EngineObjects::Physics::MeshCollider::Update()
{
	Collider::Update();

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (root != nullptr && root->IsA<RigidBody^>() && !root->As<RigidBody^>()->Kinematic && meshCollisionType == Enums::MeshCollisionType::Concave)
	{
		printError("A non kinematic rigidbody can't use a concave collider.");
		meshCollisionType = Enums::MeshCollisionType::Convex;
		registered = false;
	}

	if (!registered && root == nullptr)
	{
		SetCollisionShape(this, this->meshCollisionType);

		if (this->collisionType == Enums::ColliderType::Trigger)
			collisionShape->createBulletGhostObject(true);
		else
			collisionShape->createBulletObject(true);

		registered = true;

		if (Parent != nullptr && this->originalCollisionShape == nullptr)
		{
			this->originalCollisionShape = (Engine::Native::CollisionShape*)Parent->getCollisionShape();
			Parent->setCollisionShape(this->getCollisionShape());
		}
	}

	if (collisionShape->getCollisionObject() != nullptr)
	{
		SetShapeScaling(collisionShape->getCollisionObject(), { transform->scale.x, transform->scale.y, transform->scale.z });
	}

	if (Parent != nullptr)
	{
		auto collisionObject = collisionShape->getCollisionObject();

		Engine::EngineObjects::Physics::Native::updateCollisionObject(
			collisionObject, 
			{ transform->position.x, transform->position.y, transform->position.z }, 
			{ transform->rotation.x, transform->rotation.y, transform->rotation.z }
		);
	}

	if (root)
	{
		transform->position = root->transform->position;
	}
}

#endif