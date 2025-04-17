#include "../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionType.h"
#include "Rigidbody.h"
#include "Native/NativePhysicsService.h"
#include "PhysicsService.h"

using namespace System;
using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Native;
using namespace Engine::Internal::Components;
using namespace System::Runtime::InteropServices;

void SendObjectCallback(void* obj0ptr, void* obj1ptr)
{
	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	rootObject0->OnCollisionEnter(rootObject1);
	rootObject1->OnCollisionEnter(rootObject0);

	handle0.Free();
	handle1.Free();
}

#pragma managed(push, off)

private struct GE_ContactResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
	bool collisionDetected = false;

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1) override
	{
		void* obj0ptr = colObj0Wrap->getCollisionObject()->getUserPointer();
		void* obj1ptr = colObj1Wrap->getCollisionObject()->getUserPointer();

		SendObjectCallback(obj0ptr, obj1ptr);

		collisionDetected = true;
		return 0;
	}
};



void setGravity(btDiscreteDynamicsWorld* world, float x, float y, float z)
{
	world->setGravity(btVector3(x, y, z));

	btAlignedObjectArray<btRigidBody*> rigidBodies = world->getNonStaticRigidBodies();

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		btRigidBody* rigidBody = rigidBodies[i];

		if (rigidBody != nullptr)
		{
			rigidBody->setGravity(btVector3(x, y, z));
			rigidBody->applyGravity();
		}
	}
}


void testCollision(btDiscreteDynamicsWorld* world)
{
	btAlignedObjectArray<btCollisionObject*> rigidBodies = world->getCollisionObjectArray();

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		btCollisionObject* collisionObject = rigidBodies[i];
		GE_ContactResultCallback result;
		world->contactTest(collisionObject, result);

	}
}

#pragma managed(pop)

PhysicsService::PhysicsService(String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::Script(name, transform)
{
}

void PhysicsService::Start()
{
	this->protectMember();

	if (this->Gravity == nullptr)
		this->Gravity = gcnew Engine::Components::Vector3(0, 9.81f, 0);

	collisionConfig = new btDefaultCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfig);
	bvhInterface = new btDbvtBroadphase();
	SQCsolver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(collisionDispatcher, bvhInterface, SQCsolver, collisionConfig);
	nativePhysicsService = new NativePhysicsService();

	setGravity(world, this->Gravity->x, this->Gravity->y, this->Gravity->z);

	frameRate = 60;
	maxSubSteps = 10;

	Singleton<PhysicsService^>::Create(this);

	if (!attributes->hasAttribute("Gravity"))
		attributes->addAttribute(Engine::Scripting::Attribute::New(Engine::Scripting::AccessLevel::Public, "Gravity", gcnew Engine::Components::Vector3(0, 9.81f, 0), Engine::Components::Vector3::typeid));
}

void PhysicsService::Update()
{
	if (maxSubSteps <= 0)
		maxSubSteps = 1;

	setGravity(world, this->Gravity->x, this->Gravity->y, this->Gravity->z);
	world->stepSimulation((1.0f / frameRate), (maxSubSteps > 0 ? maxSubSteps : 1));
}

void PhysicsService::AddPhysicsObject(Engine::EngineObjects::Physics::RigidBody^ rigidBody)
{
	if (rigidBody != nullptr && rigidBody->getRigidBody() != nullptr && world != nullptr)
	{
		world->addRigidBody(rigidBody->getRigidBody());
	}
	else
		if (world == nullptr)
			throw gcnew Exception("Physics World not instantiated");
		else if (rigidBody->getRigidBody() == nullptr)
			throw gcnew Exception("Rigidbody not instantiated");
}

void PhysicsService::RemovePhysicsObject(Engine::EngineObjects::Physics::RigidBody^ rigidBody)
{
	if (rigidBody != nullptr && rigidBody->getRigidBody() != nullptr && world != nullptr)
	{
		world->removeRigidBody(rigidBody->getRigidBody());
	}
	else
		if (world == nullptr)
			throw gcnew Exception("Physics World not instantiated");
		else if (rigidBody->getRigidBody() == nullptr)
			throw gcnew Exception("Rigidbody not instantiated");
}

void PhysicsService::AddCollisionObject(btCollisionObject* collisionObject)
{
	if (this == nullptr)
	{
		throw gcnew Exception("Physics Service is not initialized");
		return;
	}

	if (collisionObject != nullptr && world != nullptr)
	{
		world->addCollisionObject(collisionObject);
	}
	else
		if (world == nullptr)
			throw gcnew Exception("Physics World is not instantiated");
		else if (collisionObject == nullptr)
			throw gcnew Exception("CollisionObject is not a pointer to an instance");
}

void PhysicsService::RemoveCollisionObject(btCollisionObject* collisionObject)
{
	if (collisionObject != nullptr && world != nullptr)
	{
		world->removeCollisionObject(collisionObject);
	}
	else
		if (world == nullptr)
			printError("Physics World not instantiated");
		else if (collisionObject == nullptr)
			printError("CollisionObject is not a pointer to an instance");
}


NativePhysicsService* PhysicsService::getNativePhysicsService()
{
	return this->nativePhysicsService;
}

#endif