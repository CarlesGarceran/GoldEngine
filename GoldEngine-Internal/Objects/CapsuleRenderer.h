#pragma once

namespace Engine::EngineObjects::Geometry
{
	public ref class CapsuleRenderer : public Engine::EngineObjects::Geometry::Abstract::Renderer
	{
	public:
		[Engine::Scripting::PropertyAttribute] float Height = 2;
		[Engine::Scripting::PropertyAttribute] float Radius = 1;
		[Engine::Scripting::PropertyAttribute] int Slices = 16;
		[Engine::Scripting::PropertyAttribute] int Rings = 16;

	public:
		CapsuleRenderer();

		void Start() override;
		void Draw() override;

		RAYLIB::Model& GetModel() override;
		RAYLIB::Model* GetModelPtr() override;

		property Engine::Components::Material^ sharedMaterial { Engine::Components::Material^ get() override; }
		property cli::array<Engine::Components::Material^>^ sharedMaterials { cli::array<Engine::Components::Material^>^ get() override; }
	};
}