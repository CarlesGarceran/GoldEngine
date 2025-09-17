#pragma once

namespace Engine::EngineObjects::Geometry::Abstract
{
	public ref class Renderer abstract : public Engine::EngineObjects::Script
	{
	public:
		Renderer();
		Renderer(String^ name, Engine::Internal::Components::Transform^ transform);



		virtual RAYLIB::Model& GetModel() = 0;
	};
}