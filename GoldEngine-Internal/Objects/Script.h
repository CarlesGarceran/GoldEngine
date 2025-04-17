#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Script : public Engine::EngineObjects::ScriptBehaviour
	{
	public:
		Script(System::String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}
		}

		Script() : Engine::EngineObjects::ScriptBehaviour("Script", gcnew Engine::Internal::Components::Transform(Engine::Components::Vector3::zero(), Engine::Components::Vector3::zero(), gcnew Engine::Components::Vector3(1,1,1), nullptr))
		{
			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}
		}

		virtual void Init() override
		{
			Engine::EngineObjects::ScriptBehaviour::Setup();

			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}

			System::GC::Collect();
		}

		virtual void Setup() override
		{
			Engine::EngineObjects::ScriptBehaviour::Setup();

			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}

			System::GC::Collect();
		}
	};
}