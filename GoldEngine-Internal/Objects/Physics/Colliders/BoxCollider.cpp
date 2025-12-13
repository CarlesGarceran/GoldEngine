#include "../SDK.h"

#ifdef USE_BULLET_PHYS

#include "../CollisionType.h"
#include "../Triggers/Collider.h"
#include "BoxCollider.h"
#include "../Native/CollisionShape.h"
#include "../Native/NativePhysicsService.h"
#include "../RigidBody.h"
#include "../PhysicsService.h"

#include "../../Abstract/Renderer.h"

UNMANAGED_BEGIN

inline void CopyMatrix(float matrix[16], RAYLIB::Matrix& _matrix)
{
	_matrix.m0 = matrix[0];
	_matrix.m1 = matrix[1];
	_matrix.m2 = matrix[2];
	_matrix.m3 = matrix[3];
	_matrix.m4 = matrix[4];
	_matrix.m5 = matrix[5];
	_matrix.m6 = matrix[6];
	_matrix.m7 = matrix[7];
	_matrix.m8 = matrix[8];
	_matrix.m9 = matrix[9];
	_matrix.m10 = matrix[10];
	_matrix.m11 = matrix[11];
	_matrix.m12 = matrix[12];
	_matrix.m13 = matrix[13];
	_matrix.m14 = matrix[14];
	_matrix.m15 = matrix[15];
}

inline void _setCollisionShape(Engine::Native::CollisionShape* hookedShape, std::array<float, 3> bounds, std::array<float, 3> offset)
{
	if (hookedShape == nullptr) return;

	btVector3 _bounds(
		(btScalar)bounds[0],
		(btScalar)bounds[1],
		(btScalar)bounds[2]
	);
	btBoxShape* shape = new btBoxShape(_bounds / 2);
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

inline void SafeUnloadModel(RAYLIB::Model& model)
{
	if (RAYLIB::IsModelValid(model))
		RAYLIB::UnloadModel(model);
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

void SetCollisionShape(GameObject^ Instance, Engine::Components::Vector3 extents, Engine::Components::Vector3 origin)
{
	if (Instance == nullptr) return;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)Instance->getCollisionShape());
	_setCollisionShape(
		collisionShape, 
		{ 
			extents.x, 
			extents.y, 
			extents.z 
		}, 
		{ 
			origin.x,
			origin.y, 
			origin.z 
		}
	);

	collisionShape->resampleAABB();
}

Engine::EngineObjects::Physics::BoxCollider::BoxCollider() :
	Collider()
{
	colliderShape = Enums::ColliderShape::Box;

	if (Parent != nullptr)
	{
		extents = Parent->transform->scale;
		transform->position = Parent->transform->position;
	}
}

void Engine::EngineObjects::Physics::BoxCollider::Awake()
{

	if (this->attributes->hasAttribute("extents"))
		this->attributes->getAttribute("extents")->onPropertyChanged->connect(gcnew Action<Engine::Components::Vector3, Engine::Components::Vector3>(this, &BoxCollider::OnExtentsChanged));

	if (this->attributes->hasAttribute("origin"))
		this->attributes->getAttribute("origin")->onPropertyChanged->connect(gcnew Action<Engine::Components::Vector3, Engine::Components::Vector3>(this, &BoxCollider::OnOriginChanged));

	if (this->attributes->hasAttribute("collisionType"))
		this->attributes->getAttribute("collisionType")->onPropertyChanged->connect(gcnew Action<Engine::EngineObjects::Physics::Enums::ColliderType, Engine::EngineObjects::Physics::Enums::ColliderType>(this, &BoxCollider::OnCollisionTypeChanged));

	this->onPropertyChanged->connect(gcnew Action<String^, System::Object^, System::Object^>(this, &BoxCollider::OnParentChanged));

	SetCollisionShape(this, extents, origin);

	this->cachedModel = new Engine::Native::EnginePtr<RAYLIB::Model>(
		{ },
		&SafeUnloadModel,
		&SafeUnloadModel
	);
}

void Engine::EngineObjects::Physics::BoxCollider::Start()
{
	Collider::Start();

	if (!registered && Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instantiated)
	{
		Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());
		
		if(!collisionShape->hasCollisionShape()) SetCollisionShape(this, extents, origin);

		if (this->collisionType == Enums::ColliderType::Trigger)
			collisionShape->createBulletGhostObject(true);
		else
			collisionShape->createBulletObject(true);
	
		if (renderWires)
		{
			auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
			RAYLIB::Mesh mesh = { 0 };
			nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

			this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
		}
	}
}

