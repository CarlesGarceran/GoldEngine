#pragma once

#ifdef USE_BULLET_PHYS

namespace Engine::EngineObjects::Physics
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	public ref class Collider abstract : Engine::EngineObjects::Script
	{
	protected:
		[Engine::Scripting::SerializePropertyAttribute(Engine::Scripting::AccessLevel::ReadOnly)]
		Enums::ColliderShape colliderShape;
		bool registered = false;

		GameObject^ root;

	public:
		[Engine::Scripting::SerializePropertyAttribute] Engine::Components::Vector3 origin = Engine::Components::Vector3::Zero();
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ wireColor;
		[Engine::Scripting::PropertyAttribute] bool renderWires = false;
		[Engine::Scripting::SerializePropertyAttribute] Enums::ColliderType collisionType;

		[Newtonsoft::Json::JsonIgnoreAttribute]
		Engine::Scripting::Events::Event^ HitBegin = Engine::Scripting::Events::Event::New();

		[Newtonsoft::Json::JsonIgnoreAttribute]
		Engine::Scripting::Events::Event^ Hit = Engine::Scripting::Events::Event::New();

		[Newtonsoft::Json::JsonIgnoreAttribute]
		Engine::Scripting::Events::Event^ HitEnded = Engine::Scripting::Events::Event::New();

	public:
		Collider();

		virtual void Start() override;
		virtual void DrawGizmo() override;
		[Engine::Attributes::ExecuteInEditModeAttribute] virtual void Update() override;

		virtual void Destroy() override;

		virtual bool ClaimOwnership(GameObject^ instance) = 0;
		virtual void Disown() = 0;
		virtual bool IsOwned() = 0;
		virtual void CreateShape() = 0;

		virtual void OnCollisionEnter(GameObject^ instance) override;
		virtual void OnCollisionStay(GameObject^ instance) override;
		virtual void OnCollisionExit(GameObject^ instance) override;
		virtual void OnCollided(GameObject^ instance) override;


		virtual void OnTriggerEnter(GameObject^ instance) override;
		virtual void OnTriggerStay(GameObject^ instance) override;
		virtual void OnTriggerExit(GameObject^ instance) override;
		virtual void OnTriggered(GameObject^ instance) override;

	protected:
		virtual void OnCollisionTypeChanged(Enums::ColliderType newType, Enums::ColliderType oldType) = 0;
		virtual void OnOriginChanged(Engine::Components::Vector3 newValue, Engine::Components::Vector3 oldValue) = 0;
	};
}

#endif