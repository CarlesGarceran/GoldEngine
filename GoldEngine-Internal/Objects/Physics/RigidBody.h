#pragma once


#ifdef USE_BULLET_PHYS

#pragma managed(push, off)
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#pragma managed(pop)

namespace Engine::EngineObjects::Physics
{
	public enum class ForceMode
	{
		Impulse,
		Force
	};

	public ref class RigidBody : Engine::EngineObjects::Script
	{
	private:
		btRigidBody* rigidBody;
		void onModelIdChanged(unsigned int newValue, unsigned int oldValue);
		void onMeshIdChanged(unsigned int newValue, unsigned int oldValue);

	public:
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		GameObject^ hookedObject;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		CollisionType collisionType;

		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int modelId;
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int meshId;

	public:
		RigidBody(String^ name, Engine::Internal::Components::Transform^ transform);

		void Start() override;

		void Update() override;

		void OnUnactive() override;
		void OnActive() override;

		void AddForce(Engine::Components::Vector3^ position, ForceMode mode);
		void addForce(Engine::Components::Vector3^ position, ForceMode mode) { return AddForce(position, mode); }

		btRigidBody* getRigidBody();


	private:
		void createRigidBody();
	};
}

#endif