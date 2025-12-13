#pragma once

#ifdef USE_BULLET_PHYS

#pragma managed(push, off)
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#pragma managed(pop)

namespace Engine::EngineObjects::Physics
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
	public value struct RaycastHit
	{
		Engine::Components::Vector3 position;
		Engine::Components::Vector3 normal;
		GameObject^ gameObject;
		bool hit;

	private:
		RaycastHit(int x)
		{
			position = Engine::Components::Vector3(0, 0, 0);
			normal = Engine::Components::Vector3(0, 0, 0);
			gameObject = nullptr;
			hit = false;
		}

	public:
		static RaycastHit New() { return RaycastHit(0); }
		static RaycastHit Create() { return RaycastHit(0); }
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public value struct HitTest
	{
	public:
		Engine::Components::Vector3 Point;
		Engine::Components::Vector3 Normal;
		Engine::Components::Vector3 Correction;
		float PenetrationDepth;

	private:
		HitTest(int x)
		{
			Point = Engine::Components::Vector3(0, 0, 0);
			Normal = Engine::Components::Vector3(0, 0, 0);
			Correction = Engine::Components::Vector3(0, 0, 0);
			PenetrationDepth = 0;
		}

	public:
		static HitTest New() { return HitTest(0); }
		static HitTest Create() { return HitTest(0); }
	};

	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class PhysicsService : public Engine::EngineObjects::Script
	{
	private:
		btDefaultCollisionConfiguration* collisionConfig;
		btCollisionDispatcher* collisionDispatcher;
		btBroadphaseInterface* bvhInterface;
		btSequentialImpulseConstraintSolver* SQCsolver;
		Engine::EngineObjects::Physics::Native::NativePhysicsService* nativePhysicsService;
		btDiscreteDynamicsWorld* world;

		System::Threading::Thread^ physicsUpdateThread;
		System::Threading::AutoResetEvent^ processingPhysicsStart = gcnew System::Threading::AutoResetEvent(false);
		System::Threading::AutoResetEvent^ processingPhysicsDone = gcnew System::Threading::AutoResetEvent(false);
		bool processingPhysicsFinished;
		
		bool updateAABBs;
		bool pauseProcessing;

	public:
		[Engine::Scripting::PropertyAttribute] Engine::Components::Vector3 Gravity = Engine::Components::Vector3(0, -9.81f, 0);
		[Engine::Scripting::PropertyAttribute] float frameRate;
		[Engine::Scripting::PropertyAttribute] int maxSubSteps;

		System::Collections::Generic::Dictionary<int, System::Collections::Generic::Dictionary<int, bool>^>^ collisionGroups = gcnew System::Collections::Generic::Dictionary<int, System::Collections::Generic::Dictionary<int, bool>^>();

	public:
		PhysicsService();

	public:
		void Awake() override;
		void Start() override;

		void Update() override;

		void AddPhysicsObject(Engine::EngineObjects::Physics::RigidBody^);
		void RemovePhysicsObject(Engine::EngineObjects::Physics::RigidBody^);

		void AddPhysicsObject(btRigidBody*);
		void RemovePhysicsObject(btRigidBody*);

		void AddCollisionObject(btCollisionObject*);
		void RemoveCollisionObject(btCollisionObject*);

		bool Raycast(Engine::Components::Vector3 from, Engine::Components::Vector3 to, unsigned int layer);
		bool Raycast(Engine::Components::Vector3 from, Engine::Components::Vector3 to, unsigned int layer, [System::Runtime::InteropServices::OutAttribute] RaycastHit% instance);

		cli::array<RaycastHit>^ RaycastAll(Engine::Components::Vector3 from, Engine::Components::Vector3 to, unsigned int layer);

		bool SphereCast(Engine::Components::Vector3 position, float radius, Engine::Components::Vector3 direction, float distance, unsigned int layer);
		bool SphereCast(Engine::Components::Vector3 position, float radius, Engine::Components::Vector3 direction, float distance, unsigned int layer, [System::Runtime::InteropServices::OutAttribute] RaycastHit% instance);

		bool ContactTest(Engine::Internal::Components::GameObject^ object0, Engine::Internal::Components::GameObject^ object1, [System::Runtime::InteropServices::OutAttribute] HitTest% outNormal);

		bool CanCollide(Engine::Components::Layer^ layerMask, Engine::Components::Layer^ collisionMask);
		System::Collections::Generic::Dictionary<int, bool>^ GetLayerCollisionMasks(Engine::Components::Layer^ layerMask);
		void SetCollide(Engine::Components::Layer^ layerMask, Engine::Components::Layer^ collisionMask, bool canCollide);

		void resampleAABB();

		Engine::EngineObjects::Physics::Native::NativePhysicsService* getNativePhysicsService();

		void Destroy() override;

		std::mutex& RequestMutex();

		int GetCollisionGroup(Engine::Components::Layer^ layer) { return GetCollisionGroup(layer->layerMask); }
		int GetCollisionMask(int mask, Engine::Components::Layer^ layer) { return GetCollisionMask(mask, layer->layerMask); }
		int GetCollisionMask(Engine::Components::Layer^ layer);

		int GetCollisionGroup(int collisionGroup);
		int GetCollisionMask(int mask, int layerId);


	public: // STATIC METHODS

	private:
		void UpdatePhysicsThread();

		void InitializeCollision(Engine::Components::Layer^ layerMask);


		void AddLayer(Engine::Components::Layer^ layerMask);
		void RemoveLayer(Engine::Components::Layer^ layerMask);
		void RecalculateCollisionGroups();
	};
}

#endif