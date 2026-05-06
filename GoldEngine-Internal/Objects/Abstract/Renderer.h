#pragma once

namespace Engine::EngineObjects::Geometry::Abstract
{
	public ref class Renderer abstract : public Engine::EngineObjects::Script
	{
	public:
		[Engine::Scripting::PropertyAttribute] Engine::Components::Color^ Tint = Engine::Components::Color::New(0xFFFFFFFF);

		virtual RAYLIB::Model& GetModel() = 0;
		virtual RAYLIB::Model* GetModelPtr() = 0;

		virtual property Engine::Components::Material^ sharedMaterial { Engine::Components::Material^ get() = 0; }
		virtual property cli::array<Engine::Components::Material^>^ sharedMaterials { cli::array<Engine::Components::Material^>^ get() = 0; }
	};
}