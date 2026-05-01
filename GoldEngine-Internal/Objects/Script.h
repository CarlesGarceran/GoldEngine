#pragma once

namespace Engine::EngineObjects
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	public ref class Script : public Engine::EngineObjects::ScriptBehaviour
	{
	private:
		Script(System::String^ name, Engine::Internal::Components::Transform^ transform) : Engine::EngineObjects::ScriptBehaviour(name, transform)
		{
			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}
		}

	public:
		Script() : Engine::EngineObjects::ScriptBehaviour("Script", gcnew Engine::Internal::Components::Transform(Engine::Components::Vector3::zero(), Engine::Components::Quaternion::Identity, Engine::Components::Vector3(1,1,1), nullptr))
		{
			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}
		}

		void Init() override
		{
			/*
			Engine::EngineObjects::ScriptBehaviour::Init();

			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}

			System::GC::Collect();
			*/
		}

		void Setup() override
		{
			Engine::EngineObjects::ScriptBehaviour::Setup();

			for each (Engine::Scripting::Attribute ^ attrib in attributes->attributes)
			{
				Singleton<Engine::Scripting::ObjectManager^>::Instance->PatchAttributeObject(attrib);
				attrib->synchronizeDescriptor();
			}

			System::GC::Collect();
		}

		Engine::Scripting::Events::Event^ GetPropertyChangedEvent(System::String^ propertyName)
		{
			if (HasProperty(propertyName)) 
			{
				return attributes->getAttribute(propertyName)->onPropertyChanged;
			}

			return nullptr;
		}

		bool HasProperty(System::String^ propertyName)
		{
			return attributes->hasAttribute(propertyName);
		}

		generic <class T>
		T GetProperty(System::String^ propertyName)
		{
			if (HasProperty(propertyName))
			{
				return (T)this->attributes->getAttribute(propertyName)->getValue();
			}

			return T();
		}

		System::Object^ GetProperty(System::String^ propertyName)
		{
			if (HasProperty(propertyName))
			{
				return this->attributes->getAttribute(propertyName)->getValue();
			}

			return nullptr;
		}

		void SetProperty(System::String^ propertyName, System::Object^ newValue)
		{
			if (HasProperty(propertyName))
			{
				attributes->getAttribute(propertyName)->setValue(newValue);
			}
			else
			{
				attributes->addAttribute(propertyName, newValue);
			}
		}
	};
}