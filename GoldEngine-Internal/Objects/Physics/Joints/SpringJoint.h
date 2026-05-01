#pragma once

namespace Engine::EngineObjects::Physics::Joints
{
	public ref class SpringJoint : public Engine::EngineObjects::Script
	{
	private:
		btGeneric6DofSpringConstraint* springConstraint;

	public:
		Engine::Scripting::InstanceReference<Engine::EngineObjects::Physics::RigidBody^> rb0;
		Engine::Scripting::InstanceReference<Engine::EngineObjects::Physics::RigidBody^> rb1;

		void Awake() override;
	};
}