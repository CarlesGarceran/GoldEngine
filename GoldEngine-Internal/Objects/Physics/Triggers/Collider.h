#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	public ref class Collider abstract : Engine::EngineObjects::Script
	{
	protected:
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::ReadOnly)]
		Enums::ColliderShape colliderShape;
		bool registered = false;

	public:
		[Engine::Scripting::PropertyAttribute]
		Engine::Components::Color^ wireColor;

		[Engine::Scripting::PropertyAttribute]
		bool renderWires = false;

		[Engine::Scripting::PropertyAttribute]
		Enums::ColliderType collisionType;
		/*
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int modelId;

		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)]
		unsigned int meshIndex;
		*/
	private:
		/*
		Engine::Native::EnginePtr<RAYLIB::Model>* modelInstance = nullptr;
		Engine::Native::EnginePtr<RAYLIB::Mesh>* meshInstance = nullptr;
		*/
	private:
		void onColliderShapeChanged(Enums::ColliderShape newShape, Enums::ColliderShape oldShape);

	public:
		Collider(String^ name, Engine::Internal::Components::Transform^ transform);

		virtual void Start() override;
		virtual void DrawGizmo() override;
		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;
	};
}

#endif