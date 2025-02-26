#pragma once

namespace Engine::EngineObjects::Private
{
	private ref class Scene : Engine::EngineObjects::Script
	{
	private:
		Engine::Management::Scene^ scenePtr;


	public:
		[Engine::Scripting::PropertyAttribute]
		String^ sceneName;

		[Engine::Scripting::PropertyAttribute]
		Engine::Components::Color^ skyColor;

	public:
		Scene(String^ name, Engine::Internal::Components::Transform^ transform);


		[Engine::Attributes::ExecuteInEditModeAttribute]
		void Update() override;
	};
}
