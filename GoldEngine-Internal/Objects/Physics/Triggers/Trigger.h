#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	public enum class ColliderShape
	{
		Box,
		Sphere,
		Mesh
	};

	public ref class Trigger : Engine::EngineObjects::Script
	{
	public:
		[Engine::Scripting::PropertyAttribute]
		ColliderShape colliderShape;

		[Engine::Scripting::PropertyAttribute]
		Engine::Components::Color^ wireColor;

		[Engine::Scripting::PropertyAttribute]
		bool renderWires = false;

	private:
		void onColliderShapeChanged(ColliderShape newShape, ColliderShape oldShape);

	public:
		Trigger(String^ name, Engine::Internal::Components::Transform^ transform);

		void Start() override;
		void DrawGizmo() override;
		void Update() override;
	};
}

#endif