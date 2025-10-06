#include "../../SDK.h"

#ifdef USE_BULLET_PHYS

#include "CollisionType.h"
#include "Rigidbody.h"
#include "Native/NativePhysicsService.h"
#include "PhysicsService.h"
#include "../../native/CollisionShape.h"
#include <set>
#include <utility>
#include <shared_mutex>

using namespace System;
using namespace Engine::EngineObjects::Physics;
using namespace Engine::EngineObjects::Physics::Native;
using namespace Engine::Internal::Components;
using namespace System::Runtime::InteropServices;

void SendObjectStayCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnCollisionStay(rootObject1);
	rootObject1->OnCollisionStay(rootObject0);
}

void SendTriggerStayCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnTriggerStay(rootObject1);
	rootObject1->OnTriggerStay(rootObject0);
}


void SendOnTriggeredCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnTriggered(rootObject1);
	rootObject1->OnTriggered(rootObject0);
}

void SendOnCollidedCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnCollided(rootObject1);
	rootObject1->OnCollided(rootObject0);
}

void SendObjectEnterCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);
	
	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnCollisionEnter(rootObject1);
	rootObject1->OnCollisionEnter(rootObject0);
}

void SendTriggerEnterCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnTriggerEnter(rootObject1);
	rootObject1->OnTriggerEnter(rootObject0);
}

void SendObjectExitCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnCollisionExit(rootObject1);
	rootObject1->OnCollisionExit(rootObject0);
}

void SendTriggerExitCallback(void* obj0ptr, void* obj1ptr)
{
	if (obj0ptr == nullptr || obj1ptr == nullptr)
		return;

	IntPtr intPtr0(obj0ptr);
	IntPtr intPtr1(obj1ptr);

	GCHandle handle0 = GCHandle::FromIntPtr(intPtr0);
	GCHandle handle1 = GCHandle::FromIntPtr(intPtr1);

	GameObject^ rootObject0 = safe_cast<GameObject^>(handle0.Target);
	GameObject^ rootObject1 = safe_cast<GameObject^>(handle1.Target);

	if (rootObject0 == nullptr || rootObject1 == nullptr)
		return;

	rootObject0->OnTriggerExit(rootObject1);
	rootObject1->OnTriggerExit(rootObject0);
}

void SendPhysicsUpdateCallback()
{
	for each (GameObject^ object in Singleton<Engine::Scripting::ObjectManager^>::Instance->GetObjects())
	{
		if(object == nullptr) continue;

		object->PhysicsUpdate();
	}
}

msclr::gcroot<GameObject^> GetObjectFromPointer(const btCollisionObject* collisionObject)
{
	if (collisionObject == nullptr) return msclr::gcroot<GameObject^>(nullptr);

	void* userPointer = collisionObject->getUserPointer();

	if (userPointer == nullptr) return msclr::gcroot<GameObject^>(nullptr);

	IntPtr ptr(userPointer);
	GCHandle handle = GCHandle::FromIntPtr(ptr);

	return msclr::gcroot<GameObject^>(safe_cast<GameObject^>(handle.Target));
}

#pragma managed(push, off)

static std::mutex physicsMutex;

typedef struct RCHit
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	msclr::gcroot<GameObject^> gameObject;
	bool hit;
} RCHit;

typedef struct HTest
{
	std::array<float, 3> hitPoint;
	std::array<float, 3> hitNormal;
	std::array<float, 3> correction;
	float depth;
} HTest;

inline auto MakePair(const void* a, const void* b)
{
	return std::minmax(a, b);
}

private struct GE_SingleContactResultCallback : public btCollisionWorld::ContactResultCallback
{
private:
	HTest& hitTest;

public:
	bool collisionDetected = false;

