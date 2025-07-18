#include "../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionType.h"
#include "Native/NativePhysicsService.h"
#include "RigidBody.h"
#include "PhysicsService.h"

using namespace System;
using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Native;

#pragma managed(push, off)

void disposeRigidBody(btRigidBody* rigidBody)
{
	if (!rigidBody) return;

	btMotionState* motionState = rigidBody->getMotionState();
	if (motionState)
		delete motionState;

	btCollisionShape* shape = rigidBody->getCollisionShape();
	if (shape)
		delete shape;


	delete rigidBody;
}

btRigidBody* createPhysBody(std::array<float,3> originPosition, float mass)
{
	btCollisionShape* collisionShape = new btBoxShape(btVector3(1, 1, 1)); 
	btVector3 inertia(0, 0, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(originPosition[0], originPosition[1], originPosition[2]));

	bool isDynamic = (mass != 0.0f);

	if(isDynamic)
		collisionShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, collisionShape, inertia);

	btRigidBody* rigidBody = new btRigidBody(constructionInfo);
	
	rigidBody->setActivationState(DISABLE_DEACTIVATION);

	return rigidBody;
}

std::array<float, 3> getVector(btRigidBody* rigidBody)
{
	btTransform _transform;
	rigidBody->getMotionState()->getWorldTransform(_transform);

	std::array<float, 3> data = std::array<float, 3>();

	data[0] = _transform.getOrigin().getX();
	data[1] = _transform.getOrigin().getY();
	data[2] = _transform.getOrigin().getZ();

	return data;
}

void setVector(btRigidBody* rigidBody, float x, float y, float z)
{
	if (rigidBody == nullptr)
		return;

	btTransform transform;
	rigidBody->getMotionState()->getWorldTransform(transform);

	transform.setOrigin(btVector3(x, y, z));

	rigidBody->getMotionState()->setWorldTransform(transform);
	rigidBody->setWorldTransform(transform);
}

void _addForce(btRigidBody* rigidBody, float x, float y, float z, int forceMode)
{
	if (forceMode == 0)
	{
		btTransform _transform;
		rigidBody->getMotionState()->getWorldTransform(_transform);

		rigidBody->applyImpulse(btVector3(x, y, z), _transform.getOrigin());
	}
	else if (forceMode == 1)
	{
		btTransform _transform;
		rigidBody->getMotionState()->getWorldTransform(_transform);

		rigidBody->applyForce(btVector3(x, y, z), _transform.getOrigin());
	}
}

#pragma managed(pop)

void RigidBody::createRigidBody()
{
	PhysicsService^ physicsService = Singleton<PhysicsService^>::Instance;

	rigidBody = createPhysBody({ transform->position->x, transform->position->y, transform->position->z }, mass);

	// Add it to the service
	Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);

	// hook binding
	if (attributes->hasAttribute("modelId"))
		attributes->getAttribute("modelId")->onPropertyChanged->connect(gcnew System::Action<unsigned int, unsigned int>(this, &RigidBody::onModelIdChanged));
	if (attributes->hasAttribute("meshId"))
		attributes->getAttribute("meshId")->onPropertyChanged->connect(gcnew System::Action<unsigned int, unsigned int>(this, &RigidBody::onMeshIdChanged));
	if (attributes->hasAttribute("mass"))
		attributes->getAttribute("mass")->onPropertyChanged->connect(gcnew System::Action<float, float>(this, &RigidBody::onMassChanged));
}

void RigidBody::onModelIdChanged(unsigned int newValue, unsigned int oldValue)
{
	PhysicsService^ physicsService = Singleton<PhysicsService^>::Instance;
}

void RigidBody::onMeshIdChanged(unsigned int newValue, unsigned int oldValue)
{
	PhysicsService^ physicsService = Singleton<PhysicsService^>::Instance;
}

void Engine::EngineObjects::Physics::RigidBody::onMassChanged(float newValue, float oldValue)
{
	Singleton<PhysicsService^>::Instance->RemovePhysicsObject(this);

	disposeRigidBody(rigidBody);
}

RigidBody::RigidBody(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
{
	if (!Singleton<PhysicsService^>::Instantiated)
		return;

	collisionType = Enums::CollisionType::Convex;
	meshId = 0;
	modelId = 0;
	hookedObject = nullptr;
}


void RigidBody::Start()
{
	if (!Singleton<PhysicsService^>::Instantiated)
		return;

	if (hookedObject == nullptr)
		return;

	createRigidBody();
}

void RigidBody::Update()
{
	if (!Singleton<PhysicsService^>::Instantiated || hookedObject == nullptr)
		return;
	
	if (this->rigidBody == nullptr)
		createRigidBody();

	if (!rigidBody->isActive())
		printError("Rigidbody not active");

	std::array<float, 3> data = getVector(this->rigidBody);

	transform->position->x = data[0];
	transform->position->y = data[1];
	transform->position->z = data[2];

	hookedObject->transform->position->copy(transform->position);
}

void Engine::EngineObjects::Physics::RigidBody::Draw()
{
	setVector(rigidBody, transform->position->x, transform->position->y, transform->position->z);
}

void RigidBody::OnInactive()
{
	if (rigidBody == nullptr)
		return;

	if (rigidBody->isActive())
	{
		rigidBody->setActivationState(DISABLE_SIMULATION);
		Singleton<PhysicsService^>::Instance->RemovePhysicsObject(this);
	}

	setVector(rigidBody, (float)transform->position->x, (float)transform->position->y, (float)transform->position->z);
}

void RigidBody::AddForce(Engine::Components::Vector3^ position, Enums::ForceMode mode)
{
	if (rigidBody == nullptr)
		return;

	_addForce(rigidBody, position->x, position->y, position->z, (int)mode);
}

void RigidBody::OnActive()
{
	if (rigidBody == nullptr)
		return;

	Singleton<PhysicsService^>::Instance->AddPhysicsObject(this);

	if (!rigidBody->isActive())
	{
		rigidBody->activate(true);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
}

btRigidBody* RigidBody::getRigidBody()
{
	return rigidBody;
}

#endif
