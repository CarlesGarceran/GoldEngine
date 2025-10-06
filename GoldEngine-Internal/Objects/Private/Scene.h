#pragma once

namespace Engine::EngineObjects::Private
{
	[MoonSharp::Interpreter::MoonSharpUserDataAttribute]
	[Engine::Attributes::LuaAPIAttribute]
	private ref class Scene : Engine::EngineObjects::Script
	{
	private:
		Engine::Management::Scene^ scenePtr;

	public:
		[Engine::Scripting::PropertyAttribute]
		String^ sceneName;

		[Engine::Scripting::PropertyAttribute]
		Engine::Components::Color^ skyColor;

		System::Collections::Generic::List<Engine::Components::Layer^>^ layerMasks;

	public:
		Scene(String^ name, Engine::Internal::Components::Transform^ transform);

		void Awake() override;

		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;

		GameObject^ GetService(System::String^ serviceName);
		GameObject^ Service(System::String^ serviceName) { return GetService(serviceName); }
	};
}
