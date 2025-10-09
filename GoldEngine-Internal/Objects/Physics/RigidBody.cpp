#include "../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionType.h"
#include "Native/NativePhysicsService.h"
#include "RigidBody.h"
#include "PhysicsService.h"
#include "../../native/CollisionShape.h"
#include "Triggers/Collider.h"

using namespace System;
using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Native;

#pragma managed(push, off)

inline btCompoundShape* _createCompoundShape(btVector3 _bounds = btVector3(1,1,1))
{
	btBoxShape* shape = new btBoxShape(_bounds);
	btCompoundShape* compoundShape = new btCompoundShape();

	btTransform localTransform;
	localTransform.setIdentity();
	localTransform.setOrigin({ 0,0,0 });
	compoundShape->addChildShape(localTransform, shape);

	shape->setMargin(0.0f);

	return compoundShape;
}

inline void disposeRigidBody(btRigidBody*& rigidBody)
{
	if (!rigidBody) return;

	btMotionState* motionState = rigidBody->getMotionState();
	if (motionState)
		delete motionState;

	delete rigidBody;
}

inline void setMass(btRigidBody*& rigidBody, float mass, std::array<float, 3> inertia)
{
	btVector3 _inertia(inertia[0], inertia[1], inertia[2]);

	if (mass != 0.0f)
	{
		btCollisionShape* collisionShape = rigidBody->getCollisionShape();
		collisionShape->calculateLocalInertia(mass, _inertia);
	}

	rigidBody->setMassProps(mass, _inertia);
	rigidBody->updateInertiaTensor();
}

inline void _setAngularVelocity(btRigidBody*& rigidBody, float x, float y, float z)
{
	rigidBody->setAngularVelocity({ x,y,z });
}

inline void _setLinearVelocity(btRigidBody*& rigidBody, float x, float y, float z)
{
	rigidBody->setLinearVelocity({ x,y,z });
}

inline btRigidBody* createPhysBody(std::array<float,3> originPosition, float mass, Engine::Native::CollisionShape* hookedShape)
{
	btCollisionShape* collisionShape;

	if (hookedShape == nullptr || !hookedShape->hasCollisionShape())
	{
		hookedShape->createCollisionShape(_createCompoundShape());
	}

	collisionShape = hookedShape->getCollisionShape();

	btVector3 inertia(0, 0, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(originPosition[0], originPosition[1], originPosition[2]));

	bool isDynamic = (mass != 0);

	if(isDynamic)
		collisionShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, collisionShape, inertia);

	btRigidBody* rigidBody = new btRigidBody(constructionInfo);

	rigidBody->activate(true);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setWorldTransform(transform);

	return rigidBody;
}

inline std::array<float, 6> getVector(btRigidBody*& rigidBody)
{
	btTransform& _transform = rigidBody->getWorldTransform();

	std::array<float, 6> data = std::array<float, 6>();

	data[0] = _transform.getOrigin().getX();
	data[1] = _transform.getOrigin().getY();
	data[2] = _transform.getOrigin().getZ();

	btQuaternion q = _transform.getRotation();
	btScalar eulerX, eulerY, eulerZ;

	btScalar roll, pitch, yaw;
	btMatrix3x3(q).getEulerYPR(yaw, pitch, roll);

	data[3] = roll;
	data[4] = pitch;
	data[5] = yaw;

	return data;
}

inline void setVector(btRigidBody*& rigidBody, float x, float y, float z)
{
	if (rigidBody == nullptr)
		return;

	btTransform transform;
	rigidBody->getMotionState()->getWorldTransform(transform);

	transform.setOrigin(btVector3(x, y, z));

	rigidBody->setWorldTransform(transform);
	rigidBody->getMotionState()->setWorldTransform(transform);
	rigidBody->setInterpolationWorldTransform(transform);
}