	GE_SingleContactResultCallback(HTest& hit)
		: hitTest(hit) 
	{
	}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1) override
	{
		if (colObj0Wrap == nullptr) return 0;
		if (colObj1Wrap == nullptr) return 0;

		const btCollisionObject* objA = colObj0Wrap->getCollisionObject();
		const btCollisionObject* objB = colObj1Wrap->getCollisionObject();

		if (objA == nullptr || objB == nullptr) return 0;

		void* obj0ptr = objA->getUserPointer();
		void* obj1ptr = objB->getUserPointer();

		if (obj0ptr == nullptr || obj1ptr == nullptr) return 0;

		collisionDetected = true;

		if (cp.getDistance() < 0)
		{
			btVector3 normalOnB = cp.m_normalWorldOnB;
			btScalar penetrationDepth = -cp.getDistance();

			btVector3 pointOnA = cp.getPositionWorldOnA();
			btVector3 pointOnB = cp.getPositionWorldOnB();

			btVector3 correction = normalOnB * penetrationDepth;

			this->hitTest.hitPoint = { pointOnA.x(), pointOnA.y(), pointOnA.z() };
			this->hitTest.hitNormal = { pointOnB.x(), pointOnB.y(), pointOnB.z() };
			this->hitTest.correction = { correction.x(), correction.y(), correction.z() };
			this->hitTest.depth = penetrationDepth;
		}
		else
		{
			btScalar penetrationDepth = cp.getDistance();

			btVector3 pointOnA = cp.getPositionWorldOnA();
			btVector3 pointOnB = cp.getPositionWorldOnB();


			this->hitTest.hitPoint = { pointOnA.x(), pointOnA.y(), pointOnA.z() };
			this->hitTest.hitNormal = { pointOnB.x(), pointOnB.y(), pointOnB.z() };
			this->hitTest.correction = { pointOnA.x(), pointOnA.y(), pointOnA.z() };
			this->hitTest.depth = penetrationDepth;
		}

		return 0;
	}
};

private struct GE_ContactResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
	bool collisionDetected = false;
	std::set<std::pair<const void*, const void*>>* framePairs;

	GE_ContactResultCallback(std::set<std::pair<const void*, const void*>>* pairs)
		: framePairs(pairs) {
	}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1) override
	{
		if (colObj0Wrap == nullptr) return 0;
		if (colObj1Wrap == nullptr) return 0;

		const btCollisionObject* objA = colObj0Wrap->getCollisionObject();
		const btCollisionObject* objB = colObj1Wrap->getCollisionObject();

		if (objA == nullptr || objB == nullptr) return 0;

		void* obj0ptr = objA->getUserPointer();
		void* obj1ptr = objB->getUserPointer();

		if (obj0ptr == nullptr || obj1ptr == nullptr) return 0;

		auto key = MakePair(objA, objB);
		framePairs->insert(key);

		collisionDetected = true;

		return 0;
	}
};

#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

static std::set<std::pair<const void*, const void*>> prevFramePairs = std::set<std::pair<const void*, const void*>>();
static std::set<std::pair<const void*, const void*>> thisFramePairs = std::set<std::pair<const void*, const void*>>();

inline RCHit spherecast(btDiscreteDynamicsWorld* world, float originX, float originY, float originZ, float radius, float endX, float endY, float endZ, int layerGroup, int collisionMasks)
{
	RCHit result;
	world->computeOverlappingPairs();

	btSphereShape* sphere = new btSphereShape(radius);
	btVector3 startPos(originX, originY, originZ);
	btVector3 endPos(endX, endY, endZ);

	btTransform start, end;
	start.setIdentity();
	end.setIdentity();
	start.setOrigin(startPos);
	end.setOrigin(endPos);

	btCollisionWorld::ClosestConvexResultCallback callback(startPos, endPos);
	callback.m_collisionFilterGroup = layerGroup;
	callback.m_collisionFilterMask = collisionMasks;
	world->convexSweepTest(sphere, start, end, callback);

	if (callback.hasHit())
	{
		result.hit = callback.hasHit();
		result.gameObject = GetObjectFromPointer(callback.m_hitCollisionObject);

		result.position = { 
			callback.m_hitPointWorld.getX(), 
			callback.m_hitPointWorld.getY(), 
			callback.m_hitPointWorld.getZ()
		};

		result.normal = { 
			callback.m_hitNormalWorld.getX(), 
			callback.m_hitNormalWorld.getY(), 
			callback.m_hitNormalWorld.getZ() 
		};
	}

	delete sphere;
	return result;
}

