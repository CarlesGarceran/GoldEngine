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

inline void SetShapeScaling(btCollisionObject* object, std::array<float, 3> scale)
{
	object->getCollisionShape()->setLocalScaling({ scale[0], scale[1], scale[2] });
}

inline void _setCollisionShape(NativePhysicsService nativePhysService, Engine::Native::CollisionShape* hookedShape, RAYLIB::Mesh& meshInstance, int collisionType, std::array<float, 3> offset)
{
	if (hookedShape == nullptr) return;

	btCollisionShape* collisionShape = nativePhysService.getCollisionShapeFromMesh(meshInstance, collisionType);
	btCompoundShape* compoundShape = new btCompoundShape();

	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin({ offset[0], offset[1], offset[2] });
	compoundShape->addChildShape(localTransform, collisionShape);

	hookedShape->createCollisionShape(compoundShape);
}

UNMANAGED_END

using PhysicsService = Engine::EngineObjects::Physics::PhysicsService;

void SetCollisionShape(GameObject^ Instance, Engine::EngineObjects::Physics::Enums::MeshCollisionType meshCollisionType, Engine::Components::Vector3 origin)
{
	if (Instance == nullptr) return;

	PhysicsService^ phys = Singleton<PhysicsService^>::Instance;

	if (Instance->Parent != nullptr && Instance->Parent->IsA<Renderer^>())
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)Instance->getCollisionShape());

		Renderer^ renderer = Instance->Parent->As<Renderer^>();

		int meshIndex = 0;

		if (renderer->IsA<MeshRenderer^>())
		{
			meshIndex = renderer->As<MeshRenderer^>()->meshIndex;

			if (meshIndex > (*renderer->As<MeshRenderer^>()->GetModelPtr()).meshCount)
				return;
		}
		else if (renderer->IsA<ModelRenderer^>())
		{
			meshIndex = 0;

			if (meshIndex > (*renderer->As<ModelRenderer^>()->GetModelPtr()).meshCount)
				return;
		}

		_setCollisionShape(*phys->getNativePhysicsService(), collisionShape, renderer->GetModelPtr()->meshes[meshIndex], (int)meshCollisionType, { origin.x, origin.y, origin.z });
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

		if (!collisionShape->hasCollisionShape())
			SetCollisionShape(this, meshCollisionType, origin);

		if (this->collisionType == Enums::ColliderType::Trigger)
			collisionShape->createBulletGhostObject(true);
		else
			collisionShape->createBulletObject(true);

		registered = true;
	}
}

void Engine::EngineObjects::Physics::MeshCollider::DrawGizmo()
{
	if (!renderWires) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (Parent != nullptr && Parent->IsA<Renderer^>())
	{
		btCollisionObject* collisionObject = collisionShape->getCollisionObject();
		if (!collisionShape->hasCollisionObject() || !collisionShape->hasCollisionShape()) return;

		if(meshCollisionType == Enums::MeshCollisionType::Concave)
		{
			RAYLIB::Model* model = Parent->As<Renderer^>()->GetModelPtr();
			
			RAYLIB::DrawModelWiresEx(
				*model,
				(transform->position + (transform->localPosition * -1)).toNative(),
				transform->rotation.ToEulerAngles().toNative(),
				0.0f,
				transform->scale.toNative(),
				wireColor->toNative()
			);
		}
		else if (meshCollisionType == Enums::MeshCollisionType::Convex) 
		{
			Engine::Native::CollisionShape* collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();
			btCompoundShape* compoundShape = (btCompoundShape*)collisionShape->getCollisionShape();
			btConvexHullShape* convexHullShape = (btConvexHullShape*)compoundShape->getChildShape(0);

			int numVerts = convexHullShape->getNumEdges();

			for (int x = 0; x < numVerts; x++)
			{
				std::array<RAYLIB::Vector3, 2> vtxs = GetEdge(convexHullShape, x);

				RAYLIB::Vector3 vtx = vtxs[0];
				RAYLIB::Vector3 vtx1 = vtxs[1];

				Engine::Components::Vector3 offsetted = transform->position + origin;

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

				start.x += offsetted.x;
				start.y += offsetted.y;
				start.z += offsetted.z;

				end.x += offsetted.x;
				end.y += offsetted.y;
				end.z += offsetted.z;

				RAYLIB::DrawLine3D(start, end, wireColor->toNative());
			}
		}
	}
}

void Engine::EngineObjects::Physics::MeshCollider::Destroy()
{
	if (root)
	{
		if (root->IsA<RigidBody^>()) root->As<RigidBody^>()->DisposedShape();
	}

	if (Parent != nullptr)
	{
		Parent->restoreCollisionShape();
	}
}

bool Engine::EngineObjects::Physics::MeshCollider::IsOwned()
{
	return (root != nullptr);
}

bool Engine::EngineObjects::Physics::MeshCollider::ClaimOwnership(GameObject^ instance)
{
	if (!instance->IsA<Engine::EngineObjects::Physics::RigidBody^>())
		return false;

	Engine::Native::CollisionShape* collisionShape = this->getCollisionShape();

	if (root == instance) return true;

	root = instance;

	if (!collisionShape->hasCollisionShape())
	{
		SetCollisionShape(this, this->meshCollisionType, origin);
	}

	if (registered && collisionShape->getCollisionObject() != nullptr)
	{
		Singleton<PhysicsService^>::Instance->RemoveCollisionObject(collisionShape->getCollisionObject());
		registered = false;
	}

	if (root != nullptr)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (_collisionShape->hasCollisionObject())
		{
			btCollisionObject*& collisionObject = _collisionShape->getCollisionObject();
			collisionObject->setCollisionShape(collisionShape->getCollisionShape());

			collisionShape->setCollisionObject(collisionObject, false, false);

			if (this->collisionType == Enums::ColliderType::Trigger)
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			else
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		if (collisionShape->hasCollisionObject() && registered)
		{
			Singleton<PhysicsService^>::Instance->RemoveCollisionObject(collisionShape->getCollisionObject());
			collisionShape->freeCollisionObject();
		}

		collisionShape->resampleAABB();
	}

	return true;
}

void Engine::EngineObjects::Physics::MeshCollider::CreateShape()
{
	SetCollisionShape(this, this->meshCollisionType, origin);
	root = nullptr;
}

void Engine::EngineObjects::Physics::MeshCollider::Disown()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	root = nullptr;
	registered = false;

	SetCollisionShape(this, meshCollisionType, origin);

	if (this->collisionType == Enums::ColliderType::Trigger)
		collisionShape->createBulletGhostObject(true);
	else
		collisionShape->createBulletObject(true);

	registered = true;
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
	if (collisionShape == nullptr) return;

	if (root != nullptr && root->IsA<RigidBody^>())
	{
		if (root->As<RigidBody^>()->Kinematic && meshCollisionType == Enums::MeshCollisionType::Concave)
		{
			printWarning("A non kinematic rigidbody can't use a concave collider.");
			meshCollisionType = Enums::MeshCollisionType::Convex;
			registered = false;

			btRigidBody* body = root->As<RigidBody^>()->getRigidBody();
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			btRigidBody* body = root->As<RigidBody^>()->getRigidBody();
			body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
			body->activate(true);
		}
	}
}

#endif