inline void setVector(btRigidBody*& rigidBody, float x, float y, float z, float yaw, float pitch, float roll)
{
	if (rigidBody == nullptr)
		return;

	btTransform& transform = rigidBody->getWorldTransform();
	//rigidBody->getMotionState()->getWorldTransform(transform);

	btQuaternion identity = btQuaternion::getIdentity();
	identity.setEuler(yaw, pitch, roll);

	transform.setRotation(identity);
	transform.setOrigin(btVector3(x, y, z));

	//rigidBody->setWorldTransform(transform);
	//rigidBody->getMotionState()->setWorldTransform(transform);
	//rigidBody->setInterpolationWorldTransform(transform);
}

inline void _clearForces(btRigidBody*& rigidBody)
{
	rigidBody->setLinearVelocity(btVector3(0, 0, 0));
	rigidBody->clearForces();
}

inline void _addForce(btRigidBody*& rigidBody, float x, float y, float z, int forceMode)
{
	if (forceMode == 0)
	{
		rigidBody->applyCentralImpulse(btVector3(x, y, z));
	}
	else if (forceMode == 1)
	{
		rigidBody->applyCentralForce(btVector3(x, y, z));
	}
}

inline void _addTorque(btRigidBody*& rigidBody, float x, float y, float z, int forceMode)
{
	if (forceMode == 0)
	{
		rigidBody->applyTorqueImpulse(btVector3(x, y, z));
	}
	else if (forceMode == 1)
	{
		rigidBody->applyTorque(btVector3(x, y, z));
	}
}

inline void _setAngularFactor(btRigidBody*& rigidBody, float x, float y, float z)
{
	rigidBody->setAngularFactor({
		x,
		y,
		z
	});
}

#pragma managed(pop)

void Engine::EngineObjects::Physics::RigidBody::createRigidBody()
{
	if (!Parent) return;

	Engine::Native::CollisionShape* collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();
	PhysicsService^ physicsService = Singleton<PhysicsService^>::Instance;
	Engine::EngineObjects::Physics::Collider^ collider = Parent->FindFirstChild<Engine::EngineObjects::Physics::Collider^>();

	btRigidBody* rigidBody;

	if (collider != nullptr)
	{
		collider->ClaimOwnership(this);
		rigidBody = createPhysBody(
			{
				Parent->transform->position.x,
				Parent->transform->position.y,
				Parent->transform->position.z
			},
			Mass,
			(Engine::Native::CollisionShape*)collider->getCollisionShape()
		);
	}
	else
	{
		rigidBody = createPhysBody(
			{
				Parent->transform->position.x,
				Parent->transform->position.y,
				Parent->transform->position.z
			},
			Mass,
			collisionShape
		);
	}

	collisionShape->setCollisionObject(rigidBody, false);

	if (Kinematic)
	{
		setMass(getRigidBody(), 0.0f, {0,0,0});
		getRigidBody()->setFlags(getRigidBody()->getFlags() | btRigidBody::CF_KINEMATIC_OBJECT);
	}
	else
	{
		setMass(getRigidBody(), Mass, { 0,0,0 });
		getRigidBody()->setFlags(getRigidBody()->getFlags() & ~btRigidBody::CF_KINEMATIC_OBJECT);
	}
	
	getRigidBody()->activate(true);
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);

	Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);

	registered = true;
}

void Engine::EngineObjects::Physics::RigidBody::PropertyChanged(String^ arg, Object^ _new, Object^ old)
{
	if (arg == "position")
	{
		if (!EngineState::PlayMode)
		{
			Engine::Components::Vector3 v3 = (Engine::Components::Vector3)_new;

			if (getRigidBody() == nullptr)
			{
				printError("btRigidbody* is not a reference to a valid pointer");
				return;
			}

			setVector(getRigidBody(), v3.x, v3.y, v3.z);
		}
	}
}
void Engine::EngineObjects::Physics::RigidBody::reloadRigidbody()
{ 
	disposeRigidBody(getRigidBody());
	createRigidBody();
}