inline std::vector<RCHit> raycastAll(btDiscreteDynamicsWorld* world, float originX, float originY, float originZ, float targetX, float targetY, float targetZ, int collisionGroup, int collisionMask)
{
	std::vector<RCHit> hits = std::vector<RCHit>();

	world->computeOverlappingPairs();

	btVector3 origin(originX, originY, originZ);
	btVector3 target(targetX, targetY, targetZ);

	btCollisionWorld::AllHitsRayResultCallback allHitsResult(origin, target);

	allHitsResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	allHitsResult.m_collisionFilterGroup = collisionGroup;
	allHitsResult.m_collisionFilterMask = collisionMask;

	world->rayTest(origin, target, allHitsResult);

	for (int i = 0; i < allHitsResult.m_collisionObjects.size(); i++)
	{
		RCHit hit = {};

		hit.hit = allHitsResult.hasHit();
		hit.gameObject = GetObjectFromPointer(allHitsResult.m_collisionObjects[i]);
		hit.position = { allHitsResult.m_hitPointWorld[i].x(), allHitsResult.m_hitPointWorld[i].y(), allHitsResult.m_hitPointWorld[i].z() };
		hit.normal = { allHitsResult.m_hitNormalWorld[i].x(), allHitsResult.m_hitNormalWorld[i].y(), allHitsResult.m_hitNormalWorld[i].z() };

		hits.push_back(
			hit
		);
	}

	return hits;
}

inline RCHit raycast(btDiscreteDynamicsWorld* world, float originX, float originY, float originZ, float targetX, float targetY, float targetZ, int collisionGroup, int collisionMask)
{
	world->computeOverlappingPairs();

	btVector3 origin(originX, originY, originZ);
	btVector3 target(targetX, targetY, targetZ);

	btCollisionWorld::ClosestRayResultCallback rayCallback(origin, target);
	rayCallback.m_collisionFilterGroup = collisionGroup;
	rayCallback.m_collisionFilterMask = collisionMask;

	world->rayTest(origin, target, rayCallback);

	return
	{
		{ 
			rayCallback.m_hitPointWorld.x(),
			rayCallback.m_hitPointWorld.y(),
			rayCallback.m_hitPointWorld.z()
		},
		{
			rayCallback.m_hitNormalWorld.x(),
			rayCallback.m_hitNormalWorld.y(),
			rayCallback.m_hitNormalWorld.z()
		},
		GetObjectFromPointer(rayCallback.m_collisionObject),
		rayCallback.hasHit()
	};
}

inline void setGravity(btDiscreteDynamicsWorld* world, float x, float y, float z)
{
	if (world == nullptr) return;

	world->setGravity(btVector3(x, y, z));
}

