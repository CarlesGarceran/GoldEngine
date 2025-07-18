#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	public ref class BoxCollider : Engine::EngineObjects::Physics::Collider
	{



	public:
		BoxCollider(String^ name, Engine::Internal::Components::Transform^ transform);

		void Start() override;
		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;
		void DrawGizmo() override;
	};
}

#endif