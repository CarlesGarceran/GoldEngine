#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
		[Engine::Attributes::LuaAPIAttribute]
		public ref class CapsuleCollider : Engine::EngineObjects::Physics::Collider
	{
	public:
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)] float radius;
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)] float height;

	private:
		[Engine::Scripting::SerializePropertyAttribute] float gizmo_slices;
		[Engine::Scripting::SerializePropertyAttribute] float gizmo_rings;

	public:
		CapsuleCollider();

		void Awake() override;
		void Start() override;
		[Engine::Attributes::ExecuteInEditModeAttribute] void Update() override;
		void DrawGizmo() override;
		void Destroy() override;

		bool IsOwned() override;
		bool ClaimOwnership(GameObject^ instance) override;
		void CreateShape() override;
		void Disown() override;

	protected:
		void OnCollisionTypeChanged(Enums::ColliderType newType, Enums::ColliderType oldType) override;
		void OnOriginChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue) override;

	private:
		void OnRadiusChanged(float newValue, float oldValue);
		void OnHeightChanged(float newValue, float oldValue);
		void OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform);
	};
}

#endif