void Engine::EngineObjects::Physics::RigidBody::onMassChanged(float newValue, float oldValue)
{
	if (newValue == oldValue)
		return;

	setMass(getRigidBody(), newValue, { 0,0,0 });
}

void Engine::EngineObjects::Physics::RigidBody::RecalculateInertia()
{
	setMass(getRigidBody(), Mass, { 0,0,0 });
}

void Engine::EngineObjects::Physics::RigidBody::onKinematicChanged(bool newValue, bool oldValue)
{
	if (newValue == oldValue) return;

	if (newValue)
	{
		setMass(getRigidBody(), 0.0f, { 0,0,0 });
		getRigidBody()->setFlags(getRigidBody()->getFlags() | btRigidBody::CF_KINEMATIC_OBJECT);
	}
	else
	{
		setMass(getRigidBody(), Mass, { 0,0,0 });
		getRigidBody()->setFlags(getRigidBody()->getFlags() & ~btRigidBody::CF_KINEMATIC_OBJECT);
	}
}

RigidBody::RigidBody(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
{
	if (!Singleton<PhysicsService^>::Instantiated)
		return;
}

void Engine::EngineObjects::Physics::RigidBody::Awake()
{
	// hook binding
	if (attributes->hasAttribute("Mass"))
		attributes->getAttribute("Mass")->onPropertyChanged->connect(gcnew System::Action<float, float>(this, &RigidBody::onMassChanged));

	if (attributes->hasAttribute("Kinematic"))
		attributes->getAttribute("Kinematic")->onPropertyChanged->connect(gcnew Action<bool, bool>(this, &RigidBody::onKinematicChanged));

	// hook property changed
	this->onPropertyChanged->connect(gcnew Action<String^, Object^, Object^>(this, &RigidBody::PropertyChanged));

}

void RigidBody::Start()
{
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
		return;

	createRigidBody();

	if (Parent == nullptr || !FindFirstSibling<Collider^>())
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}
	else
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)FindFirstSibling<Collider^>()->getCollisionShape();

		if (!_collisionShape->hasCollisionShape()) return;
		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		FindFirstSibling<Collider^>()->ClaimOwnership(this);
		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}

	registered = true;
}

void RigidBody::Update()
{
	if (!Singleton<PhysicsService^>::Instantiated || Parent == nullptr)
		return;
	
	if (!getRigidBody()->isActive())
	{
		return;
	}

	std::array<float, 6> data = getVector(this->getRigidBody());

	Parent->transform->position = Engine::Components::Vector3(data[0], data[1], data[2]);
	Parent->transform->rotation += Engine::Components::Vector3(data[3], data[4], data[5]);

	transform->position = Parent->transform->position;
	transform->rotation = Parent->transform->rotation;

	if (FindFirstSibling<Collider^>())
	{
		auto collider = FindFirstSibling<Collider^>();

		collider->transform->position = transform->position;
		collider->transform->rotation = transform->rotation;
	}
	
	if(Parent == nullptr || !FindFirstSibling<Collider^>())
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)this->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}
	else
	{
		Engine::Native::CollisionShape* _collisionShape = (Engine::Native::CollisionShape*)FindFirstSibling<Collider^>()->getCollisionShape();

		if (getRigidBody()->getCollisionShape() == _collisionShape->getCollisionShape()) return;

		FindFirstSibling<Collider^>()->ClaimOwnership(this);
		getRigidBody()->setCollisionShape(_collisionShape->getCollisionShape());
	}
}

