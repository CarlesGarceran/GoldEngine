#include "../SDK.h"

#ifdef USE_BULLET_PHYS

#include "../CollisionType.h"
#include "../Triggers/Collider.h"
#include "CapsuleCollider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#include "../../Abstract/Renderer.h"

UNMANAGED_BEGIN

inline void _setCollisionShape(Engine::Native::CollisionShape* hookedShape, float radius, float height, std::array<float, 3> offset)
{
	if (hookedShape == nullptr) return;

	btCapsuleShape* shape = new btCapsuleShape(radius, height);
	btCompoundShape* compoundShape = new btCompoundShape();

	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin({ offset[0], offset[1], offset[2] });
	compoundShape->addChildShape(localTransform, shape);

	shape->setMargin(0.0f);

	hookedShape->createCollisionShape(compoundShape);
	return;
}

inline void updateCompoundShape(btCompoundShape*& collisionShape, std::array<float, 3> offset)
{
	if (collisionShape == nullptr) return;

	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin({ offset[0], offset[1], offset[2] });
	collisionShape->updateChildTransform(0, localTransform, true);
}

inline std::array<float, 3> computePosition(btCollisionObject* a0, btCompoundShape* a1, int index = 0)
{
	if (a0 == nullptr || a1 == nullptr) return { 0,0,0 };
	if (index > a1->getNumChildShapes()) return { 0,0,0 };

	const btTransform& transform = a0->getWorldTransform();
	const btTransform& child = a1->getChildTransform(index);

	btTransform childTransform = transform * child;

	return { childTransform.getOrigin().x(), childTransform.getOrigin().y(), childTransform.getOrigin().z() };
}

UNMANAGED_END

inline Engine::Components::Vector3 getWorldTransform(btCollisionObject* colObj)
{
	if (colObj == nullptr)
	{
		return Engine::Components::Vector3::Zero();
	}

	return Engine::Components::Vector3(
		colObj->getWorldTransform().getOrigin().x(),
		colObj->getWorldTransform().getOrigin().y(),
		colObj->getWorldTransform().getOrigin().z()
	);
}

inline Engine::Components::Vector3 getWorldTransform(btCompoundShape* colObj, int index = 0)
{
	if (colObj == nullptr)
	{
		return Engine::Components::Vector3::Zero();
	}

	return Engine::Components::Vector3(
		colObj->getChildTransform(index).getOrigin().x(),
		colObj->getChildTransform(index).getOrigin().y(),
		colObj->getChildTransform(index).getOrigin().z()
	);
}

void SetCollisionShape(GameObject^ Instance, float radius, float height, Engine::Components::Vector3 origin)
{
	if (Instance == nullptr) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)Instance->getCollisionShape());
	_setCollisionShape(
		collisionShape,
		radius,
		height,
		{
			origin.x,
			origin.y,
			origin.z
		}
	);

	collisionShape->resampleAABB();
}

#endif

Engine::EngineObjects::Physics::CapsuleCollider::CapsuleCollider()
	: Collider()
{
	colliderShape = Enums::ColliderShape::Capsule;
}

void Engine::EngineObjects::Physics::CapsuleCollider::Awake()
{
	if (this->attributes->hasAttribute("origin"))
		this->attributes->getAttribute("origin")->onPropertyChanged->connect(gcnew Action<Engine::Components::Vector3, Engine::Components::Vector3>(this, &CapsuleCollider::OnOriginChanged));

	if (this->attributes->hasAttribute("collisionType"))
		this->attributes->getAttribute("collisionType")->onPropertyChanged->connect(gcnew Action<Engine::EngineObjects::Physics::Enums::ColliderType, Engine::EngineObjects::Physics::Enums::ColliderType>(this, &CapsuleCollider::OnCollisionTypeChanged));

	this->onPropertyChanged->connect(gcnew Action<String^, System::Object^, System::Object^>(this, &CapsuleCollider::OnParentChanged));

	SetCollisionShape(this, radius, height, origin);
}

