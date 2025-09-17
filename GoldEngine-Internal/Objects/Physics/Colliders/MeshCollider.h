#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class MeshCollider : Engine::EngineObjects::Physics::Collider
	{
	private:
		void* originalCollisionShape;

	public:
		[Engine::Scripting::PropertyAttribute(Engine::Scripting::AccessLevel::Public)] Engine::EngineObjects::Physics::Enums::MeshCollisionType meshCollisionType = Engine::EngineObjects::Physics::Enums::MeshCollisionType::Convex;


	public:
		MeshCollider(String^ name, Engine::Internal::Components::Transform^ transform);
		MeshCollider();

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
		void OnOriginChanged(Engine::Components::Vector3 newOrigin, Engine::Components::Vector3 oldOrigin) override;

	private:
		void OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform);
		void OnMeshCollisionTypeChanged(Engine::EngineObjects::Physics::Enums::MeshCollisionType newValue, Engine::EngineObjects::Physics::Enums::MeshCollisionType oldValue);
	};
}

#endif