void Engine::EngineObjects::Physics::RigidBody::Draw()
{
	if (getRigidBody() != nullptr && Parent != nullptr)
	{
		btRigidBody* rigidBody = getRigidBody();

		this->angularDamping = rigidBody->getAngularDamping();
		this->angularVelocity = Engine::Components::Vector3(rigidBody->getAngularVelocity().x(), rigidBody->getAngularVelocity().y(), rigidBody->getAngularVelocity().z());
	

		if (!Parent->transform->position.Equals(transform->position))
		{
			_clearForces(rigidBody);
			Engine::EngineObjects::Physics::Native::updateCollisionObject(
				rigidBody,
				{
					Parent->transform->position.x,
					Parent->transform->position.y,
					Parent->transform->position.z
				},
				{
					Parent->transform->rotation.x,
					Parent->transform->rotation.y,
					Parent->transform->rotation.z
				},
				{
					transform->scale.x,
					transform->scale.y,
					transform->scale.z
				}
			);
			_clearForces(rigidBody);
		}
	}

	if (Parent)
	{
		transform->position = Parent->transform->position;
	}

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->transform->position = transform->position;
	}
}

void Engine::EngineObjects::Physics::RigidBody::Destroy()
{
	Singleton<PhysicsService^>::Instance->RemovePhysicsObject(this->getRigidBody());

	if (Parent->FindFirstChild<Collider^>())
	{
		Parent->FindFirstChild<Collider^>()->Disown();
	}

	delete getRigidBody();
}

void RigidBody::OnInactive()
{
	if (getRigidBody() == nullptr)
		return;

	if (getRigidBody()->isActive())
	{
		setVector(getRigidBody(), (float)transform->position.x, (float)transform->position.y, (float)transform->position.z);
		getRigidBody()->setActivationState(DISABLE_SIMULATION);
		Singleton<PhysicsService^>::Instance->RemovePhysicsObject(this);
	}

	setVector(getRigidBody(), (float)transform->position.x, (float)transform->position.y, (float)transform->position.z);
}

void RigidBody::AddForce(Engine::Components::Vector3 position, Enums::ForceMode mode)
{
	if (getRigidBody() == nullptr)
		return;

	_addForce(getRigidBody(), position.x, position.y, position.z, (int)mode);
}

void Engine::EngineObjects::Physics::RigidBody::AddTorque(Engine::Components::Vector3 direction, Enums::ForceMode mode)
{
	if (getRigidBody() == nullptr)
		return;

	_addTorque(getRigidBody(), direction.x, direction.y, direction.z, (int)mode);
}

void RigidBody::OnActive()
{
	if (getRigidBody() == nullptr)
		return;

	if (!getRigidBody()->isActive())
	{
		getRigidBody()->activate(true);
		getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
		Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);
	}
}

void Engine::EngineObjects::Physics::RigidBody::clearForces()
{
	btRigidBody* rigidBody = getRigidBody();

	_clearForces(rigidBody);

	Engine::EngineObjects::Physics::Native::updateCollisionObject(
		rigidBody,
		{
			Parent->transform->position.x,
			Parent->transform->position.y,
			Parent->transform->position.z
		},
				{
					Parent->transform->rotation.x,
					Parent->transform->rotation.y,
					Parent->transform->rotation.z
				},
				{
					transform->scale.x,
					transform->scale.y,
					transform->scale.z
				}
	);
}

void Engine::EngineObjects::Physics::RigidBody::setAngularVelocity(Engine::Components::Vector3 velocity)
{
	if (getRigidBody() == nullptr)
		return;

	_setAngularVelocity(getRigidBody(), velocity.x, velocity.y, velocity.z);
}