void Engine::EngineObjects::Physics::CapsuleCollider::Start()
{
	Collider::Start();

	if (!registered && Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

		if (!collisionShape->hasCollisionShape()) SetCollisionShape(this, radius, height, origin);

		if (this->collisionType == Enums::ColliderType::Trigger)
			collisionShape->createBulletGhostObject(true);
		else
			collisionShape->createBulletObject(true);
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::Update()
{
	Collider::Update();

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (root)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (_collisionShape->hasCollisionObject())
		{
			btCollisionObject*& collisionObject = _collisionShape->getCollisionObject();

			if (collisionObject->getCollisionShape() != collisionShape->getCollisionShape())
			{
				collisionObject->setCollisionShape(collisionShape->getCollisionShape());
			}
		}

		if (collisionShape->hasCollisionObject() && collisionShape->getCollisionObject() != nullptr)
		{
			if (collisionShape->getCollisionObject() != _collisionShape->getCollisionObject())
				collisionShape->freeCollisionObject();
		}
	}
	else
	{
		if (collisionShape->hasCollisionObject())
		{
			auto collisionObject = collisionShape->getCollisionObject();
			Engine::EngineObjects::Physics::Native::updateCollisionObject(collisionObject,
				{
					transform->position.x + origin.x,
					transform->position.y + origin.y,
					transform->position.z + origin.z
				},
				{
					transform->rotation.x,
					transform->rotation.y,
					transform->rotation.z
				},
				{
					transform->scale.x,
					transform->scale.y,
					transform->scale.z
				}
			);
		}
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::DrawGizmo()
{
	if (!renderWires) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());
	btCollisionShape* shape = (btCollisionShape*)collisionShape->getCollisionShape();

	if (shape == nullptr) return;

	if (root != nullptr)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (!_collisionShape->hasCollisionObject()) return;

		btCollisionObject* collisionObject = _collisionShape->getCollisionObject();

		if (collisionObject == nullptr || collisionObject->getCollisionShape() != shape) return;

		std::array<float, 3> computedPosition = computePosition(collisionObject, (btCompoundShape*)shape, 0);

		RAYLIB::DrawCapsuleWires(
			{ computedPosition[0], computedPosition[1] - (height / 2), computedPosition[2] },
			{ computedPosition[0], computedPosition[1] + (height / 2), computedPosition[2] },
			radius,
			gizmo_slices,
			gizmo_rings,
			wireColor->toNative()
		);
	}
	else
	{
		btCollisionObject* collisionObject = collisionShape->getCollisionObject();

		std::array<float, 3> computedPosition = computePosition(collisionObject, (btCompoundShape*)shape, 0);

		RAYLIB::DrawCapsuleWires(
			{ computedPosition[0], computedPosition[1] - (height / 2), computedPosition[2] },
			{ computedPosition[0], computedPosition[1] + (height / 2), computedPosition[2] },
			radius,
			gizmo_slices,
			gizmo_rings,
			wireColor->toNative()
		);
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::Destroy()
{
	if (Parent != nullptr && this->originalCollisionShape != nullptr)
	{
		Parent->setCollisionShape(this->originalCollisionShape);
	}
	else
	{
		if (originalCollisionShape != nullptr) // Idk in which case this could happen but... idk, just in case it happens.
			delete originalCollisionShape;
	}
}


bool Engine::EngineObjects::Physics::CapsuleCollider::IsOwned()
{
	return (root != nullptr);
}

bool Engine::EngineObjects::Physics::CapsuleCollider::ClaimOwnership(GameObject^ instance)
{
	if (!instance->IsA<Engine::EngineObjects::Physics::RigidBody^>())
		return false;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	root = instance;

	if (!collisionShape->hasCollisionShape())
	{
		SetCollisionShape(
			this,
			radius,
			height,
			origin
		);
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

void Engine::EngineObjects::Physics::CapsuleCollider::CreateShape()
{
	SetCollisionShape(this, radius, height, origin);
	root = nullptr;
}

void Engine::EngineObjects::Physics::CapsuleCollider::Disown()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	root = nullptr;

	SetCollisionShape(this, radius, height, origin);

	if (this->collisionType == Enums::ColliderType::Trigger)
		collisionShape->createBulletGhostObject(true);
	else
		collisionShape->createBulletObject(true);

	registered = true;
}

void Engine::EngineObjects::Physics::CapsuleCollider::OnCollisionTypeChanged(Enums::ColliderType newType, Enums::ColliderType oldType)
{
	if (newType == oldType) return;

	if (root)
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (collisionShape->hasCollisionObject())
		{
			btCollisionObject*& collisionObject = collisionShape->getCollisionObject();

			if (newType == Enums::ColliderType::Trigger)
			{
				collisionObject->setCollisionShape(nullptr);
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
			else
			{
				collisionObject->setCollisionShape(((Engine::Native::CollisionShape*)this->getCollisionShape())->getCollisionShape());
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
			}
		}
	}
	else
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

		if (!collisionShape->hasCollisionObject())
		{
			if (newType == Enums::ColliderType::Trigger)
				collisionShape->createBulletGhostObject(true);
			else
				collisionShape->createBulletObject(true);
		}
		else
		{
			btCollisionObject*& collisionObject = collisionShape->getCollisionObject();

			if (newType == Enums::ColliderType::Trigger)
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			else
				collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::OnOriginChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue)
{
	if (newValue.Equals(oldValue)) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (!collisionShape->hasCollisionShape()) return;

	updateCompoundShape(
		(btCompoundShape*&)collisionShape->getCollisionShape(),
		{
			newValue.x,
			newValue.y,
			newValue.z
		}
	);

	if (root && root->IsA<RigidBody^>())
	{
		auto rigidBody = root->as<RigidBody^>();
		rigidBody->getRigidBody()->setCollisionShape(collisionShape->getCollisionShape());
		rigidBody->RecalculateInertia();
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::OnRadiusChanged(float newValue, float oldValue)
{
	if (newValue == oldValue) return;

	SetCollisionShape(this, newValue, height, origin);

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (root)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (_collisionShape->hasCollisionObject())
		{
			btCollisionObject*& collisionObject = _collisionShape->getCollisionObject();
			collisionObject->setCollisionShape(collisionShape->getCollisionShape());
		}
	}
	else
	{
		if (!collisionShape->hasCollisionObject())
		{
			if (this->collisionType == Enums::ColliderType::Trigger)
				collisionShape->createBulletGhostObject(true);
			else
				collisionShape->createBulletObject(true);
		}
		else
		{
			btCollisionObject*& collisionObject = collisionShape->getCollisionObject();
			collisionObject->setCollisionShape(collisionShape->getCollisionShape());
			collisionShape->resampleAABB();
		}
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::OnHeightChanged(float newValue, float oldValue)
{
	if (newValue == oldValue) return;

	SetCollisionShape(this, radius, newValue, origin);

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (root)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		if (_collisionShape->hasCollisionObject())
		{
			btCollisionObject*& collisionObject = _collisionShape->getCollisionObject();
			collisionObject->setCollisionShape(collisionShape->getCollisionShape());
		}
	}
	else
	{
		if (!collisionShape->hasCollisionObject())
		{
			if (this->collisionType == Enums::ColliderType::Trigger)
				collisionShape->createBulletGhostObject(true);
			else
				collisionShape->createBulletObject(true);
		}
		else
		{
			btCollisionObject*& collisionObject = collisionShape->getCollisionObject();
			collisionObject->setCollisionShape(collisionShape->getCollisionShape());
			collisionShape->resampleAABB();
		}
	}
}

void Engine::EngineObjects::Physics::CapsuleCollider::OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform)
{
	if (property == "parent")
	{
		this->transform->position = ((Engine::Internal::Components::Transform^)transform)->position;

		root = nullptr;
	}
}