void Engine::EngineObjects::Physics::BoxCollider::DrawGizmo()
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

		RenderGuizmos(collisionObject);
	}
	else
	{
		btCollisionObject* collisionObject = collisionShape->getCollisionObject();

		std::array<float, 3> computedPosition = computePosition(collisionObject, (btCompoundShape*)shape, 0);

		RenderGuizmos(collisionObject);
	}
}

void Engine::EngineObjects::Physics::BoxCollider::Destroy()
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

bool Engine::EngineObjects::Physics::BoxCollider::IsOwned()
{
	return (root != nullptr);
}

bool Engine::EngineObjects::Physics::BoxCollider::ClaimOwnership(GameObject^ instance)
{
	if (!instance->IsA<Engine::EngineObjects::Physics::RigidBody^>())
		return false;

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	/*
	if (instance->IsA<RigidBody^>() && root != instance)
	{
		SetCollisionShape(
			instance, 
			extents,
			transform->position + origin
		);
	}
	*/

	root = instance;

	if (!collisionShape->hasCollisionShape())
	{
		SetCollisionShape(
			this,
			extents,
			origin
		);

		if (renderWires)
		{
			auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
			RAYLIB::Mesh mesh = { 0 };
			nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

			this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
		}
	}

	if (root != nullptr)
	{
		Engine::Native::CollisionShape* _collisionShape = ((Engine::Native::CollisionShape*)root->getCollisionShape());

		//SetCollisionShape(this, extents, origin);

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

void Engine::EngineObjects::Physics::BoxCollider::CreateShape()
{
	SetCollisionShape(this, extents, origin);

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if(renderWires)
	{
		auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
		RAYLIB::Mesh mesh = { 0 };
		nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

		this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
	}

	root = nullptr;
}

void Engine::EngineObjects::Physics::BoxCollider::Disown()
{
	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	root = nullptr;

	SetCollisionShape(this, extents, origin);

	if (renderWires)
	{
		auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
		RAYLIB::Mesh mesh = { 0 };
		nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

		this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
	}

	if (this->collisionType == Enums::ColliderType::Trigger)
		collisionShape->createBulletGhostObject(true);
	else
		collisionShape->createBulletObject(true);

	registered = true;
}

void Engine::EngineObjects::Physics::BoxCollider::OnCollisionTypeChanged(Enums::ColliderType newType, Enums::ColliderType oldType)
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

void Engine::EngineObjects::Physics::BoxCollider::OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform)
{
	if (property == "parent")
	{
		extents = extents * ((Engine::Internal::Components::Transform^)transform)->scale;
		this->transform->position = ((Engine::Internal::Components::Transform^)transform)->position;

		root = nullptr;
	}
}

void Engine::EngineObjects::Physics::BoxCollider::RenderGuizmos(btCollisionObject* cObj)
{
	if (cachedModel && cachedModel->isLoaded())
	{
		RAYLIB::Model& model = cachedModel->getInstance();

		if (!RAYLIB::IsModelValid(model)) return;

		float matrix[16] = {};
		Engine::EngineObjects::Physics::Native::getOpenGLMatrix(cObj, matrix);
		CopyMatrix(matrix, model.transform);

		RAYLIB::DrawModelWiresEx(model,
			Engine::Components::Vector3::Zero().toNative(),
			{ 0,0,0 },
			0,
			transform->scale.toNative(),
			wireColor->toNative()
		);
	}
}

void Engine::EngineObjects::Physics::BoxCollider::OnExtentsChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue)
{
	if (newValue.Equals(oldValue)) return;

	SetCollisionShape(this, newValue, origin);

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (renderWires)
	{
		auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
		RAYLIB::Mesh mesh = { 0 };
		nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

		this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
	}

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

void Engine::EngineObjects::Physics::BoxCollider::OnOriginChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue)
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

void Engine::EngineObjects::Physics::BoxCollider::Update()
{
	Collider::Update();

	Engine::Native::CollisionShape* collisionShape = ((Engine::Native::CollisionShape*)this->getCollisionShape());

	if (renderWires)
	{
		if (this->cachedModel == nullptr) return;

		if (!this->cachedModel->isLoaded() && !RAYLIB::IsModelValid(this->cachedModel->getInstance()))
		{
			auto nPhys = Singleton<Engine::EngineObjects::Physics::PhysicsService^>::Instance->getNativePhysicsService();
			RAYLIB::Mesh mesh = { 0 };
			nPhys->ShapeToMesh((btCompoundShape*)collisionShape->getCollisionShape(), mesh, true);

			this->cachedModel->setInstanceRef(RAYLIB::LoadModelFromMesh(mesh));
		}
	}
	else
	{
		if (this->cachedModel->isLoaded() && RAYLIB::IsModelValid(this->cachedModel->getInstance()))
		{
			this->cachedModel->free();
		}
	}

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

#endif