inline void testCollision(btDiscreteDynamicsWorld* world)
{
	SendPhysicsUpdateCallback();

	thisFramePairs.clear();

	auto objs = world->getCollisionObjectArray();


	for (int i = 0; i < objs.size(); ++i)
	{
		if (objs[i] == nullptr) continue;

		btCollisionObject* obj = objs[i];

		GE_ContactResultCallback result(&thisFramePairs);
		world->contactTest(obj, result);
	}

	// Handle collided & triggered
	for (auto& key : thisFramePairs)
	{
		const btCollisionObject* objA = static_cast<const btCollisionObject*>(key.first);
		const btCollisionObject* objB = static_cast<const btCollisionObject*>(key.second);

		void* obj0ptr = objA->getUserPointer();
		void* obj1ptr = objB->getUserPointer();

		if (!obj0ptr || !obj1ptr) continue;

		bool isTriggerA = (objA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

		bool isTriggerB = (objB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

		if (isTriggerA || isTriggerB)
		{
			SendOnTriggeredCallback(obj0ptr, obj1ptr);
		}
		else
		{
			SendOnCollidedCallback(obj0ptr, obj1ptr);
		}
	}

	// Handle new enters
	for (auto& key : thisFramePairs)
	{
		if (prevFramePairs.find(key) == prevFramePairs.end())
		{
			 const btCollisionObject* objA = static_cast<const btCollisionObject*>(key.first);
			 const btCollisionObject* objB = static_cast<const btCollisionObject*>(key.second);

			void* obj0ptr = objA->getUserPointer();
			void* obj1ptr = objB->getUserPointer();

			if (!obj0ptr || !obj1ptr) continue;

			bool isTriggerA = (objA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			bool isTriggerB = (objB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			if (isTriggerA || isTriggerB)
				SendTriggerEnterCallback(obj0ptr, obj1ptr);
			else
				SendObjectEnterCallback(obj0ptr, obj1ptr);
		}
	}

	// Handle stays
	for (auto& key : thisFramePairs)
	{
		if (prevFramePairs.find(key) != prevFramePairs.end())
		{
			const btCollisionObject* objA = static_cast<const btCollisionObject*>(key.first);
			const btCollisionObject* objB = static_cast<const btCollisionObject*>(key.second);

			void* obj0ptr = objA->getUserPointer();
			void* obj1ptr = objB->getUserPointer();

			if (!obj0ptr || !obj1ptr) continue;

			bool isTriggerA = (objA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			bool isTriggerB = (objB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			if (isTriggerA || isTriggerB)
				SendTriggerStayCallback(obj0ptr, obj1ptr);
			else
				SendObjectStayCallback(obj0ptr, obj1ptr);
		}
	}

	// Handle exits
	for (auto& key : prevFramePairs)
	{
		if (thisFramePairs.find(key) == thisFramePairs.end())
		{
			const btCollisionObject* objA = static_cast<const btCollisionObject*>(key.first);
			const btCollisionObject* objB = static_cast<const btCollisionObject*>(key.second);

			void* obj0ptr = objA->getUserPointer();
			void* obj1ptr = objB->getUserPointer();

			if (!obj0ptr || !obj1ptr) continue;


			bool isTriggerA = (objA->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			bool isTriggerB = (objB->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

			if (isTriggerA || isTriggerB)
				SendTriggerExitCallback(obj0ptr, obj1ptr);
			else
				SendObjectExitCallback(obj0ptr, obj1ptr);
		}
	}

	prevFramePairs = thisFramePairs;
}

inline int MakeGroup(int layerId) {
	return 1 << layerId;
}

inline int AddMask(int mask, int layerId) {
	return mask | (1 << layerId);
}

#pragma managed(pop)

PhysicsService::PhysicsService(String^ name, Engine::Internal::Components::Transform^ transform) 
	: Engine::EngineObjects::Script(name, transform),
	frameRate(60),
	maxSubSteps(10),
	Gravity(Engine::Components::Vector3(0, -9.81f, 0))
{

}

void PhysicsService::Awake()
{
	this->protectMember();

	collisionConfig = new btDefaultCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfig);
	bvhInterface = new btDbvtBroadphase();
	SQCsolver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(collisionDispatcher, bvhInterface, SQCsolver, collisionConfig);
	nativePhysicsService = new NativePhysicsService();
	btGImpactCollisionAlgorithm::registerAlgorithm(collisionDispatcher);
	setGravity(world, this->Gravity.x, this->Gravity.y, this->Gravity.z);

	Singleton<PhysicsService^>::Create(this);
	SharedInstance::Create("PhysicsService", this);

	if (collisionGroups->Count <= 0)
	{
		System::Collections::Generic::List<Engine::Components::Layer^>^ _layers = Engine::Scripting::LayerManager::GetLayers();
		
		for each (Engine::Components::Layer ^ layer in _layers)
		{
			InitializeCollision(layer);
			AddLayer(layer);
		}

		RecalculateCollisionGroups();
	}
}

void PhysicsService::Start()
{
	Engine::Scripting::LayerManager::onLayerAdded->connect(gcnew System::Action<Engine::Components::Layer^>(this, &PhysicsService::AddLayer));
	Engine::Scripting::LayerManager::onLayerRemoved->connect(gcnew System::Action<Engine::Components::Layer^>(this, &PhysicsService::RemoveLayer));

	physicsUpdateThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &PhysicsService::UpdatePhysicsThread));
	physicsUpdateThread->Start();
}

void PhysicsService::Update()
{
	processingPhysicsStart->Set();
}

void PhysicsService::AddPhysicsObject(Engine::EngineObjects::Physics::RigidBody^ rigidBody)
{
	std::lock_guard lock(physicsMutex);

	if (this == nullptr)
	{
		throw gcnew Exception("Physics Service is not initialized");
		return;
	}

	if (rigidBody != nullptr && rigidBody->getRigidBody() != nullptr && world != nullptr)
	{
		world->addRigidBody(rigidBody->getRigidBody());
	}
	else
		if (world == nullptr)
			printError("Physics World not instantiated");
		else if (rigidBody->getRigidBody() == nullptr)
			printError("Rigidbody not instantiated");
}

void PhysicsService::RemovePhysicsObject(Engine::EngineObjects::Physics::RigidBody^ rigidBody)
{
	std::lock_guard lock(physicsMutex);

	if (this == nullptr)
	{
		throw gcnew Exception("Physics Service is not initialized");
		return;
	}

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

void Engine::EngineObjects::Physics::PhysicsService::AddPhysicsObject(btRigidBody* collisionObject)
{
	std::lock_guard lock(physicsMutex);

	if (this == nullptr)
	{
		throw gcnew Exception("Physics Service is not initialized");
		return;
	}

	if (collisionObject != nullptr && world != nullptr)
	{
		world->addRigidBody(collisionObject);
	}
	else
		if (world == nullptr)
			printError("Physics World is not instantiated");
		else if (collisionObject == nullptr)
			printError("CollisionObject is not a pointer to an instance");
}

void Engine::EngineObjects::Physics::PhysicsService::RemovePhysicsObject(btRigidBody* collisionObject)
{
	std::lock_guard lock(physicsMutex);

	if (this == nullptr)
	{
		throw gcnew Exception("Physics Service is not initialized");
		return;
	}

	if (collisionObject != nullptr && world != nullptr)
	{
		world->removeRigidBody(collisionObject);
	}
	else
		if (world == nullptr)
			printError("Physics World is not instantiated");
		else if (collisionObject == nullptr)
			printError("CollisionObject is not a pointer to an instance");
}

void PhysicsService::AddCollisionObject(btCollisionObject* collisionObject)
{
	std::lock_guard lock(physicsMutex);

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
			printError("Physics World is not instantiated");
		else if (collisionObject == nullptr)
			printError("CollisionObject is not a pointer to an instance");
}

void PhysicsService::RemoveCollisionObject(btCollisionObject* collisionObject)
{
	std::lock_guard lock(physicsMutex);

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

bool Engine::EngineObjects::Physics::PhysicsService::Raycast(Engine::Components::Vector3 origin, Engine::Components::Vector3 to, unsigned int layer)
{
	std::lock_guard lock(physicsMutex);

	int group = MakeGroup(layer);
	int mask = 0;
	auto _collisionMasks = GetLayerCollisionMasks(Engine::Scripting::LayerManager::GetLayerFromId(layer));

	for each(int l in _collisionMasks->Keys)
	{
		if (_collisionMasks[l])
		{
			mask = AddMask(mask, l);
		}
	}

	RCHit _hit = raycast(world, origin.x, origin.y, origin.z, to.x, to.y, to.z, group, mask);
	return _hit.hit;
}

bool Engine::EngineObjects::Physics::PhysicsService::Raycast(Engine::Components::Vector3 from, Engine::Components::Vector3 to, unsigned int layer, [System::Runtime::InteropServices::OutAttribute] RaycastHit% instance)
{
	std::lock_guard lock(physicsMutex);

	int group = MakeGroup(layer);
	int mask = 0;
	auto _collisionMasks = GetLayerCollisionMasks(Engine::Scripting::LayerManager::GetLayerFromId(layer));

	for each(int l in _collisionMasks->Keys)
	{
		if (_collisionMasks[l])
		{
			mask = AddMask(mask, l);
		}
	}

	RCHit _hit = raycast(world, from.x, from.y, from.z, to.x, to.y, to.z, group, mask);
	RaycastHit hit = RaycastHit();
	hit.gameObject = _hit.gameObject;
	hit.hit = _hit.hit;
	hit.position = Engine::Components::Vector3(_hit.position[0], _hit.position[1], _hit.position[2]);
	hit.normal = Engine::Components::Vector3(_hit.normal[0], _hit.normal[1], _hit.normal[2]);
	
	instance = hit;
	return _hit.hit;
}

cli::array<RaycastHit>^ Engine::EngineObjects::Physics::PhysicsService::RaycastAll(Engine::Components::Vector3 from, Engine::Components::Vector3 to, unsigned int layer)
{
	cli::array<RaycastHit>^ hits;

	std::lock_guard lock(physicsMutex);

	int group = MakeGroup(layer);
	int mask = 0;
	auto _collisionMasks = GetLayerCollisionMasks(Engine::Scripting::LayerManager::GetLayerFromId(layer));

	for each (int l in _collisionMasks->Keys)
	{
		if (_collisionMasks[l])
		{
			mask = AddMask(mask, l);
		}
	}

	std::vector<RCHit> raycasts = raycastAll(world, from.x, from.y, from.z, to.x, to.y, to.z, group, mask);
	
	hits = gcnew cli::array<RaycastHit>(raycasts.size());
	
	for(int x = 0; x < raycasts.size(); x++)
	{
		RaycastHit hit = RaycastHit();
		hit.hit = raycasts[x].hit;
		hit.gameObject = raycasts[x].gameObject;
		hit.position = Engine::Components::Vector3(raycasts[x].position[0], raycasts[x].position[1], raycasts[x].position[2]);
		hit.normal = Engine::Components::Vector3(raycasts[x].normal[0], raycasts[x].normal[1], raycasts[x].normal[2]);

		hits[x] = hit;
	}

	return hits;
}

bool Engine::EngineObjects::Physics::PhysicsService::SphereCast(Engine::Components::Vector3 position, float radius, Engine::Components::Vector3 direction, float distance, unsigned int layer)
{
	std::lock_guard lock(physicsMutex);

	int group = MakeGroup(layer);
	int mask = 0;
	auto _collisionMasks = GetLayerCollisionMasks(Engine::Scripting::LayerManager::GetLayerFromId(layer));

	for each(int l in _collisionMasks->Keys)
	{
		if (_collisionMasks[l])
		{
			mask = AddMask(mask, l);
		}
	}

	Engine::Components::Vector3 dirNorm = direction.Normalized();
	Engine::Components::Vector3 endPos = position + dirNorm * distance;

	RCHit hit = spherecast(world, position.x, position.y, position.z, radius, endPos.x, endPos.y, endPos.z, group, mask);

	return hit.hit;
}

bool Engine::EngineObjects::Physics::PhysicsService::SphereCast(Engine::Components::Vector3 position, float radius, Engine::Components::Vector3 direction, float distance, unsigned int layer, RaycastHit% instance)
{
	std::lock_guard lock(physicsMutex);

	int group = MakeGroup(layer);
	int mask = 0;
	auto _collisionMasks = GetLayerCollisionMasks(Engine::Scripting::LayerManager::GetLayerFromId(layer));

	for each(int l in _collisionMasks->Keys)
	{
		if (_collisionMasks[l])
		{
			mask = AddMask(mask, l);
		}
	}

	Engine::Components::Vector3 dirNorm = direction.Normalized();
	Engine::Components::Vector3 endPos = position + dirNorm * distance;

	RCHit _hit = spherecast(world, position.x, position.y, position.z, radius, endPos.x, endPos.y, endPos.z, group, mask);

	RaycastHit hit = RaycastHit();
	hit.gameObject = _hit.gameObject;
	hit.hit = _hit.hit;
	hit.position = Engine::Components::Vector3(_hit.position[0], _hit.position[1], _hit.position[2]);
	hit.normal = Engine::Components::Vector3(_hit.normal[0], _hit.normal[1], _hit.normal[2]);

	instance = hit;

	return _hit.hit;
}

bool Engine::EngineObjects::Physics::PhysicsService::ContactTest(
	Engine::Internal::Components::GameObject^ object0, 
	Engine::Internal::Components::GameObject^ object1, 
	[System::Runtime::InteropServices::OutAttribute] HitTest% outNormal)
{
	if (object0->getCollisionShape() == nullptr || object1->getCollisionShape() == nullptr)
	{
		if (object0->getCollisionShape() == nullptr) throw gcnew System::NullReferenceException(object0->name + " does not have a Collision Shape.");
		if (object1->getCollisionShape() == nullptr) throw gcnew System::NullReferenceException(object1->name + " does not have a Collision Shape.");
	}

	Engine::Native::CollisionShape* shape0 = (Engine::Native::CollisionShape*)object0->getCollisionShape();
	Engine::Native::CollisionShape* shape1 = (Engine::Native::CollisionShape*)object1->getCollisionShape();

	if (!shape0->hasCollisionObject() || !shape1->hasCollisionObject())
	{
		if (!shape0->hasCollisionObject()) throw gcnew System::NullReferenceException(object0->name + " does not have a btCollisionObject*");
		if (!shape1->hasCollisionObject()) throw gcnew System::NullReferenceException(object1->name + " does not have a btCollisionObject*");
	}

	HTest hTest;
	HitTest hitTest;
	GE_SingleContactResultCallback callback(hTest);

	{
		std::lock_guard lock(physicsMutex);
		world->contactPairTest(shape0->getCollisionObject(), shape1->getCollisionObject(), callback);
	}

	hitTest.Point = Engine::Components::Vector3(hTest.hitPoint[0], hTest.hitPoint[1], hTest.hitPoint[2]);
	hitTest.Normal = Engine::Components::Vector3(hTest.hitNormal[0], hTest.hitNormal[1], hTest.hitNormal[2]);
	hitTest.Correction = Engine::Components::Vector3(hTest.correction[0], hTest.correction[1], hTest.correction[2]);
	hitTest.PenetrationDepth = hTest.depth;

	return callback.collisionDetected;
}

bool Engine::EngineObjects::Physics::PhysicsService::CanCollide(Engine::Components::Layer^ layerMask, Engine::Components::Layer^ collisionMask)
{
	bool value = false;

	System::Collections::Generic::Dictionary<int, bool>^ innerDict;

	if (collisionGroups->TryGetValue(layerMask->layerMask, innerDict))
	{
		if (innerDict->ContainsKey(collisionMask->layerMask))
			value = innerDict[collisionMask->layerMask];
	}

	return value;
}

System::Collections::Generic::Dictionary<int, bool>^ Engine::EngineObjects::Physics::PhysicsService::GetLayerCollisionMasks(Engine::Components::Layer^ layerMask)
{
	return collisionGroups[layerMask->layerMask];
}

void Engine::EngineObjects::Physics::PhysicsService::SetCollide(Engine::Components::Layer^ layerMask, Engine::Components::Layer^ collisionMask, bool interacts)
{
	System::Collections::Generic::Dictionary<int, bool>^ innerDict;

	if (collisionGroups->TryGetValue(layerMask->layerMask, innerDict))
	{
		if (innerDict->ContainsKey(collisionMask->layerMask))
		{
			innerDict[collisionMask->layerMask] = interacts;
		}
		else
		{
			innerDict->Add(collisionMask->layerMask, interacts);
		}
	}
	else
	{
		AddLayer(layerMask);
		SetCollide(layerMask, collisionMask, interacts);
	}

	RecalculateCollisionGroups();
}

void Engine::EngineObjects::Physics::PhysicsService::InitializeCollision(Engine::Components::Layer^ layerMask)
{
	this->collisionGroups->Add(layerMask->layerMask, gcnew System::Collections::Generic::Dictionary<int, bool>());
}

void Engine::EngineObjects::Physics::PhysicsService::AddLayer(Engine::Components::Layer^ newLayer)
{
	auto newDict = gcnew System::Collections::Generic::Dictionary<
		int, bool>();

	for each (auto kvp in collisionGroups)
	{
		auto existingLayer = kvp.Key;
		auto existingDict = kvp.Value;

		existingDict[newLayer->layerMask] = true;
		newDict[existingLayer] = true;
	}

	newDict[newLayer->layerMask] = true;

	collisionGroups[newLayer->layerMask] = newDict;
}

void Engine::EngineObjects::Physics::PhysicsService::RemoveLayer(Engine::Components::Layer^ layerMask)
{
	collisionGroups->Remove(layerMask->layerMask);

	for each (auto kvp in collisionGroups)
	{
		kvp.Value->Remove(layerMask->layerMask);
	}
}

void Engine::EngineObjects::Physics::PhysicsService::RecalculateCollisionGroups()
{
	if (world != nullptr)
	{
		auto objs = world->getCollisionObjectArray();

		for (int x = 0; x < objs.size(); x++) 
		{
			auto obj = objs[x];

			if (obj == nullptr) continue;

			GameObject^ instance = GetObjectFromPointer(obj);

			if (instance == nullptr) continue;

			auto _collisionMasks = GetLayerCollisionMasks(instance->layerMask);
			int group = MakeGroup(instance->layerMask->layerMask);
			int mask = 0;

			for each(int l in _collisionMasks->Keys)
			{
				if (_collisionMasks[l])
				{
					mask = AddMask(mask, l);
				}
			}

			obj->getBroadphaseHandle()->m_collisionFilterGroup = group;
			obj->getBroadphaseHandle()->m_collisionFilterMask = mask;

			world->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
				obj->getBroadphaseHandle(), 
				world->getDispatcher()
			);
			
			obj->activate();
		}
	}
}

void Engine::EngineObjects::Physics::PhysicsService::resampleAABB()
{
	if (this == nullptr) return;

	this->updateAABBs = true;
}

NativePhysicsService* PhysicsService::getNativePhysicsService()
{
	return this->nativePhysicsService;
}

void Engine::EngineObjects::Physics::PhysicsService::Destroy()
{
	processingPhysicsFinished = true;
	physicsUpdateThread->Join();

	btAlignedObjectArray<btRigidBody*> rigidBodies = world->getNonStaticRigidBodies();

	for (int i = 0; i < rigidBodies.size(); i++)
	{
		btRigidBody* rigidBody = rigidBodies[i];
		if (rigidBody == nullptr) continue;

		world->removeRigidBody(rigidBody);
		delete rigidBody->getCollisionShape();
		delete rigidBody;
	}

	btCollisionObjectArray& collObjArray = world->getCollisionObjectArray();
	for (int x = 0; x < collObjArray.size(); x++)
	{
		btCollisionObject* collisionPtr = collObjArray[x];

		if (collisionPtr == nullptr) continue;

		world->removeCollisionObject(collisionPtr);
		delete collisionPtr->getCollisionShape();
		delete collisionPtr;
	}

	delete collisionConfig;
	delete collisionDispatcher;
	delete bvhInterface;
	delete SQCsolver;
	delete world;
	delete nativePhysicsService;
}

std::mutex& Engine::EngineObjects::Physics::PhysicsService::RequestMutex()
{
	return physicsMutex;
}

int Engine::EngineObjects::Physics::PhysicsService::GetCollisionMask(Engine::Components::Layer^ layer)
{
	int mask = 0;
	auto _masks = collisionGroups[layer->layerMask];

	for each (int maskId in _masks->Keys)
	{
		if (_masks[maskId])
			mask = AddMask(mask, maskId);
	}
	
	return mask;
}

int Engine::EngineObjects::Physics::PhysicsService::GetCollisionGroup(int collisionGroup)
{
	return MakeGroup(collisionGroup);
}

int Engine::EngineObjects::Physics::PhysicsService::GetCollisionMask(int mask, int layerId)
{
	return AddMask(mask, layerId);
}

void Engine::EngineObjects::Physics::PhysicsService::UpdatePhysicsThread()
{
	while (true)
	{
		try
		{
			if (processingPhysicsFinished)
				break;

			processingPhysicsStart->WaitOne();

			if (maxSubSteps <= 0)
				maxSubSteps = 1;

			{
				std::lock_guard lock(physicsMutex);

				if (updateAABBs)
				{
					updateAABBs = false;
					world->updateAabbs();
				}
				RecalculateCollisionGroups();

				setGravity(world, this->Gravity.x, this->Gravity.y, this->Gravity.z);
				world->stepSimulation((1.0f / frameRate), (maxSubSteps > 0 ? maxSubSteps : 1));
				testCollision(world);
			}

			System::Threading::Thread::Sleep((1000.0f / frameRate));
			processingPhysicsDone->Set();
		}
		catch (std::exception ex)
		{
			print("[PHYS SERVICE EXCEPTION]:", gcnew String(ex.what()));
		}
		catch (Exception^ ex)
		{
			print("[PHYS SERVICE EXCEPTION]:", ex->Message);
		}
	}
}

#endif