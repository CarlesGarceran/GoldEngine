#pragma once

#ifdef USE_BULLET_PHYS

UNMANAGED_BEGIN

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

UNMANAGED_END

namespace Engine::EngineObjects::Physics
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class BoxCollider : Engine::EngineObjects::Physics::Collider
	{
	private:
		void* originalCollisionShape;
		Engine::Native::EnginePtr<RAYLIB::Model>* cachedModel;

	public:
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::Public)] Engine::Components::Vector3 extents = Engine::Components::Vector3::Zero();

	public:
		BoxCollider(String^ name, Engine::Internal::Components::Transform^ transform);
		BoxCollider();

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
		void RenderGuizmos(btCollisionObject* cObj);

		void OnExtentsChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue);
		void OnParentChanged(String^ property, System::Object^ transform, System::Object^ oldTransform);
	};
}

#endif