void Engine::EngineObjects::Physics::RigidBody::setLinearVelocity(Engine::Components::Vector3 velocity)
{
	if (getRigidBody() == nullptr)
		return;

	_setLinearVelocity(getRigidBody(), velocity.x, velocity.y, velocity.z);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getCenterOfMass()
{
	return Engine::Components::Vector3(
		getRigidBody()->getCenterOfMassPosition().x(),
		getRigidBody()->getCenterOfMassPosition().y(),
		getRigidBody()->getCenterOfMassPosition().z()
	);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getTotalForce()
{
	return Engine::Components::Vector3(
		getRigidBody()->getTotalForce().x(),
		getRigidBody()->getTotalForce().y(),
		getRigidBody()->getTotalForce().z()
	);
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::getTotalTorque()
{
	return Engine::Components::Vector3(
		getRigidBody()->getTotalTorque().x(),
		getRigidBody()->getTotalTorque().y(),
		getRigidBody()->getTotalTorque().z()
	);
}

btRigidBody*& RigidBody::getRigidBody()
{
	return ((btRigidBody*&)((Engine::Native::CollisionShape*)this->getCollisionShape())->getCollisionObject());
}

void Engine::EngineObjects::Physics::RigidBody::SetBounciness(float bounciness)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setRestitution(bounciness);
}

float Engine::EngineObjects::Physics::RigidBody::GetBounciness()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getRestitution();
	else
		return 0.0f;
}

void Engine::EngineObjects::Physics::RigidBody::SetFriction(float friction)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setFriction(friction);
}

void Engine::EngineObjects::Physics::RigidBody::SetLinearDamping(float value)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(value, getRigidBody()->getAngularDamping());
}

void Engine::EngineObjects::Physics::RigidBody::SetAngularDamping(float value)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(getRigidBody()->getLinearDamping(), value);
}

void Engine::EngineObjects::Physics::RigidBody::SetDamping(float linearDamping, float angularDamping)
{
	if (getRigidBody() != nullptr)
		getRigidBody()->setDamping(linearDamping, angularDamping);
}

void Engine::EngineObjects::Physics::RigidBody::SetAngularFactor(Engine::Components::Vector3 axis)
{
	if (getRigidBody() != nullptr)
		_setAngularFactor(getRigidBody(), axis.x, axis.y, axis.z);
}

float Engine::EngineObjects::Physics::RigidBody::GetLinearDamping()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getLinearDamping();
	else
		return 0.0f;
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::GetLinearVelocity()
{
	if (getRigidBody() != nullptr)
		return Engine::Components::Vector3(getRigidBody()->getLinearVelocity().getX(), getRigidBody()->getLinearVelocity().getY(), getRigidBody()->getLinearVelocity().getZ());
	else
		return Engine::Components::Vector3::Zero();
}

Engine::Components::Vector3 Engine::EngineObjects::Physics::RigidBody::GetAngularFactor()
{
	if (getRigidBody() != nullptr)
		return Engine::Components::Vector3(getRigidBody()->getAngularFactor().getX(), getRigidBody()->getAngularFactor().getY(), getRigidBody()->getAngularFactor().getZ());
	else
		return Engine::Components::Vector3::Zero();
}

float Engine::EngineObjects::Physics::RigidBody::GetAngularDamping()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getAngularDamping();
	else
		return 0.0f;
}

float Engine::EngineObjects::Physics::RigidBody::GetFriction()
{
	if (getRigidBody() != nullptr)
		return getRigidBody()->getFriction();
	else
		return 0.0f;
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionEnter(GameObject^ instance)
{
	Parent->OnCollisionEnter(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionEnter(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionStay(GameObject^ instance)
{
	Parent->OnCollisionStay(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionStay(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollisionExit(GameObject^ instance)
{
	Parent->OnCollisionExit(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionExit(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerEnter(GameObject^ instance)
{
	Parent->OnTriggerEnter(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionEnter(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerStay(GameObject^ instance)
{
	Parent->OnTriggerStay(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionStay(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggerExit(GameObject^ instance)
{
	Parent->OnTriggerExit(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollisionExit(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnCollided(GameObject^ instance)
{
	Parent->OnCollided(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollided(instance);
	}
}

void Engine::EngineObjects::Physics::RigidBody::OnTriggered(GameObject^ instance)
{
	Parent->OnTriggered(instance);

	if (FindFirstSibling<Collider^>())
	{
		FindFirstSibling<Collider^>()->OnCollided(instance);
	}
}

#endif
