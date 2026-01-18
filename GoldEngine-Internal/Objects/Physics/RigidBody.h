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
	public ref class RigidBody : Engine::EngineObjects::Script
	{
	public:
		// RIGIDBODY PROPERTIES
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)] bool Kinematic;
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)] float Mass;

		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::ReadOnly)] float angularDamping;
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::ReadOnly)] Engine::Components::Vector3 angularVelocity = Engine::Components::Vector3::Zero();

	public:
		void Awake() override;
		void Start() override;

		void Update() override;
		
		void Draw() override;

		void Destroy() override;

		void OnInactive() override;
		void OnActive() override;

		void AddForce(Engine::Components::Vector3 direction, Enums::ForceMode mode);
		void addForce(Engine::Components::Vector3 direction, Enums::ForceMode mode) { return AddForce(direction, mode); }

		void AddTorque(Engine::Components::Vector3 direction, Enums::ForceMode mode);
		void addTorque(Engine::Components::Vector3 direction, Enums::ForceMode mode) { return AddTorque(direction, mode); }

		void SetAngularVelocity(Engine::Components::Vector3 velocity) { setAngularVelocity(velocity); }
		void setAngularVelocity(Engine::Components::Vector3 velocity);

		void SetLinearVelocity(Engine::Components::Vector3 velocity) { setLinearVelocity(velocity); }
		void setLinearVelocity(Engine::Components::Vector3 velocity);

		Engine::Components::Vector3 GetAngularVelocity() { return angularVelocity; }
		Engine::Components::Vector3 getAngularVelocity() { return GetAngularVelocity(); }

		Engine::Components::Vector3 GetLinearVelocity();
		Engine::Components::Vector3 getLinearVelocity() { return GetLinearVelocity(); }


		void ClearForces() { clearForces(); }
		void clearForces();

		Engine::Components::Vector3 GetCenterOfMass() { return getCenterOfMass(); }
		Engine::Components::Vector3 getCenterOfMass();

		Engine::Components::Vector3 GetTotalForce() { return getCenterOfMass(); }
		Engine::Components::Vector3 getTotalForce();

		Engine::Components::Vector3 GetTotalTorque() { return getCenterOfMass(); }
		Engine::Components::Vector3 getTotalTorque();

		void SetBounciness(float bounciness);
		float GetBounciness();

		void setBounciness(float bounciness) { return SetBounciness(bounciness); }
		float getBounciness() { return GetBounciness(); }

		void SetFriction(float friction);
		float GetFriction();

		void setFriction(float friction) { return SetFriction(friction); }
		float getFriction() { return GetFriction(); }

		void SetLinearDamping(float value);
		void setLinearDamping(float value) { SetLinearDamping(value); }

		void SetAngularDamping(float value);
		void setAngularDamping(float value) { SetAngularDamping(value); }

		void setDamping(float linearDamping, float angularDamping) { SetDamping(linearDamping, angularDamping); }
		void SetDamping(float linearDamping, float angularDamping);

		float getLinearDamping() { return GetLinearDamping(); }
		float GetLinearDamping();

		float getAngularDamping() { return GetAngularDamping(); }
		float GetAngularDamping();

		Engine::Components::Vector3 getAngularFactor() { return GetAngularFactor(); }
		Engine::Components::Vector3 GetAngularFactor();

		void setAngularFactor(Engine::Components::Vector3 axis) { SetAngularFactor(axis); }
		void SetAngularFactor(Engine::Components::Vector3 axis);

		virtual void OnCollisionEnter(GameObject^ instance) override;
		virtual void OnCollisionStay(GameObject^ instance) override;
		virtual void OnCollisionExit(GameObject^ instance) override;

		virtual void OnTriggerEnter(GameObject^ instance) override;
		virtual void OnTriggerStay(GameObject^ instance) override;
		virtual void OnTriggerExit(GameObject^ instance) override;

		virtual void OnCollided(GameObject^ instance) override;
		virtual void OnTriggered(GameObject^ instance) override;


		void RecalculateInertia();

	internal:
		btRigidBody* getRigidBody();

	private:
		void createRigidBody();
		void PropertyChanged(String^ arg, Object^ _new, Object^ old);
		void onMassChanged(float newValue, float oldValue);
		void onKinematicChanged(bool newValue, bool oldValue);
		void reloadRigidbody();

		bool registered = false;
	};
}

#endif